#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_shader(nullptr)
    , m_skybox(nullptr)
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
    constexpr float step = 0.01;
    switch(event->key()) {
    case Qt::Key_A:
        cam.translate_left(step);
        break;
    case Qt::Key_D:
        cam.translate_left(-step);
        break;
    case Qt::Key_W:
        cam.translate_up(step);
        break;
    case Qt::Key_S:
        cam.translate_up(-step);
        break;
    case Qt::Key_F:
        cam.translate_forward(step);
        break;
    case Qt::Key_B:
        cam.translate_forward(-step);
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
    constexpr int ncube = 4;
    for (int i = 0; i < ncube; i++) {
        m_static_cubes.push_back(new Cube(this, m_shader));
    }
}

void RenderWidget::paintGL() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 model, view, projection;
    model.translate(cam.get_eye());
    view = cam.camera_mat();
    projection.perspective(75, 1.0, 0.1, 10);

    m_skybox->draw(projection * view * model);

    const float trans[] = {-1, -0.5, 0, 0.5};
    for (std::size_t i = 0; i < m_static_cubes.size(); i++) {
        QMatrix4x4 cube_model(model);
        cube_model.translate(trans[i], 0, trans[i]);
        m_static_cubes[i]->draw(projection * view * cube_model);
    }

    update();
}

void RenderWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}
