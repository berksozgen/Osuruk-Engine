#version 330
//fragment

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float _distance = length(FragPos.xyz - lightPos); //distance glsl keyword imis galiba
    _distance = _distance / farPlane; //0 ile 1 arasina olceklemek icin
    gl_FragDepth = _distance;
}
