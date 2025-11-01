#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
namespace Core {
    struct WindowSpecification {
        std::string Title;
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool IsResizeable = true;
        bool VSync = true;
    };

    class Window {
    public:
        Window(const WindowSpecification& spec = WindowSpecification());
        ~Window();

        void Create();
        void Destroy();

        void Update();

        glm::vec2 GetFrameBufferSize();

        bool ShouldClose() const;

        GLFWwindow* GetHandle() const { return m_Handle; }
    private:
        WindowSpecification m_Specification;
        GLFWwindow* m_Handle = nullptr;
    };
}