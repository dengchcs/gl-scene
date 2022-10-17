#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class Cube
{
public:
    Cube(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program, const QVector3D& center, const float len);
private:
    void init_vertices();
    void init_buffer();
    void init_shaders();
    void init_textures();
public:
    void draw(const QMatrix4x4& mvp);
    QVector3D get_center() const { return m_center; }
    float get_len() const { return m_len; }
    float xmax() const { return m_center.x() + m_len / 2.0; }
    float xmin() const { return m_center.x() - m_len / 2.0; }
    float ymax() const { return m_center.y() + m_len / 2.0; }
    float ymin() const { return m_center.y() - m_len / 2.0; }
    float zmax() const { return m_center.z() + m_len / 2.0; }
    float zmin() const { return m_center.z() - m_len / 2.0; }
private:
    QOpenGLBuffer *m_vbo{nullptr};
    QOpenGLVertexArrayObject *m_vao{nullptr};
    QOpenGLShaderProgram *m_program{nullptr};
    QOpenGLFunctions *m_funcs{nullptr};
    QOpenGLShader *m_shader_vert{nullptr}, *m_shader_frag{nullptr};
    QOpenGLTexture *m_texture_face{nullptr}, *m_texture_box{nullptr};
    std::vector<GLfloat> m_vertices;
    QVector3D m_center{0, 0, 0};
    float m_len{0};
};

#endif // CUBE_H
