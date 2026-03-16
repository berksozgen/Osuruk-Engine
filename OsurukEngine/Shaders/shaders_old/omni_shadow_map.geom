#version 330
//geometri

layout (triangles) in; //gelen seyi pek de belirtmemize gerek yok diyo eleman da o nasil caliscak o zaman
layout (triangle_strip, max_vertices=18) out; //bu kisim cikti da karisik gibi

uniform mat4 lightMatrices[6];

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for(int i = 0; i < 3; i++) //giren trianglelar icin iste
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightMatrices[face] * FragPos;
            EmitVertex(); //triangle cizmeye basladi???
        }
        EndPrimitive(); //ve bitti yeni bir tane cizecegiz???
    }
}
