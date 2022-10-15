#include "skybox.h"
#include "camera.h"
SkyBox::SkyBox(QOpenGLFunctions *funcs, QOpenGLShaderProgram * program) {
    m_funcs = funcs;
    m_program = program;
    init_buffer();
    init_shaders();
    init_textures();
}

void SkyBox::init_buffer() {
    const GLfloat vertices[] = {
        -1, -1, -1, 0, 0,
        +1, -1, -1, 1, 0,
        +1, +1, -1, 1, 1,
        -1, +1, -1, 0, 1,

        +1, -1, +1, 0, 0,
        -1, -1, +1, 1, 0,
        -1, +1, +1, 1, 1,
        +1, +1, +1, 0, 1,

        -1, -1, +1, 0, 0,
        -1, -1, -1, 1, 0,
        -1, +1, -1, 1, 1,
        -1, +1, +1, 0, 1,

        +1, -1, -1, 0, 0,
        +1, -1, +1, 1, 0,
        +1, +1, +1, 1, 1,
        +1, +1, -1, 0, 1,

        -1, +1, -1, 0, 0,
        +1, +1, -1, 1, 0,
        +1, +1, +1, 1, 1,
        -1, +1, +1, 0, 1,

        -1, -1, +1, 0, 0,
        +1, -1, +1, 1, 0,
        +1, -1, -1, 1, 1,
        -1, -1, -1, 0, 1,
    };
    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vao->create();
    m_vao->bind();

    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(vertices, 5 * 4 * 6 * sizeof(GLfloat));
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    m_vbo->release();

    m_vao->release();
}

void SkyBox::init_shaders() {
    m_shader_vert = new QOpenGLShader(QOpenGLShader::Vertex);
    m_shader_frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if (! m_shader_vert->compileSourceFile(":/shaders/skybox.vert.shader")) {
        qDebug() << "SkyBox Vert Shader Failed: " << m_shader_vert->log();
    }
    if (! m_shader_frag->compileSourceFile(":/shaders/skybox.frag.shader")) {
        qDebug() << "SkyBox Frag Shader Failed: " << m_shader_frag->log();
    }
}

void SkyBox::init_textures() {
    const std::vector<QString> images = {"front.jpg", "back.jpg", "left.jpg", "right.jpg", "top.jpg", "bottom.jpg"};
    const QString prefix = ":/images/";
    for (std::size_t i = 0; i < images.size(); i++) {
        auto str = images[i];
        str.prepend(prefix);
        m_textures.push_back(new QOpenGLTexture(QImage(str).mirrored()));
    }
    for (auto texture : m_textures) {
        texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
        texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
    }
}

void SkyBox::draw(const Camera& camera) {
    m_funcs->glDisable(GL_DEPTH_TEST);
    m_program->removeAllShaders();
    m_program->addShader(m_shader_vert);
    m_program->addShader(m_shader_frag);

    if (!m_program->link()) {
        qDebug() << "SkyBox Link Error " << m_program->log();
    }
    if (!m_program->bind()) {
        qDebug() << "SkyBox Bind Error " << m_program->log();
    }

    QMatrix4x4 model;
    model.translate(camera.get_eye());
    QMatrix4x4 view;
    view.lookAt(camera.get_eye(), camera.get_center(), camera.get_up());
    QMatrix4x4 projection;
    projection.perspective(90, 1, 0.01, 50);

    m_vao->bind();
    m_program->setUniformValue("MVP", projection * view * model);
    for (int i = 0; i < 6; i++) {
        // m_funcs->glActiveTexture(GL_TEXTURE0);
        m_textures[i]->bind(m_textures[i]->textureId());
        m_program->setUniformValue("skyBox", m_textures[i]->textureId());
        m_funcs->glDrawArrays(GL_QUADS, i * 4, 4);
        m_textures[i]->release();
    }
    m_program->release();
    m_vao->release();
}

