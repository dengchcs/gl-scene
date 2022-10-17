#ifndef SKYBOX_H
#define SKYBOX_H
#include <vector>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class Camera;
class SkyBox
{
public:
    /**
     * @brief 构造天空盒
     * @param funcs OpenGL函数指针
     * @param program 渲染程序, 会被用于加载天空盒的渲染器并绘制
     */
    SkyBox(QOpenGLFunctions *funcs, QOpenGLShaderProgram * program);
private:
    void init_buffer();     // 初始化 vao, vbo
    void init_shaders();    // 初始化两个shader
    void init_textures();   // 初始化纹理对象
public:
    /**
     * @brief 绘制天空盒
     * @param mvp 顶点变换矩阵
     */
    void draw(const QMatrix4x4& mvp);
private:
    QOpenGLBuffer *m_vbo{nullptr};
    QOpenGLVertexArrayObject *m_vao{nullptr};
    QOpenGLShaderProgram *m_program{nullptr};
    QOpenGLFunctions *m_funcs{nullptr};
    QOpenGLShader *m_shader_vert{nullptr}, *m_shader_frag{nullptr};
    QOpenGLTexture* m_texture{nullptr};
};

#endif // SKYBOX_H
