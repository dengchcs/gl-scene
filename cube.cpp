#include "cube.h"

Cube::Cube(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program)
{
    m_funcs = funcs;
    m_program = program;
    init_buffer();
    init_shaders();
    init_textures();
}

void Cube::init_buffer() {

    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);

    m_vao->create();
    m_vao->bind();

    m_vbo->create();
    m_vbo->bind();
    constexpr GLfloat  xmax = -0.8, xmin = -1.0, ymax = -0.8, ymin = -1.0, zmax = 1.0, zmin = 0.8;
    const GLfloat vertices[] = {
        xmin, ymin, zmin, 1, 1, // 0
        xmin, ymin, zmax, 0, 1, // 1
        xmax, ymin, zmax, 0, 0, // 2
        xmax, ymin, zmin, 1, 0, // 3

        xmin, ymin, zmin, 1, 1, // 0
        xmin, ymin, zmax, 0, 1, // 1
        xmin, ymax, zmax, 0, 0, // 5
        xmin, ymax, zmin, 1, 0, // 4

        xmin, ymin, zmax, 1, 1, // 1
        xmax, ymin, zmax, 0, 1, // 2
        xmax, ymax, zmax, 0, 0, // 6
        xmax, ymax, zmax, 1, 0, // 5

        xmax, ymin, zmax, 1, 1, // 2
        xmax, ymin, zmin, 0, 1, // 3
        xmax, ymax, zmin, 0, 0, // 7
        xmax, ymax, zmax, 1, 0, // 6

        xmax, ymin, zmin, 1, 1, // 3
        xmin, ymin, zmin, 0, 1, // 0
        xmin, ymax, zmin, 0, 0, // 4
        xmax, ymax, zmin, 1, 0, // 7

        xmin, ymax, zmin, 1, 1, // 4
        xmin, ymax, zmax, 0, 1, // 5
        xmax, ymax, zmax, 0, 0, // 6
        xmax, ymax, zmin, 1, 0, // 7
    };
    m_vbo->allocate(vertices, 5 * 4 * 6 * sizeof(GLfloat));
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    m_vbo->release();

    m_vao->release();
}

void Cube::init_shaders() {
    m_shader_vert = new QOpenGLShader(QOpenGLShader::Vertex);
    m_shader_frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if (! m_shader_vert->compileSourceFile(":/shaders/cube.vert.shader")) {
        qDebug() << "SkyBox Vert Shader Failed: " << m_shader_vert->log();
    }
    if (! m_shader_frag->compileSourceFile(":/shaders/cube.frag.shader")) {
        qDebug() << "SkyBox Frag Shader Failed: " << m_shader_frag->log();
    }
}

void Cube::init_textures() {
    m_texture_face = new QOpenGLTexture(QImage(":/images/awesomeface.jpg"));
    m_texture_box = new QOpenGLTexture(QImage(":/images/container.jpg"));
}

void Cube::draw(const QMatrix4x4& mvp) {
    m_funcs->glEnable(GL_DEPTH_TEST);
    m_program->removeAllShaders();
    m_program->addShader(m_shader_vert);
    m_program->addShader(m_shader_frag);

    if (!m_program->link()) {
        qDebug() << "SkyBox Link Error " << m_program->log();
    }
    if (!m_program->bind()) {
        qDebug() << "SkyBox Bind Error " << m_program->log();
    }

    m_program->setUniformValue("MVP", mvp);

    m_vao->bind();
    m_texture_face->bind(m_texture_face->textureId());
    m_texture_box->bind(m_texture_box->textureId());
    m_program->setUniformValue("textureFace", m_texture_face->textureId());
    m_program->setUniformValue("textureBox", m_texture_box->textureId());
    m_funcs->glDrawArrays(GL_QUADS, 0, 24);
    m_texture_face->release();
    m_texture_box->release();
    m_vao->release();

    m_program->release();
}
