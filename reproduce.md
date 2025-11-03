# Reproduce GLFW Architecture demo

I'm super excited to reproduce the glfw architecture demo made by Cherno, a greate game engine developer. This is a guide to fully reproduce the demo step by step (Just like playing with the puzzle).

## Project Setup

First, we need to set up the project structure from scratch.

We only care about the cpp code, so we can steal other build related resources from Cherno's repo.

My initial setup of the project is like this:

```
.
├── App
│   ├── CMakeLists.txt
│   ├── Shaders
│   │   ├── Fragment.glsl
│   │   └── Vertex.glsl
│   └── Source
│       ├── AppLayer.h
│       └── Main.cpp
├── CMakeLists.txt
├── Core
│   ├── CMakeLists.txt
│   ├── Source
│   │   └── Core
│   │       ├── Application.h
│   │       └── Renderer
│   └── vendor
│       └── stb
│           ├── stb_image.cpp
│           └── stb_image.h
├── Dependencies.cmake
├── LICENSE
├── README.md
└── reproduce.md

10 directories, 14 files

```

With `Main.cpp` have the following content:

```cpp
#include <print>
#include "Core/Application.h"
#include "AppLayer.h"

int main() {
    std::println("Hello GLFW!");
}
```

Hopefully if you execute `cmake -B build && cmake --build build && ./build/App/App`, the program would output:

```
Hello GLFW!
```

> If you encountered any error says
```
CMake Error at App/CMakeLists.txt:13 (target_sources):
  Cannot find source file:

    Source/AppLayer.cpp

```
> you just need to comment it out from CML at this time.

## Architecture

As the source shown, we need `ApplicationSpecification` and `Application` two core class implemented in core.

```cpp
#include "Core/Application.h"
#include "AppLayer.h"

int main() {
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "Architecture";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Core::Application application(appSpec);
    application.PushLayer<AppLayer>();
    application.Run();
}
```

Then, we need a bunch of head files containing structural declaration for `Main.cpp`.

Namely, in `Core/Application.h`, we need:

```cpp
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Layer.h"
#include "Window.h"

namespace Core {
    struct ApplicationSpecification {
        std::string Name = "Application";
        WindowSpecification WindowSpec;
    };

    class Application {
    public:
        Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();

        void Run();
        void Stop();

        template<typename TLayer>
        requires(std::is_base_of_v<Layer, TLayer>)
        void PushLayer() {
            m_LayerStack.push_back(std::make_unique<TLayer>());
        }

    private:
        ApplicationSpecification m_Specification;
        std::shared_ptr<Window> m_Window;
        bool m_Running = false;

        std::vector<std::unique_ptr<Layer>> m_LayerStack;
    };

}
```

The `Application` owns a `Window` and a `Layer` stack. So we need to declare both types in `Core/Window.h` and `Core/Layer.h` respectively.

Here is the full declaration in `Window.h`:

```cpp
#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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
```

and in `Layer.h`:

```cpp
#pragma once

#include "Event.h"

namespace Core {
    class Layer {
    public:
        virtual ~Layer() = default;
        virtual void OnEvent() {}
        virtual void OnUpdate(float ts) {}
        virtual void OnRender() {}
    };
}
```

> For simplicity, we temporarily ignore the `Event` declaration by now, which layers need to capture.

Let's come back to the `Main.cpp`, the other head file we need to create is `AppLayer.h` which inherits from `Layer.h` in Core.

```cpp
#pragma once

#include <stdint.h>

#include "Core/Layer.h"

class AppLayer: public Core::Layer {
public:
    AppLayer();
    virtual ~AppLayer();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

private:
    uint32_t m_Shader = 0;
};
```

## Stub implementation

The language server (e.g. clangd) should now not gives you any error any more. But in order to compile and run, we need to provide some stub implementation for those declaration above.

First, in `Application.cpp`:

```cpp
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
```

Next, in `AppLayer.cpp`:

