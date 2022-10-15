#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "camera.h"
#include "skybox.h"

#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
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
    QElapsedTimer *m_timer;
    QOpenGLShaderProgram *m_shader;
    SkyBox* m_skybox;

    Camera cam;
    int mouse_x, mouse_y;
    ProjectionType projection_type;
};

#endif // RENDERWIDGET_H
