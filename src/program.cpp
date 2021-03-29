#include "program.h"

ProgramUPtr Program::Create(
  const std::vector<ShaderPtr>& shaders) {
  auto program = ProgramUPtr(new Program());
  if (!program->Link(shaders))
    return nullptr;
  return std::move(program);
}

Program::~Program() {
    if (m_program) {
      glDeleteProgram(m_program);
    }
} 
// 소멸자 구현

bool Program::Link(
  const std::vector<ShaderPtr>& shaders) {
  m_program = glCreateProgram();
  for (auto& shader: shaders) // 생략된 형태 4-4 : 14분
      glAttachShader(m_program, shader->Get());
  glLinkProgram(m_program);

      int success = 0;
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success) {
      char infoLog[1024];
      glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
      SPDLOG_ERROR("failed to link program: {}", infoLog);
      return false;
  }
  return true;
}

void Program::Use() const {
    glUseProgram(m_program);
}