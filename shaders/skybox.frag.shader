#version 330 core
uniform samplerCube skyBox;
in vec3 texCoord;
out vec4 fragColor;

void main() {
    fragColor = texture(skyBox, texCoord);
}
