# MacBundleFixup.cmake
# Usage:
#   cmake -DAPP="/path/to/AppName.app" -DSEARCH_DIRS="/path1;/path2;..." -P MacBundleFixup.cmake

if(NOT APP)
  message(FATAL_ERROR "APP variable must point to the .app bundle directory")
endif()

# Default search dirs hint (Homebrew arm64/intel + build dirs)
if(NOT SEARCH_DIRS)
  set(SEARCH_DIRS 
      "/opt/homebrew/lib"
      "/usr/local/lib"
      "${CMAKE_BINARY_DIR}"
      )
endif()

# Ensure Frameworks dir exists
file(MAKE_DIRECTORY "${APP}/Contents/Frameworks")

# Use BundleUtilities to copy dependent dylibs and fix install names
include(BundleUtilities)
# Let fixup_bundle follow and copy nested items and adjust permissions
set(BU_CHMOD_BUNDLE_ITEMS ON)
# Avoid stripping for easier debugging
set(BU_STRIP FALSE)

# The main executable path inside the bundle (usually <AppName>)
set(EXE "${APP}/Contents/MacOS/${CMAKE_PROJECT_NAME}")
if(NOT EXISTS "${EXE}")
  # Fallback: try to glob one binary in Contents/MacOS
  file(GLOB _bins "${APP}/Contents/MacOS/*")
  list(LENGTH _bins _bins_len)
  if(_bins_len GREATER 0)
    list(GET _bins 0 EXE)
  else()
    message(FATAL_ERROR "Could not locate main executable under ${APP}/Contents/MacOS")
  endif()
endif()

# Run fixup_bundle on the bundle directory; second arg (libs) left empty for auto-discovery
fixup_bundle("${APP}" "" "${SEARCH_DIRS}")

message(STATUS "fixup_bundle completed for: ${APP}")
