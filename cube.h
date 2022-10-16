#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class Camera;
class Cube
{
public:
    Cube(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program);
public:
    void init_buffer();
    void init_shaders();
    void init_textures();
    void draw(const QMatrix4x4& mvp);
private:
    QOpenGLBuffer *m_vbo;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLShaderProgram *m_program;
    QOpenGLFunctions *m_funcs;
    QOpenGLShader *m_shader_vert, *m_shader_frag;
    QOpenGLTexture *m_texture_face, *m_texture_box;
};

#endif // CUBE_H
