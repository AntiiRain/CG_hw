#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 尽管我们不用它，但 VAO 提供了它

uniform mat4 uMVP;

void main(){
   gl_Position = uMVP * vec4(aPos, 1.0);
}