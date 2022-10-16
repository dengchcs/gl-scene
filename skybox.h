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
    QOpenGLBuffer *m_vbo;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLShaderProgram *m_program;
    QOpenGLFunctions *m_funcs;
    QOpenGLShader *m_shader_vert, *m_shader_frag;
    QOpenGLTexture* m_texture;
};

#endif // SKYBOX_H
