#version 330 core
in vec3 finalColor; // 从顶点着色器插值而来
out vec4 FragColor;

void main()
{
    FragColor = vec4(finalColor, 1.0);
}