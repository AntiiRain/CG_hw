#version 330 core
in vec3 FragPos_World;
in vec3 Normal_World;
in vec2 UVcoord; // --- 新增 ---

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
// uniform vec3 uObjectColor; // --- 移除 (或保留用于镜面反射颜色) ---
uniform sampler2D uTex; // --- 新增 ---

void main()
{
    // --- 新增：从纹理采样颜色 ---
    vec3 texColor = texture(uTex, UVcoord).rgb; //

    vec3 Normal = normalize(Normal_World);

    // 环境光 (Ambient)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor; // (环境光 * 纹理颜色)

    // 漫反射 (Diffuse)
    vec3 lightDir = normalize(uLightPos - FragPos_World);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor; // (漫反射光 * 纹理颜色)

    // 镜面光 (Specular)
    vec3 viewDir = normalize(uViewPos - FragPos_World);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * uLightColor; // (镜面光通常是光的颜色，不受纹理影响)

    // --- 修改：使用 texColor 替换 uObjectColor ---
    vec3 result = (ambient * texColor) + (diffuse * texColor) + specular;
    FragColor = vec4(result, 1.0);
}