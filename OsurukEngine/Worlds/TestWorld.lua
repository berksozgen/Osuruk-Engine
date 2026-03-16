--birinvi yol
--textures = {}
--textures["brick"] = "/Textures/brick.png"
--textures["dirt"] = "/Textures/dirt.png"
--textures["plain"] = "/Textures/plain.png"

--ikinci yol
textures = { brick = "/Textures/brick.png", dirt = "/Textures/dirt.png", plain = "/Textures/plain.png" }

--luayi sikecegim, table icerigi niye random gelitor amk
function LoadTextures()
    for key,value in pairs(textures) do
        --print(value)
        LoadTextureCPP(key, value)
        end
end

--function GetTexture(n)
--    return textures[n]
--end
