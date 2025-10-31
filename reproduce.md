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
    Core::ApplicationSpecification appSpec;
    Core::Application application(appSpec);
```