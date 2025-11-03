#include "AppLayer.h"

#include <cstddef>
#include <print>

#include <glad/gl.h>
#include "Core/Application.h"
#include "Core/Renderer/Shader.h"

AppLayer::AppLayer() {
    std::println("AppLayer created.");
    m_Shader = Renderer::CreateGraphicsShader("App/Shaders/Vertex.glsl", "App/Shaders/Fragment.glsl");
    glCreateVertexArrays(1, &m_VertexArray);
    glCreateBuffers(1, &m_VertexBuffer);

    struct Vertex {
        glm::vec2 Position;
        glm::vec2 TexCoord;
    };

    Vertex vertices[] = {
        { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
        { { 3.0f, -1.0f }, { 2.0f, 0.0f } },
        { { -1.0f, 3.0f }, { 0.0f, 2.0f } },
    };

    glNamedBufferData(m_VertexBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind the VBO to VAO at binding index 0
    glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(Vertex));

    // Enable attributes
    glEnableVertexArrayAttrib(m_VertexArray, 0);
    glEnableVertexArrayAttrib(m_VertexArray, 1);

    // Format: location, size, type, normalized, relative offset
    glVertexArrayAttribFormat(m_VertexArray, 0, 2, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, Position)));
    glVertexArrayAttribFormat(m_VertexArray, 1, 2, GL_FLOAT, GL_FALSE, static_cast<GLuint>(offsetof(Vertex, TexCoord)));

    // Link attribute locations to binding index 0
    glVertexArrayAttribBinding(m_VertexArray, 0, 0);
    glVertexArrayAttribBinding(m_VertexArray, 1, 0);
}

AppLayer::~AppLayer() {
    std::println("AppLayer destroyed.");
    glDeleteVertexArrays(1, &m_VertexArray);
    glDeleteBuffers(1, &m_VertexBuffer);

    glDeleteProgram(m_Shader);
}

void AppLayer::OnRender() {
    std::println("Rendering...");
    glUseProgram(m_Shader);

    // Uniforms
    glUniform1f(0, Core::Application::GetTime());
    glm::vec2 frameBufferSize = Core::Application::Get().GetFrameBufferSize();
    glUniform2f(1, frameBufferSize.x, frameBufferSize.y);
    glViewport(0, 0, static_cast<GLsizei>(frameBufferSize.x), static_cast<GLsizei>(frameBufferSize.y));

    // Render
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(m_VertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void AppLayer::OnUpdate(float ts) {
    std::println("Updating...");
}