```cpp
#include "AppLayer.h"
#include <print>

AppLayer::AppLayer() {
    std::println("AppLayer created.");
}

AppLayer::~AppLayer() {
    std::println("AppLayer destroyed.");
}

void AppLayer::OnRender() {
    std::println("Rendering...");
}

void AppLayer::OnUpdate(float ts) {
    std::println("Updating...");
}
```

Remember to add this two file to CML's Source, if you've commented out them before.

## Window initialization

We have built the main architecture for a glfw app. Then we are going to make it **actually** run.

First, in the construction process of `Application`, the main task is to create a `Window` with GLFW. In `Application.cpp`:

```cpp
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
```

Further digging into the implementation for `Window`, we should provide following code in `Window.cpp` and `Renderer/GLUtils.cpp`

```cpp
#include "Core/Window.h"

#include <GLFW/glfw3.h>
#include "glad/gl.h"
#include <print>

namespace Core {
    Window::Window(const WindowSpecification& specification)
    :m_Specification(specification) {
    }
    Window::~Window() {
        Destroy();
    }

    void Window::Create() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        m_Handle = glfwCreateWindow(m_Specification.Width, m_Specification.Height,
            m_Specification.Title.c_str(), nullptr, nullptr);
        
        if (!m_Handle) {
            std::println(stderr, "Failed to create GLFW window!");
            assert(false);
        }

        glfwMakeContextCurrent(m_Handle);
        gladLoadGL(glfwGetProcAddress);

        glfwSwapInterval(m_Specification.VSync ? 1 : 0);
    }

    void Window::Update() {
        glfwSwapBuffers(m_Handle);
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_Handle);
    }

    void Window::Destroy() {
        if (m_Handle) 
            glfwDestroyWindow(m_Handle);

        m_Handle = nullptr;
    }
}
```

```cpp
#include "glad/gl.h"
#include <print>
namespace Renderer::Utils {
    //ToString methods are copied from Cherno's repo
	const char* GLDebugSourceToString(GLenum source)
	{
		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:     return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION:     return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER:
			default:                              return "UNKNOWN";
		}
	}

	const char* GLDebugTypeToString(GLenum type)
	{
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UDEFINED BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: 	    return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE:         return "PERFORMANCE";
			case GL_DEBUG_TYPE_OTHER:               return "OTHER";
			case GL_DEBUG_TYPE_MARKER:              return "MARKER";
			default:                                return "UNKNOWN";
		}

	}

	const char* GLDebugSeverityToString(GLenum severity)
	{
		switch (severity) 
		{
			case GL_DEBUG_SEVERITY_HIGH:         return "HIGH";
			case GL_DEBUG_SEVERITY_MEDIUM:       return "MEDIUM";
			case GL_DEBUG_SEVERITY_LOW:          return "LOW";
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			default:                             return "UNKNOWN";
		}
	}
    static void GLDebugCallback(GLenum source,
        GLenum type,
        GLenum id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam) {
        if (severity != GL_DEBUG_SEVERITY_MEDIUM && severity != GL_DEBUG_SEVERITY_HIGH)
            return;

        const char* sourceStr = GLDebugSourceToString(source);
        const char* typeStr = GLDebugTypeToString(type);
        const char* severityStr = GLDebugSeverityToString(severity);

        std::println("[OpenGL] [{} - {} - ({})]: [{}] {}",
            severityStr, typeStr, id, sourceStr, message);

    }
    void InitOpenGLDebugMessageCallback() {
        glDebugMessageCallback(GLDebugCallback, nullptr);
    }
}
```

Then we need a loop to continuously poll events, update status and render frames in `Application.cpp`:

```cpp
    void Application::Run(){
        m_Running = true;
        float lastTime = (float) glfwGetTime();
        std::println("app running...");

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
        m_Window->Destroy();
        glfwTerminate();
        s_Application = nullptr;
    }
```

Now when we run this app, a window would show up and the console keep printing `Rendering...\nUpdating...\n`

## Our rendering layer

By far we've got a black window, next we would render the *fire* in it.