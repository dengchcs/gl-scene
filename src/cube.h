#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

/**
 * @brief 描述一个轴对齐的立方体
 */
class Cube
{
public:
    /**
     * @brief 构造一个可供绘制的立方体对象
     * @param funcs OpenGL函数指针
     * @param program 渲染程序, 会被立方体用于加载其shader并绘制
     * @param center 立方体中心坐标
     * @param len 立方体边长
     */
    Cube(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program, const QVector3D& center, const float len);
private:
    void init_vertices();   // 初始化vertices数组
    void init_buffer();     // 初始化vao, vbo
    void init_shaders();    // 初始化两个shader
    void init_textures();   // 初始化两个纹理
public:
    /**
     * @brief 绘制立方体
     * @param mvp 顶点变换矩阵
     */
    void draw(const QMatrix4x4& mvp);
    [[nodiscard]] QVector3D get_center() const { return m_center; }
    [[nodiscard]] float get_len() const { return m_len; }
private:
    // 获取立方体的边界
    [[nodiscard]] float xmax() const { return m_center.x() + m_len / 2.0; }
    [[nodiscard]] float xmin() const { return m_center.x() - m_len / 2.0; }
    [[nodiscard]] float ymax() const { return m_center.y() + m_len / 2.0; }
    [[nodiscard]] float ymin() const { return m_center.y() - m_len / 2.0; }
    [[nodiscard]] float zmax() const { return m_center.z() + m_len / 2.0; }
    [[nodiscard]] float zmin() const { return m_center.z() - m_len / 2.0; }
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
