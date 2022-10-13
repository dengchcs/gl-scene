#include "renderwidget.h"
#include <QKeyEvent>

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(nullptr)
    , m_vao(nullptr)
    , m_shader(nullptr)
    , camera_pos{0.0, 0.0, 3.0}
{
    this->setFocusPolicy(Qt::StrongFocus);
    m_timer = new QElapsedTimer;
    m_timer->start();
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Right:
        camera_pos.setX(camera_pos.x() + 0.1f);
        break;
    case Qt::Key_Left:
        camera_pos.setX(camera_pos.x() - 0.1f);
        break;
    case Qt::Key_Up:
        camera_pos.setY(camera_pos.y() + 0.1f);
        break;
    case Qt::Key_Down:
        camera_pos.setY(camera_pos.y() - 0.1f);
        break;
    case Qt::Key_Plus:
        camera_pos.setZ(camera_pos.z() - 0.1f);
        break;
    case Qt::Key_Minus:
        camera_pos.setZ(camera_pos.z() + 0.1f);
        break;
    }
    update();
}

void RenderWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    m_texture = new QOpenGLTexture(QImage(":/images/awesomeface.png"));

    m_shader = new QOpenGLShaderProgram;
    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vert.shader");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/frag.shader");
    if (m_shader->link()) {
        qDebug("shader success");
    } else {
        qDebug("shader failer");
    }


    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_cbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_uvbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_ebo = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    m_vao->create();    // glGenBuffer()
    m_vao->bind();      // glBindBuffer()

    // (1)复制顶点数据到OpenGL的buffer中
    // (2)启用顶点属性
    // (3)指示OpenGL如何把buffer转化成属性值
    m_vbo->create();
    m_vbo->bind();
    constexpr float h = 1.0 / 3.0;
    const float sqrt2 = sqrt(2.0) / 3.0, sqrt6 = sqrt(6.0) / 3.0;
    const GLfloat VERTEX_DATA[] = {
        1, 1, 1, 1.0f, 0.0f, 0.0f,  // 0
        -1, 1, 1, 1.0f, 0.0f, 0.0f,   // 1
       -1, -1, 1, 1.0f, 0.0f, 0.0f,    // 2
        1, -1, 1, 1.0f, 0.0f, 0.0f,  // 3

        1, 1, -1, 0.0f, 1.0f, 0.0f,  // 4
       -1, 1, -1, 0.0f, 1.0f, 0.0f,   // 5
       -1, -1, -1, 0.0f, 1.0f, 0.0f,       // 6
        1, -1, -1, 0.0f, 1.0f, 0.0f,     // 7

        1, 1, 1, 1.0f, 0.0f, 0.0f,  // 0
        -1, 1, 1, 1.0f, 0.0f, 0.0f,   // 1
        -1, 1, -1, 0.0f, 1.0f, 0.0f,   // 5
        1, 1, -1, 0.0f, 1.0f, 0.0f,  // 4

        -1, 1, 1, 1.0f, 0.0f, 0.0f,   // 1
       -1, -1, 1, 1.0f, 0.0f, 0.0f,    // 2
        -1, -1, -1, 0.0f, 1.0f, 0.0f,       // 6
        -1, 1, -1, 0.0f, 1.0f, 0.0f,   // 5

        -1, -1, 1, 1.0f, 0.0f, 0.0f,    // 2
         1, -1, 1, 1.0f, 0.0f, 0.0f,  // 3
        1, -1, -1, 0.0f, 1.0f, 0.0f,     // 7
        -1, -1, -1, 0.0f, 1.0f, 0.0f,       // 6

        1, 1, 1, 1.0f, 0.0f, 0.0f,  // 0
        1, -1, 1, 1.0f, 0.0f, 0.0f,  // 3
        1, -1, -1, 0.0f, 1.0f, 0.0f,  // 7
        1, 1, -1, 0.0f, 1.0f, 0.0f,  // 4
    };
    m_vbo->allocate(VERTEX_DATA, 6 * 4 * 6 * sizeof(GLfloat));
    // 0 对应layout(location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    m_vbo->release();

    m_uvbo->create();
    m_uvbo->bind();
    const GLfloat UV_INIT_DATA[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };
    m_uvbo->allocate(UV_INIT_DATA, 2 * 4 * 6 * sizeof(GLfloat));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    m_uvbo->release();

    m_cbo->create();
    m_cbo->bind();
    static const GLfloat VERTEX_CLOS[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f
    };
    m_cbo->allocate(VERTEX_CLOS, 3 * 4 * sizeof(GLfloat));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    m_cbo->release();

    m_ebo->create();
    m_ebo->bind();
    static const GLuint VERTEX_INDICES[] = {
        0, 1, 2,
        0, 1, 3,
        0, 2, 3,
        1, 2, 3
    };
    m_ebo->allocate(VERTEX_INDICES, 6 * sizeof(GLuint));
    m_ebo->release();

    m_vao->release();
}

void RenderWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    m_vao->bind();
    m_ebo->bind();
    m_shader->bind();
    m_texture->bind();
    QMatrix4x4 mvp;
    mvp.perspective(45.0f, 1, 0.1f, 100.0f);
    QVector3D center(camera_pos);
    center.setZ(0);
    mvp.lookAt(camera_pos, center, {1.0f, 0.0f, 0.0f});
    float seconds = m_timer->elapsed() / 1000.0;
    mvp.rotate(30.0f * seconds, QVector3D{0.7f, 0.5f, 0.3f});
    m_shader->setUniformValue("MVP", mvp);
    glDrawArrays(GL_QUADS, 0, 24);
    m_texture->release();
    m_shader->release();
    m_ebo->release();
    m_vao->release();
    update();
}

void RenderWidget::resizeGL(int w, int h) {
   glViewport(0, 0, w, h);
}
