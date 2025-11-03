#pragma once
#include <cstdint>
#include <filesystem>

namespace Renderer {
    uint32_t CreateGraphicsShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
}