#version 330 core
in vec3 vertColor;
in vec2 uv;
out vec4 fragColor;
uniform sampler2D imgTexture;
void main() {
  fragColor = texture(imgTexture, uv);
}
