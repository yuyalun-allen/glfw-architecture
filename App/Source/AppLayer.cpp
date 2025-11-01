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