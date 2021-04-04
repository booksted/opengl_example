#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
      return nullptr;
    return std::move(context);
}

bool Context::Init() {
	
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
    // 위에 float 갯수 8 x 4
    GL_STATIC_DRAW, vertices, sizeof(float) *32);

   // m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0); // 위에 float 갯수 8개
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6); // 위에 6개 후에 첫번쨰 텍스쳐
    // 번호 , 차원 수  3차원 2차원

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
    GL_STATIC_DRAW, indices, sizeof(uint32_t) *6);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id : {}", m_program->Get());
	
    glClearColor(0.0f, 0.5f, 0.3f, 0.0f);

    // ... end of Context::Init()
        auto image = Image::Load("./image/container.jpg");
        if (!image) 
            return false;
        SPDLOG_INFO("image: {}x{}, {} channels",
            image->GetWidth(), image->GetHeight(), image->GetChannelCount());


    // 텍스쳐 .get 으로 Uptr 로 받은거 기본으로
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    // slot 셋팅 0번 1번 ...  총 32개까지 가능 그리고 전달
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);


    return true;
}

void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  m_program->Use();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   // GL_LINE_STRIP 체워진 것이 아닌 점과점 사이를 연결시키는 줄만 생성
}