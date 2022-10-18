#include "renderwidget.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setGeometry({100, 100, 800, 800});
    m_timer = new QElapsedTimer;
    m_timer->start();
    m_cam.set_limited(true);
    const QString title{"A:左移, D:右移, W:上移, S:下移, F:前进, B:后退, Z:放大, X:缩小;   旋转:按住鼠标并移动"};
    this->setWindowTitle(title);
}

void RenderWidget::init_buffers() {
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
}

void RenderWidget::init_shaders() {
    m_vert_shader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_frag_shader = new QOpenGLShader(QOpenGLShader::Fragment);
    m_vert_shader->compileSourceFile(":/shaders/screen.vert.shader");
    m_frag_shader->compileSourceFile(":/shaders/screen.frag.shader");
}

void RenderWidget::init_texture() {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderWidget::init_fbo() {
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

RenderWidget::~RenderWidget() {
    delete m_timer;
}

void RenderWidget::keyPressEvent(QKeyEvent *event) {
    constexpr float step = 0.01;
    switch(event->key()) {
    case Qt::Key_A:
        m_cam.translate_left(step);
        break;
    case Qt::Key_D:
        m_cam.translate_left(-step);
        break;
    case Qt::Key_W:
        m_cam.translate_up(step);
        break;
    case Qt::Key_S:
        m_cam.translate_up(-step);
        break;
    case Qt::Key_F:
        m_cam.translate_forward(step);
        break;
    case Qt::Key_B:
        m_cam.translate_forward(-step);
        break;
    case Qt::Key_Z:
        m_cam.zoom_near(0.1);
        break;
    case Qt::Key_X:
        m_cam.zoom_near(-0.1);
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
            m_cam.rotate_left(degree);
        } else {
            m_cam.rotate_left(-degree);
        }
        mouse_x = xnew;
    }

    if (ydiff > xdiff && ydiff >= 3) {
        if (ynew > mouse_y) {
            m_cam.rotate_up(-degree);
        } else {
            m_cam.rotate_up(degree);
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

    m_program = new QOpenGLShaderProgram;
    init_buffers();
    init_shaders();
    init_texture();
    init_fbo();

    m_skybox = new SkyBox(this, m_program);

    auto parse_cube = [&](const QString& str) {
        auto nums = str.split(",");
        Q_ASSERT(nums.size() == 4);
        float x = nums[0].toFloat(), y = nums[1].toFloat(), z = nums[2].toFloat(), len = nums[3].toFloat();
        return new Cube(this, m_program, {x, y, z}, len);
    };
    auto parse_direction = [&](const QString& str) {
        auto nums = str.split(",");
        Q_ASSERT(nums.size() == 3);
        float x = nums[0].toFloat(), y = nums[1].toFloat(), z = nums[2].toFloat();
        return QVector3D{x, y, z};
    };

    QString file_name = QFileDialog::getOpenFileName(this, tr("Select Scene Config"), ".", tr("Config Files (*.config)"));
    QFile file(file_name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    while (! in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("#")) continue;
        if (line.startsWith("STATIC")) {
            const int nstatic = line.last(1).toInt();
            for (int i = 0; i < nstatic; i++) {
                line = in.readLine();
                auto cube = parse_cube(line);
                m_static_cubes.push_back(cube);
            }
        }
        if (line == "MOVING") {
            line = in.readLine();
            m_moving_cube = parse_cube(line);
            line = in.readLine();
            m_direction = parse_direction(line);
        }
    }
}

void RenderWidget::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view, projection;
    view = m_cam.camera_mat();
    projection.perspective(90, m_aspect_ratio, 0.1, 10);
    const QMatrix4x4 vp = projection * view;    // 这两个矩阵是共用的

    QMatrix4x4 static_model;
    // static_model.translate(m_cam.get_eye());   // 取消注释后相机无法移动
    m_skybox->draw(vp * static_model);
    for (std::size_t i = 0; i < m_static_cubes.size(); i++) {
        m_static_cubes[i]->draw(vp * static_model);
    }

    const auto collision = move();
    QMatrix4x4 mv_model;
    // mv_model.translate(m_cam.get_eye());
    mv_model.translate(m_diff);
    mv_model.translate(m_moving_cube->get_center());        // 下面三步是为了给动态物体制造自转的效果
    mv_model.rotate(m_timer->elapsed() / 10.0, {0, 1, 0});
    mv_model.translate(-m_moving_cube->get_center());
    m_moving_cube->draw(vp * mv_model);
    if (collision) {
        m_timer->restart();
        QMessageBox msg(this);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setText(m_collision_info);
        msg.exec();
    }

    // 将帧缓冲的数据写到屏幕上, 达到滤镜的效果
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
    float seconds = m_timer->elapsed() / 1000.0;
    float kernelCenter = seconds / (seconds + 1);
    m_program->setUniformValue("kernelCenter", kernelCenter);
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
   m_aspect_ratio = w / (h + 0.0);
}

bool RenderWidget::move() {
    auto calc_velocity = [](const QVector3D& location) {
        float velocity = 0.01 / (location.lengthSquared() + 0.1);
        return velocity;
    };

    auto cube_collision = [this](const Cube* moving, const Cube* cube2) {
        const float dist = (moving->get_center() + m_diff - cube2->get_center()).length();
        return dist <= (moving->get_len() + cube2 ->get_len()) / 1.5;
    };
    bool collision = false;
    const auto current = m_moving_cube->get_center() + m_diff;
    const float lhalf = m_moving_cube->get_len() / 2.0;
    const QString border[] = {"左", "右", "下", "上", "前", "后"};
    for (int i = 0; i < 3; i++) {
        bool out1 = current[i] - lhalf <= -1.05 && m_direction[i] < 0;
        bool out2 = current[i] + lhalf >= 1.05 && m_direction[i] > 0;
        if (out1 || out2) {
            m_direction[i] = -m_direction[i];
            collision = true;
            m_collision_info = "物体到达" + border[i * 2 + out2] + "边界";
        }
    }
    for (std::size_t i = 0; i < m_static_cubes.size(); i++) {
        if (cube_collision(m_moving_cube, m_static_cubes[i])) {
            m_direction = -m_direction;
            collision = true;
            m_collision_info = "物体和第" + QString::number(i + 1) + "个立方体发送碰撞";
        }
    }
/*
    if (collision) {
        const float xrand = QRandomGenerator::global()->bounded(0, 10) / 50.0 - 0.1;
        const float yrand = QRandomGenerator::global()->bounded(0, 10) / 50.0 - 0.1;
        const float zrand = QRandomGenerator::global()->bounded(0, 10) / 50.0 - 0.1;
        m_direction += QVector3D{xrand, yrand, zrand};
    }
*/
    const float velocity = calc_velocity(current);
    const auto step = m_direction * velocity;
    m_diff += step;

    return collision;
}
