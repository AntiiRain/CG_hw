#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 finalColor; // 输出到片元着色器的*插值后*的颜色

uniform mat4 uMVP;
uniform mat4 uModel; // 模型矩阵

// 光照参数
uniform vec3 uLightPos;   // 光源在世界空间的位置
uniform vec3 uViewPos;    // 相机在世界空间的位置
uniform vec3 uLightColor;
uniform vec3 uObjectColor;

void main()
{
    // 1. 变换到世界空间
    vec3 FragPos = vec3(uModel * vec4(aPos, 1.0));
    // 法线需要用 "法线矩阵" 变换，以处理非均匀缩放
    vec3 Normal = mat3(transpose(inverse(uModel))) * aNormal;
    Normal = normalize(Normal);

    // --- 在这里 (顶点) 计算 Blinn-Phong 光照 ---

    // 环境光 (Ambient)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;

    // 漫反射 (Diffuse)
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // 镜面光 (Specular) - Blinn-Phong
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0); // 32 是高光系数
    vec3 specular = spec * uLightColor;

    // 合并结果
    vec3 result = (ambient + diffuse + specular) * uObjectColor;
    finalColor = result;

    // 2. 变换到裁剪空间
    gl_Position = uMVP * vec4(aPos, 1.0);
}