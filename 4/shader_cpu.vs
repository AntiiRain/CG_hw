// shader_cpu.vs
#version 330 core
layout (location = 0) in vec3 aPos; // aPos 将是变换后的位置
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
   gl_Position = vec4(aPos, 1.0); // 直接使用，无需矩阵乘法
   ourColor = aColor;
}