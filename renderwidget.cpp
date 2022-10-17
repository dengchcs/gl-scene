#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_program(nullptr)
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

    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vao->create();
    m_vao->bind();
    m_vbo->create();
    m_vbo->bind();
    const GLfloat vertices[] = {
        -1, -1, 0, 0,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        -1, +1, 0, 1
    };
    m_vbo->allocate(vertices, 4 * 4 * sizeof(GLfloat));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    m_vbo->release();
    m_vao->release();

    m_vert_shader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_frag_shader = new QOpenGLShader(QOpenGLShader::Fragment);
    m_vert_shader->compileSourceFile(":/shaders/screen.vert.shader");
    m_frag_shader->compileSourceFile(":/shaders/screen.frag.shader");

    m_program = new QOpenGLShaderProgram;

    m_skybox = new SkyBox(this, m_program);
    m_static_cubes.push_back(new Cube(this, m_program, {-0.9, -0.9, -0.9}, 0.2));
    m_static_cubes.push_back(new Cube(this, m_program, {+0.9, -0.9, -0.9}, 0.2));
    m_moving_cube = new Cube(this, m_program, {0.0, -0.95, -0.9}, 0.1);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width(), height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
           qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderWidget::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 model, view, projection;
    model.translate(cam.get_eye());
    view = cam.camera_mat();
    projection.perspective(90, 1.0, 0.1, 10);
    m_skybox->draw(projection * view * model);

    for (std::size_t i = 0; i < m_static_cubes.size(); i++) {
        m_static_cubes[i]->draw(projection * view * model);
    }
    move();
    QMatrix4x4 mv_model;
    mv_model.translate(cam.get_eye());
    mv_model.translate(m_diff);
    m_moving_cube->draw(projection * view * mv_model);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_program->removeAllShaders();
    m_program->addShader(m_vert_shader);
    m_program->addShader(m_frag_shader);
    if (!m_program->link()) {
        qDebug() << "Screen Shader Error";
    }
    if (!m_program->bind()) {
        qDebug() << "Screen Shader Error";
    }
    m_vao->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    m_program->setUniformValue("textureScreen", 0);
    glDrawArrays(GL_QUADS, 0, 4);
    m_vao->release();
    m_program->release();

    update();
}

void RenderWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}

void RenderWidget::move() {
    const float xmin = m_static_cubes[0]->xmax(), xmax = m_static_cubes[1]->xmin();
    const auto current = m_moving_cube->get_center() + m_diff;
    if (current.x() - m_moving_cube->get_len() / 2.0 <= xmin) {
        m_direction = {1, 0, 0};
    }
    if (current.x() + m_moving_cube->get_len() / 2.0 >= xmax) {
        m_direction = {-1, 0, 0};
    }
    const float velocity = (std::abs(current.x()) + 0.02) * 0.1;

    const auto step = m_direction * velocity;
    m_diff += step;
}
