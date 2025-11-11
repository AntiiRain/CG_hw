#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

flat out vec3 FragPos_World; // 片段在世界空间的位置
flat out vec3 Normal_World;  // 片段在世界空间的法线

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    // (代码与 phong.vs 完全相同)
    FragPos_World = vec3(uModel * vec4(aPos, 1.0));
    Normal_World = mat3(transpose(inverse(uModel))) * aNormal;

    gl_Position = uMVP * vec4(aPos, 1.0);
}