#ifndef STATIC_H
#define STATIC_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class Camera;
class Static
{
public:
    Static(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program);
public:
    void init_buffer();
    void init_shaders();
    void init_textures();
    void draw(const Camera& cam);
private:
    QOpenGLBuffer *m_vbo;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLShaderProgram *m_program;
    QOpenGLFunctions *m_funcs;
    QOpenGLShader *m_shader_vert, *m_shader_frag;
    QOpenGLTexture *m_texture_face;
};

#endif // STATIC_H
