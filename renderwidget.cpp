#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_shader(nullptr)
    , m_skybox(nullptr)
    , m_static(nullptr)
{
    this->setFocusPolicy(Qt::StrongFocus);
    projection_type = ProjectionType::Perspective;
    m_timer = new QElapsedTimer;
    m_timer->start();
}

RenderWidget::~RenderWidget() {
    delete m_timer;
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_A:
        cam.translate_left(0.01);
        break;
    case Qt::Key_D:
        cam.translate_left(-0.01);
        break;
    case Qt::Key_W:
        cam.translate_up(0.01);
        break;
    case Qt::Key_S:
        cam.translate_up(-0.01);
        break;
    case Qt::Key_F:
        cam.translate_forward(0.01);
        break;
    case Qt::Key_B:
        cam.translate_forward(-0.01);
        break;
    case Qt::Key_Z:
        cam.zoom_near(0.1);
        break;
    case Qt::Key_X:
        cam.zoom_near(-0.1);
        break;
    case Qt::Key_Space:
        if (projection_type == ProjectionType::Perspective) {
            projection_type = ProjectionType::Ortho;
        } else {
            projection_type = ProjectionType::Perspective;
        }
        break;
    case Qt::Key_Enter:
        cam = {{0, 0, 1}, {0, 1, 0}, {0, 0, 0}};
        break;
    }
    update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    const int xnew = event->pos().x();
    const int ynew = event->pos().y();
    constexpr float degree = 3;

    const auto xdiff = std::abs(mouse_x - xnew), ydiff = std::abs(mouse_y - ynew);
    if (std::abs(xdiff - ydiff) <= 3) return;
    if (xdiff > ydiff && xdiff >= 3) {
        if (xnew > mouse_x) {
            cam.rotate_left(degree);
        } else {
            cam.rotate_left(-degree);
        }
        mouse_x = xnew;
    }

    if (ydiff > xdiff && ydiff >= 3) {
        if (ynew > mouse_y) {
            cam.rotate_up(-degree);
        } else {
            cam.rotate_up(degree);
        }
        mouse_y = ynew;
    }

    update();
}

void RenderWidget::mousePressEvent(QMouseEvent *event) {
    mouse_x = event->pos().x();
    mouse_y = event->pos().y();
}

void RenderWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    m_shader = new QOpenGLShaderProgram;

    m_skybox = new SkyBox(this, m_shader);
    m_static = new Static(this, m_shader);
}

void RenderWidget::paintGL() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_skybox->draw(cam);
    m_static->draw(cam);

    update();
}

void RenderWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}
