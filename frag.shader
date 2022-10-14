#version 330 core
in vec3 vertColor;
in vec2 uv;
out vec4 fragColor;
uniform sampler2D faceTexture;
uniform sampler2D boxTexture;
void main() {
  fragColor = mix(texture(faceTexture, uv), texture(boxTexture, uv), 0.5);
}
