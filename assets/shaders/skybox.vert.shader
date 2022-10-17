#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 MVP;
out vec3 texCoord;

void main() {
    vec4 pos = MVP * vec4(position, 1.0f);
    gl_Position = pos.xyww;
    texCoord = position;
}
