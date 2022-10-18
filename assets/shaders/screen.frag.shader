#version 330 core
in vec2 uv;
out vec4 fragColor;
uniform sampler2D textureScreen;
uniform float kernelCenter;
const float offset = 1.0 / 300.0;
void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );
    float corner = (1 - kernelCenter) / 12.0;
    float kernel[9] = float[](
        corner, 2 * corner, corner,
        2 * corner, kernelCenter, 2 * corner,
        corner, 2 * corner, corner
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(textureScreen, uv.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    fragColor = vec4(col, 1.0);
}
