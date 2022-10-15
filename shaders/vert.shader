#version 330 core
layout(location = 0) in vec3 posVert;
layout(location = 1) in vec3 colVert;
layout(location = 2) in vec2 uvVert;
out vec3 vertColor;
out vec2 uv;
uniform mat4 MVP;
void main() {
  gl_Position = MVP * vec4(posVert, 1.0f);
  vertColor = colVert;
  uv = uvVert;
}
