#version 330 core
// --- 修改：添加 'flat' ---
flat in vec3 FragPos_World; // 从主导顶点而来 (未插值)
flat in vec3 Normal_World;  // 从主导顶点而来 (未插值)

out vec4 FragColor;

// 光照参数 (和 phong.fs 一样)
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform vec3 uObjectColor;

void main()
{
    // (代码与 phong.fs 完全相同)
    // 甚至不需要 normalize(Normal_World)，因为它没有被插值
    vec3 Normal = normalize(Normal_World);

    // --- 在这里 (片元) 计算 Blinn-Phong 光照 ---
    // (所有像素计算出的光照都将是相同的)

    // 环境光 (Ambient)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;

    // 漫反射 (Diffuse)
    vec3 lightDir = normalize(uLightPos - FragPos_World);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // 镜面光 (Specular) - Blinn-Phong
    vec3 viewDir = normalize(uViewPos - FragPos_World);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * uLightColor;

    // 合并结果
    vec3 result = (ambient + diffuse + specular) * uObjectColor;
    FragColor = vec4(result, 1.0);
}