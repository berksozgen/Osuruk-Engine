#version 330
//vertex

layout (location = 0) in vec3 pos; //worldlocation
layout (location = 1) in vec2 tex; //uv coord
layout (location = 2) in vec3 norm; //normal vectors //flat out vec3 Normal; seklinde atama yapar ve fragment shaderda onu cagirirsak flat in diye, bir bugdan dolayi flat shading yapmis oluyoruz
//btw bu layoutlar diger shaderlara normalized device coord diye bi sekilde gidiyormus, ama bizim out degerlerimiz raw haliyle gidiyormus uzerinde herhangi bir islem yapilmadan

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; //teknik olarak pozisyon degil ama enterpolasyon yaptigimiz icin oyle gibi de
out vec4 DirectionalLightSpacePos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 directionalLightTransform; //shadowlari eklemek icin buna burda da ihtiyacamiz var

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
    
    DirectionalLightSpacePos = directionalLightTransform * model * vec4(pos, 1.0); //bu da kameranin gorebilecegi bir sey icin referans veriyor
    
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    
    TexCoord = tex;
    
    Normal = mat3(transpose(inverse(model))) * norm; //buradaki matematik karisik biraz, modeldeki location kismina ihtiyacimiz yok ve o matrisin son kisminda ioldugu icin mat3 donusumu ile onu yok edebiliyoruz, //inverse ve transpose yapma nedenimiz ise, esit olmayan bir sekilde scale atildiginda normallerin yonu degisecek, onu duzeltmek
    
    FragPos = (model * vec4(pos, 1.0)).xyz; //bu da dunya pozisyonu, oburu biraz ekran pozisyonu gibi, bu xyz mantigi da mantikli
}
