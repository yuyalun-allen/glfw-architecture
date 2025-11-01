#include "Application.h"
#include <print>

namespace Core {
    Application::Application(const ApplicationSpecification& specification) 
    :m_Specification(specification) {
        std::println("initialize app...");
    }

    void Application::Run(){
        std::println("app running...");
    }

    void Application::Stop() {
        std::println("app stopped.");
    }

    Application::~Application() {
        std::println("destroy app.");
    }
}