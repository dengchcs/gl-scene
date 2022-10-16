#include "cube.h"

Cube::Cube(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program, const QVector3D& center, const float len)
    : m_program(program)
    , m_funcs(funcs)
    , m_center(center)
    , m_len(len)
{
    init_vertices();
    init_buffer();
    init_shaders();
    init_textures();
}

void Cube::init_vertices() {
    const float xxmin = xmin(), xxmax = xmax();
    const float yymin = ymin(), yymax = ymax();
    const float zzmin = zmin(), zzmax = zmax();
    m_vertices = {
        xxmin, yymin, zzmin, 0, 0, // 0
        xxmin, yymin, zzmax, 1, 0, // 1
        xxmax, yymin, zzmax, 1, 1, // 2
        xxmax, yymin, zzmin, 0, 1, // 3

        xxmin, yymax, zzmin, 0, 0, // 4
        xxmin, yymax, zzmax, 1, 0, // 5
        xxmax, yymax, zzmax, 1, 1, // 6
        xxmax, yymax, zzmin, 0, 1, // 7

        xxmin, yymin, zzmin, 0, 0, // 0
        xxmin, yymin, zzmax, 1, 0, // 1
        xxmin, yymax, zzmax, 1, 1, // 5
        xxmin, yymax, zzmin, 0, 1, // 4

        xxmax, yymin, zzmax, 0, 0, // 2
        xxmax, yymin, zzmin, 1, 0, // 3
        xxmax, yymax, zzmin, 1, 1, // 7
        xxmax, yymax, zzmax, 0, 1, // 6

        xxmin, yymin, zzmax, 0, 0, // 1
        xxmax, yymin, zzmax, 1, 0, // 2
        xxmax, yymax, zzmax, 1, 1, // 6
        xxmin, yymax, zzmax, 0, 1, // 5

        xxmax, yymin, zzmin, 0, 0, // 3
        xxmin, yymin, zzmin, 1, 0, // 0
        xxmin, yymax, zzmin, 1, 1, // 4
        xxmax, yymax, zzmin, 0, 1, // 7
    };
}

void Cube::init_buffer() {

    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);

    m_vao->create();
    m_vao->bind();

    m_vbo->create();
    m_vbo->bind();

    m_vbo->allocate(m_vertices.data(), 5 * 4 * 6 * sizeof(GLfloat));
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
    m_texture_face = new QOpenGLTexture(QImage(":/images/awesomeface.jpg").mirrored());
    m_texture_box = new QOpenGLTexture(QImage(":/images/container.jpg").mirrored());
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
