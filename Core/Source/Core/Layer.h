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