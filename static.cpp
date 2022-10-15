#include "static.h"
#include "camera.h"

Static::Static(QOpenGLFunctions *funcs, QOpenGLShaderProgram *program)
{
    m_funcs = funcs;
    m_program = program;
    init_buffer();
    init_shaders();
    init_textures();
}

void Static::init_buffer() {

    m_vao = new QOpenGLVertexArrayObject;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);

    m_vao->create();
    m_vao->bind();

    m_vbo->create();
    m_vbo->bind();
    const GLfloat vertices[] = {
        0.0, -1.0, 0.0, 0, 0, // 0
        0.0, -1.0, 0.5, 1, 0, // 1
        0.5, -1.0, 0.5, 1, 1, // 2
        0.5, -1.0, 0.0, 0, 1, // 3

        0.0, -1.0, 0.0, 0, 0, // 0
        0.0, -1.0, 0.5, 1, 0, // 1
        0.0, -0.5, 0.5, 1, 1, // 5
        0.0, -0.5, 0.0, 0, 1, // 4

        0.0, -1.0, 0.5, 0, 0, // 1
        0.5, -1.0, 0.5, 1, 0, // 2
        0.5, -0.5, 0.5, 1, 1, // 6
        0.0, -0.5, 0.5, 0, 1, // 5

        0.5, -1.0, 0.5, 0, 0, // 2
        0.5, -1.0, 0.0, 1, 0, // 3
        0.5, -0.5, 0.0, 1, 1, // 7
        0.5, -0.5, 0.5, 0, 1, // 6

        0.5, -1.0, 0.0, 0, 0, // 3
        0.0, -1.0, 0.0, 1, 0, // 0
        0.0, -0.5, 0.0, 1, 1, // 4
        0.5, -0.5, 0.0, 0, 1, // 7

        0.0, -0.5, 0.0, 0, 0, // 4
        0.0, -0.5, 0.5, 1, 0, // 5
        0.5, -0.5, 0.5, 1, 1, // 6
        0.5, -0.5, 0.0, 0, 1, // 7
    };
    m_vbo->allocate(vertices, 5 * 4 * 6 * sizeof(GLfloat));
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    m_vbo->release();

    m_vao->release();
}

void Static::init_shaders() {
    const char *vert_shader = "#version 330 core\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in vec2 uvPos;\n"
            "out vec3 vertColor;\n"
            "out vec2 uv;\n"
            "uniform mat4 MVP;\n"
            "void main() {\n"
            "  gl_Position = MVP * vec4(position, 1.0f);\n"
            "  uv = uvPos;\n"
            "}\n";
    const char *frag_shader = "#version 330 core\n"
            "in vec2 uv;\n"
            "out vec4 fragColor;\n"
            "uniform sampler2D textFace;\n"
            "void main() {\n"
            "  fragColor = texture(textFace, uv);\n"
            "}\n";
    m_shader_vert = new QOpenGLShader(QOpenGLShader::Vertex);
    m_shader_frag = new QOpenGLShader(QOpenGLShader::Fragment);
    if (! m_shader_vert->compileSourceCode(vert_shader)) {
        qDebug() << "SkyBox Vert Shader Failed: " << m_shader_vert->log();
    }
    if (! m_shader_frag->compileSourceCode(frag_shader)) {
        qDebug() << "SkyBox Frag Shader Failed: " << m_shader_frag->log();
    }
}

void Static::init_textures() {
    m_texture_face = new QOpenGLTexture(QImage(":/images/awesomeface.png"));
}

void Static::draw(const Camera& camera) {
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

    QMatrix4x4 model;
    // model.translate(camera.get_eye());
    QMatrix4x4 view;
    view.lookAt(camera.get_eye(), camera.get_center(), camera.get_up());
    QMatrix4x4 projection;
    projection.perspective(90, 1, 0.01, 50);

    m_program->setUniformValue("MVP", projection * view * model);

    m_vao->bind();
    m_texture_face->bind(m_texture_face->textureId());
    m_program->setUniformValue("textFace", m_texture_face->textureId());
    m_funcs->glDrawArrays(GL_QUADS, 0, 24);
    m_texture_face->release();
    m_vao->release();

    m_program->release();
}
