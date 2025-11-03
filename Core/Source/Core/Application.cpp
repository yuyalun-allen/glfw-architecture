#include "Application.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <memory>
#include <print>
#include <system_error>

#include "Renderer/GLUtils.h"

namespace Core {
    static Application* s_Application = nullptr;

    static void GLFWErrorCallBack(int error, const char* description) {
        std::println(stderr, "[GLFW Error]: {}", description);
    }

    Application::Application(const ApplicationSpecification& specification) 
    :m_Specification(specification) {
        // Let the static pointer points to the memory allocated on the stack.
        // We need to take care of the lifecycle of the object pointer.
        std::println("initialize app...");
        s_Application = this;
        glfwSetErrorCallback(GLFWErrorCallBack);
        glfwInit();
        if (m_Specification.WindowSpec.Title.empty())
            m_Specification.WindowSpec.Title = m_Specification.Name;

        m_Window = std::make_shared<Window>(m_Specification.WindowSpec);
        m_Window->Create();
        Renderer::Utils::InitOpenGLDebugMessageCallback();
    }

    void Application::Run(){
        std::println("app running...");
        m_Running = true;
        float lastTime = (float) glfwGetTime();

        while (m_Running) {
            glfwPollEvents();
            if (m_Window->ShouldClose()) {
                Stop();
                break;
            }

            float currentTime = (float) glfwGetTime();
            float timestep = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;
            for (const auto& layer: m_LayerStack) {
                layer->OnUpdate(timestep);
            }
            for (const auto& layer: m_LayerStack) {
                layer->OnRender();
            }
            m_Window->Update();
        }
    }

    void Application::Stop() {
        std::println("app stopped.");
        m_Running = false;
    }

    Application::~Application() {
        m_LayerStack.clear();
        m_Window->Destroy();
        glfwTerminate();
        s_Application = nullptr;
    }

    glm::vec2 Application::GetFrameBufferSize() const {
        return m_Window->GetFrameBufferSize();
    }

    Application& Application::Get() {
        assert(s_Application);
        return *s_Application;
    }

    float Application::GetTime() {
        return (float) glfwGetTime();
    }
}