#version 330 core
in vec2 uv;
out vec4 fragColor;
uniform sampler2D textureScreen;
void main() {
    fragColor = texture(textureScreen, uv);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    fragColor = vec4(average, average, average, 1.0);
}
