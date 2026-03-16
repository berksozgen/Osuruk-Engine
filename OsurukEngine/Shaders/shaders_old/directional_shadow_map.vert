#version 330
//vertex

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 directionalLightTransform; //projection * view of camera

void main()
{
    gl_Position = directionalLightTransform * model * vec4(pos, 1.0); //depth mapi otpmatik olusturuyor opengl
}
