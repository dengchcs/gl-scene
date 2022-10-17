#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "camera.h"
#include "skybox.h"
#include "cube.h"

#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QKeyEvent>
#include <QMouseEvent>

enum class ProjectionType {
    Perspective,
    Ortho
};

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = nullptr);
    ~RenderWidget();
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    void move();
private:
    QOpenGLBuffer *m_vbo{nullptr};
    QOpenGLVertexArrayObject *m_vao{nullptr};
    QElapsedTimer *m_timer{nullptr};
    QOpenGLShader *m_vert_shader{nullptr}, *m_frag_shader{nullptr};
    QOpenGLShaderProgram *m_program{nullptr};
    SkyBox *m_skybox{nullptr};
    std::vector<Cube*> m_static_cubes;
    Cube *m_moving_cube{nullptr};
    QVector3D m_direction{1, 0, 0};
    QVector3D m_diff{0, 0, 0};

    GLuint m_fbo, m_rbo, m_texture;

    Camera cam;
    int mouse_x, mouse_y;
    ProjectionType projection_type;
};

#endif // RENDERWIDGET_H
