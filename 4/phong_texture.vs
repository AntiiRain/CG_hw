#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // --- 新增 ---

out vec3 FragPos_World;
out vec3 Normal_World;
out vec2 UVcoord; // --- 新增 ---

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    FragPos_World = vec3(uModel * vec4(aPos, 1.0));
    Normal_World = mat3(transpose(inverse(uModel))) * aNormal;
    UVcoord = aTexCoord; // --- 新增 ---

    gl_Position = uMVP * vec4(aPos, 1.0);
}