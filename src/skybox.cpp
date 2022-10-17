#include "skybox.h"
SkyBox::SkyBox(QOpenGLFunctions *funcs, QOpenGLShaderProgram * program) {
    m_funcs = funcs;
    m_program = program;
    init_buffer();
    init_shaders();
    init_textures();
}

void SkyBox::init_buffer() {
    const GLfloat vertices[] = {
        -1, -1, -1,   // 前
        +1, -1, -1,
        +1, +1, -1,
        -1, +1, -1,

        +1, -1, +1,   // 后
        -1, -1, +1,
        -1, +1, +1,
        +1, +1, +1,

        -1, -1, +1,  // 左
        -1, -1, -1,
        -1, +1, -1,
        -1, +1, +1,

        +1, -1, -1,   // 右
        +1, -1, +1,
        +1, +1, +1,
        +1, +1, -1,

        -1, +1, -1,   // 上
        +1, +1, -1,
        +1, +1, +1,
        -1, +1, +1,

        -1, -1, +1,   // 下
        +1, -1, +1,
        +1, -1, -1,
        -1, -1, -1,
    };
    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vao->create();
    m_vao->bind();

    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(vertices, 3 * 4 * 6 * sizeof(GLfloat));
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
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
    // 前:posz, 后:negz, 左:negx, 右:posx, 上:posy, 下:negy
    std::vector<QString> paths = {"front.jpg", "back.jpg", "left.jpg", "right.jpg", "top.jpg", "bottom.jpg"};
    const QString prefix = ":/images/";
    for (auto&& str : paths) {
        str.prepend(prefix);
    }
    std::vector<QImage> images;
    for (auto&& str : paths) {
        images.push_back(QImage(str).convertToFormat(QImage::Format_RGB888));
    }

    m_texture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    m_texture->create();
    m_texture->setSize(images[0].width(), images[0].height(), images[0].depth());
    m_texture->setFormat(QOpenGLTexture::RGBFormat);
    m_texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::UInt8);
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[0].constBits());
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[1].constBits());
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[2].constBits());
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[3].constBits());
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[4].constBits());
    m_texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGB,
                       QOpenGLTexture::UInt8, images[5].constBits());

    m_texture->setMinificationFilter(QOpenGLTexture::Linear);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
    m_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
}

void SkyBox::draw(const QMatrix4x4& mvp) {
    m_funcs->glDepthFunc(GL_LEQUAL);
    m_program->removeAllShaders();
    m_program->addShader(m_shader_vert);
    m_program->addShader(m_shader_frag);

    if (!m_program->link()) {
        qDebug() << "SkyBox Link Error " << m_program->log();
    }
    if (!m_program->bind()) {
        qDebug() << "SkyBox Bind Error " << m_program->log();
    }

    m_vao->bind();
    m_program->setUniformValue("MVP", mvp);
    m_texture->bind(m_texture->textureId());
    m_program->setUniformValue("skyBox", m_texture->textureId());
    m_funcs->glDrawArrays(GL_QUADS, 0, 24);
    m_texture->release();
    m_program->release();
    m_vao->release();
    m_funcs->glDepthFunc(GL_LESS);
}
