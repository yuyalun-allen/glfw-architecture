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

## Main.cpp

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

Then 