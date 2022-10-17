#version 330 core
in vec2 uv;
out vec4 fragColor;
uniform sampler2D textureFace;
uniform sampler2D textureBox;
void main() {
    fragColor = mix(texture(textureFace, uv), texture(textureBox, uv), 0.5);
}
