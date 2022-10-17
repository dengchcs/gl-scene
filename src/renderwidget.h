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
    /**
     * @brief 更新运动物体的偏移量; 进行碰撞检测, 更新碰撞信息
     * @return 是否发生了碰撞
     */
    [[nodiscard]] bool move();
private:
    void init_buffers();    // 初始化vao, vbo
    void init_shaders();    // 初始化两个shader
    void init_texture();    // 初始化纹理对象
    void init_fbo();        // 初始化帧缓冲
private:
    QElapsedTimer *m_timer{nullptr};

    // 下面的缓冲、渲染器和纹理用于实现帧缓冲灰度效果
    QOpenGLBuffer *m_vbo{nullptr};
    QOpenGLVertexArrayObject *m_vao{nullptr};
    QOpenGLShader *m_vert_shader{nullptr}, *m_frag_shader{nullptr};
    GLuint m_fbo, m_rbo, m_texture;

    QOpenGLShaderProgram *m_program{nullptr};

    // 界面中实际要绘制的物体: 一个天空盒、多个静态立方体和一个运动的立方体
    SkyBox *m_skybox{nullptr};
    std::vector<Cube*> m_static_cubes;
    Cube *m_moving_cube{nullptr};

    // 用于描述和绘制动态物体: 运动方向、相对于起始点的位置偏移以及碰撞提示信息
    QVector3D m_direction{1, 0, 0};
    QVector3D m_diff{0, 0, 0};
    QString m_collision_info;

    // 于视角变换、界面交互有关的变量
    Camera m_cam;
    int mouse_x{0}, mouse_y{0};
    float m_aspect_ratio{0};
};

#endif // RENDERWIDGET_H
