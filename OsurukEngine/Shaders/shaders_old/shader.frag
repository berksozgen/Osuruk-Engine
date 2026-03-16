#version 330
//fragment

in vec4 vCol;
in vec2 TexCoord;//uv iste
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    Light base;
    vec3 direction;
};

struct PointLight
{
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
};

struct SpotLight
{
    PointLight base;
    vec3 direction;
    float edge;
};

struct OmniShadowMap
{
    samplerCube shadowMap;
    float farPlane;
};

struct Material
{
    float specularIntensity;
    float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture; //aktif texture u cekiyor buradan
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS]; //abow cok karisiyor

uniform Material material;

uniform vec3 eyePosition; //camera position iste amk //buna rotasyon eklesek de tatli olabilir

vec3 gridSamplingDisk[20] = vec3[] //omni directional shadowmapler icin onceden hazirlanmis 20 tane dik yon vektoru, 3 tane for dongusu cagirmak yerine bunu cagiracagiz
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float CalcDirectionalShadowFactor(DirectionalLight light)
{
    vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; //bu da kendi normallestirmemizi yaptiriyo -> soldan saga -1 den 1, yukardan asagi da 1 den -1'e
    projCoords = (projCoords * 0.5) + 0.5; //bu da -1 -> 1 den 0 -> 1 e cekiyor
    
    //float closest = texture(directionalShadowMap, projCoords.xy).r; //ortogranal gorunum oldugu icin bu sekilde cekmek calisiyor, perspektifte patlar ama, .r kismi da renk oldugu icin rgb de geliyor ama eleman .x de ise yaramali dedi
    float current = projCoords.z; //bunlar derinlik (depth) btw // su for dongusu yuzunden bu bosa cikti
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.direction);
    
    float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005); //bunlari hardcode aci yerine kamera acisi falan da alinabilir
    
    float shadow = 0.0f;
    
    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0); //0 mipmap level
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth /*piksel depth*/= texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += current - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if (projCoords.z > 1.0) //far planein kesmedigi yerler icin bu deger
    {
        shadow = 0.0;
    }
    
    return shadow;
}
/*
float CalcPointShadowFactor(PointLight light, int shadowIndex)
{
    vec3 fragToLight = FragPos - light.position;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(eyePosition - FragPos);
    float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= omniShadowMaps[shadowIndex].farPlane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    
    return shadow;
}
 */

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
    vec3 fragToLight = FragPos - light.position;
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    
    float viewDistance = length(eyePosition - FragPos);
    float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0; //0.05; //ornekleme yaricapamiz, hard value yereni kameraya baglicaz bunu
    
    for (int i = 0; i < samples; i++)
    {
        float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= omniShadowMaps[shadowIndex].farPlane; //shadowmap 0 ile 1 arasinda veriyordu, geri uzatiyoruz yani
        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }
    
    shadow /= float(samples);
    return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
    vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
    
    float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f); //A.B = |A||B|cos(angle) => A.B = 1.1.cos(Angle) => max yazma nedenimiz hicbir zaman sifirin altinda olmasini istemiyoruz isigin, renk goturen isik olmaz cunku
    vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);
    
    vec4 specularColour = vec4(0, 0, 0, 0);
    
    if(diffuseFactor > 0.0f)
    {
        vec3 fragToEye = normalize(eyePosition - FragPos);
        vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));
        
        float specularFactor = dot(fragToEye, reflectedVertex); //Yukaridaki ile ayni mantik, ikisi de normalized oldugu icin aci dondurecek bu bize
        if(specularFactor > 0.0f)
        {
            specularFactor = pow(specularFactor, material.shininess);
            specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour)); /*vCol ile carpip tatli disko efekti yapilabiliyor*/
}

vec4 CalcDirectionalLight()
{
    float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
    return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
    vec3 direction = FragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);
    
    float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);
    
    vec4 colour = CalcLightByDirection(pLight.base, direction, shadowFactor);
    float attenuation = pLight.exponent * distance * distance +
                        pLight.linear * distance +
                        pLight.constant; //ax^2 + bx + c iste
    
    return (colour / attenuation);
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
    vec3 rayDirection = normalize(FragPos - sLight.base.position);
    float slFactor = dot(rayDirection, sLight.direction);
    
    if(slFactor > sLight.edge)
    {
        vec4 colour = CalcPointLight(sLight.base, shadowIndex);
        
        return colour * (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge))); //bu garip formul degeri baska bir tabanda olcekliyor, isigin birden kesilmesini onlemek icin var
        
    } else {
        return vec4(0, 0, 0, 0);
    }
}

vec4 CalcPointLights()
{
    vec4 totalColour = vec4(0, 0, 0, 0);
    for(int i = 0; i < pointLightCount; i++) //pointLightCount bunun maxtan az oldugunu hesapladik mi
    {
        totalColour += CalcPointLight(pointLights[i], i);
    }
    
    return totalColour;
}

vec4 CalcSpotLights()
{
    vec4 totalColour = vec4(0, 0, 0, 0);
    for(int i = 0; i < spotLightCount; i++)
    {
        totalColour += CalcSpotLight(spotLights[i], i + pointLightCount);
    }
    
    return totalColour;
}

void main()
{
    vec4 finalColour = CalcDirectionalLight();
    finalColour += CalcPointLights();
    finalColour += CalcSpotLights();
    
    colour = texture(theTexture, TexCoord) * finalColour;
}
