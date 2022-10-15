#include "renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(nullptr)
    , m_vao(nullptr)
    , m_shader(nullptr)
{
    this->setFocusPolicy(Qt::StrongFocus);
    projection_type = ProjectionType::Perspective;
    m_timer = new QElapsedTimer;
    m_timer->start();
}

RenderWidget::~RenderWidget() {
    delete m_timer;
    delete m_vao;
    delete m_vbo;
    delete m_shader;
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_A:
        cam.translate_left(0.2);
        break;
    case Qt::Key_D:
        cam.translate_left(-0.2);
        break;
    case Qt::Key_W:
        cam.translate_up(0.2);
        break;
    case Qt::Key_S:
        cam.translate_up(-0.2);
        break;
    case Qt::Key_F:
        cam.translate_forward(0.2);
        break;
    case Qt::Key_B:
        cam.translate_forward(-0.2);
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
    }
    update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event) {
    const int xnew = event->pos().x();
    const int ynew = event->pos().y();
    constexpr float degree = 3;
    if (std::abs(mouse_x - xnew) >= 3) {
        if (xnew > mouse_x) {
            cam.rotate_left(degree);
        } else {
            cam.rotate_left(-degree);
        }
        mouse_x = xnew;
    }

    if (std::abs(mouse_y - ynew) >= 3) {
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

    m_texture_face = new QOpenGLTexture(QImage(":/images/awesomeface.png"));
    m_texture_box = new QOpenGLTexture(QImage(":/images/container.jpg"));

    m_shader = new QOpenGLShaderProgram;
    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vert.shader");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/frag.shader");
    if (! m_shader->link()) {
        qDebug() << "link falied: " << m_shader->log();
    }


    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vao->create();    // glGenBuffer()
    m_vao->bind();      // glBindBuffer()

    // (1)复制顶点数据到OpenGL的buffer中
    // (2)启用顶点属性
    // (3)指示OpenGL如何把buffer转化成属性值
    m_vbo->create();
    m_vbo->bind();
    const GLfloat VERTEX_DATA[] = {
        1, 1, 1,  1.0f, 0.0f, 0.0f,  0, 0,  // 0
        -1, 1, 1, 1.0f, 0.0f, 0.0f,  1, 0,  // 1
       -1, -1, 1, 1.0f, 0.0f, 0.0f,  1, 1,  // 2
        1, -1, 1, 1.0f, 0.0f, 0.0f,  0, 1,  // 3

        1, 1, -1,  0.0f, 1.0f, 0.0f, 0, 0, // 4
       -1, 1, -1,  0.0f, 1.0f, 0.0f, 1, 0, // 5
       -1, -1, -1, 0.0f, 1.0f, 0.0f, 1, 1, // 6
        1, -1, -1, 0.0f, 1.0f, 0.0f, 0, 1, // 7

        1, 1, 1,   1.0f, 0.0f, 0.0f, 1, 0, // 0
        -1, 1, 1,  1.0f, 0.0f, 0.0f, 1, 1, // 1
        -1, 1, -1, 0.0f, 1.0f, 0.0f, 0, 1, // 5
        1, 1, -1,  0.0f, 1.0f, 0.0f, 0, 0, // 4

        -1, 1, 1,   1.0f, 0.0f, 0.0f, 1, 0, // 1
       -1, -1, 1,   1.0f, 0.0f, 0.0f, 1, 1, // 2
        -1, -1, -1, 0.0f, 1.0f, 0.0f, 0, 1, // 6
        -1, 1, -1,  0.0f, 1.0f, 0.0f, 0, 0, // 5

        -1, -1, 1,  1.0f, 0.0f, 0.0f, 0, 1, // 2
         1, -1, 1,  1.0f, 0.0f, 0.0f, 0, 0, // 3
        1, -1, -1,  0.0f, 1.0f, 0.0f, 1, 0, // 7
        -1, -1, -1, 0.0f, 1.0f, 0.0f, 1, 1, // 6

        1, 1, 1,   1.0f, 0.0f, 0.0f,  0, 1, // 0
        1, -1, 1,  1.0f, 0.0f, 0.0f,  0, 0, // 3
        1, -1, -1, 0.0f, 1.0f, 0.0f,  1, 0, // 7
        1, 1, -1,  0.0f, 1.0f, 0.0f,  1, 1  // 4
    };
    m_vbo->allocate(VERTEX_DATA, 8 * 4 * 6 * sizeof(GLfloat));
    // 0 对应layout(location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
    m_vbo->release();

    m_vao->release();
}

void RenderWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    m_vao->bind();
    m_shader->bind();

    QMatrix4x4 projection;
    if (projection_type == ProjectionType::Perspective) {
        projection.perspective(90.0f, 1, 0.01f, 50.0f);
    } else {
        projection.ortho(-2, 2, -2, 2, 0.01, 50);
    }
    QMatrix4x4 view = cam.camera_mat();
    QMatrix4x4 mvp = projection * view;
    m_shader->setUniformValue("MVP", mvp);

    glActiveTexture(GL_TEXTURE0);
    m_texture_face->bind();
    m_shader->setUniformValue("faceTexture", 0);
    glActiveTexture(GL_TEXTURE1);
    m_texture_box->bind();
    m_shader->setUniformValue("boxTexture", 1);

    glDrawArrays(GL_QUADS, 0, 24);

    m_texture_box->bind();
    m_texture_face->release();
    m_shader->release();
    m_vao->release();

    update();
}

void RenderWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}
