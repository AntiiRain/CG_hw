#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// 输出到片元着色器
out vec3 FragPos_World; // 片段在世界空间的位置
out vec3 Normal_World;  // 片段在世界空间的法线

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    // 1. 变换到世界空间并传递
    FragPos_World = vec3(uModel * vec4(aPos, 1.0));
    Normal_World = mat3(transpose(inverse(uModel))) * aNormal;

    // 2. 变换到裁剪空间
    gl_Position = uMVP * vec4(aPos, 1.0);
}