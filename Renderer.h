#pragma once
#include <string>
#include <cstdint>

/// Abstract Renderer Interface
class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual bool init(uint16_t windowWidth, uint16_t windowHeight) = 0;

    virtual uint32_t createTexture(const std::string& imagePath) = 0;
    // This might be an image view or handle, so we abstract it as uint32_t ID.

    virtual void beginFrame() = 0;   // start recording/clearing
    virtual void clear() = 0;        // clear default framebuffer
    virtual void render() = 0;       // issue draw commands
    virtual void endFrame() = 0;     // present/swap buffers

    // extend with loadShader(), createPipeline(), etc.
    virtual void loadTexture(const std::string& imagePath) = 0;

    // Window resizing
	//virtual void resize(uint16_t newWidth, uint16_t newHeight) = 0; // optional for now

    virtual void waitIdle() = 0; // useful for Vulkan (gpu sync before cleanup)

    virtual void setDebugName(uint32_t resourceID, const std::string& name) = 0;
};
