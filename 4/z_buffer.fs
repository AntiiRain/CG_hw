#version 330 core
out vec4 FragColor;

void main(){
    // gl_FragCoord.z 已经是 [0, 1] 区间
    // 越靠近近平面 (near plane)，值越接近 0 (黑色)
    // 越靠近远平面 (far plane)，值越接近 1 (白色)
    float depth = gl_FragCoord.z;
    FragColor = vec4(depth, depth, depth, 1.0);
}