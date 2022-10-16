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
    SkyBox(QOpenGLFunctions *funcs, QOpenGLShaderProgram * program);
private:
    void init_buffer();
    void init_shaders();
    void init_textures();
public:
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
