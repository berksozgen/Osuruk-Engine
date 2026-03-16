# MacRecursiveSign.cmake
# Recursively sign a .app bundle and all nested dylibs/frameworks with a single identity
# Usage:
#   cmake -DAPP=/path/to/App.app -DIDENTITY="-" -DOPTIONS=runtime -P MacRecursiveSign.cmake

if(NOT APP)
  message(FATAL_ERROR "APP variable must point to the .app bundle directory")
endif()

# Default to ad-hoc identity if not provided
if(NOT IDENTITY)
  set(IDENTITY -)
endif()

# Signing options (empty or e.g. "runtime")
if(OPTIONS)
  set(SIG_OPTS --options "${OPTIONS}")
else()
  set(SIG_OPTS)
endif()

# Helper to sign a single file if it is a Mach-O (skip non-binaries)
function(_sign_file path)
  if(NOT EXISTS "${path}")
    return()
  endif()
  # Use file(MD5) etc. is unnecessary; just try to sign Mach-O files
  # Filter common binary extensions
  get_filename_component(_ext "${path}" EXT)
  if(_ext STREQUAL ".dylib" OR _ext STREQUAL "" OR _ext STREQUAL ".so")
    execute_process(
      COMMAND codesign --force --sign "${IDENTITY}" ${SIG_OPTS} "${path}"
      RESULT_VARIABLE _rc
      OUTPUT_VARIABLE _out
      ERROR_VARIABLE _err
    )
    if(NOT _rc EQUAL 0)
      message(WARNING "codesign failed for ${path}: ${_err}")
    endif()
  endif()
endfunction()

# First sign everything under Contents/Frameworks (follow symlinks to real files)
file(GLOB_RECURSE FRAME_ITEMS FOLLOW_SYMLINKS
  "${APP}/Contents/Frameworks/*"
)
foreach(item IN LISTS FRAME_ITEMS)
  _sign_file("${item}")
endforeach()

# Then sign everything under Contents/MacOS (executables, helpers)
file(GLOB_RECURSE MACOS_ITEMS FOLLOW_SYMLINKS
  "${APP}/Contents/MacOS/*"
)
foreach(item IN LISTS MACOS_ITEMS)
  _sign_file("${item}")
endforeach()

# Finally sign the entire .app bundle (deep is not necessary if we signed children, but harmless)
execute_process(
  COMMAND codesign --force --deep --sign "${IDENTITY}" ${SIG_OPTS} "${APP}"
  RESULT_VARIABLE APP_RC
  OUTPUT_VARIABLE APP_OUT
  ERROR_VARIABLE APP_ERR
)
if(NOT APP_RC EQUAL 0)
  message(WARNING "codesign failed for app bundle: ${APP_ERR}")
endif()

# Verify (best-effort)
execute_process(
  COMMAND codesign --verify --deep --strict --verbose=3 "${APP}"
  RESULT_VARIABLE VER_RC
  OUTPUT_VARIABLE VER_OUT
  ERROR_VARIABLE VER_ERR
)
if(NOT VER_RC EQUAL 0)
  message(WARNING "codesign verify reported issues: ${VER_ERR}")
else()
  message(STATUS "codesign verification OK for: ${APP}")
endif()
