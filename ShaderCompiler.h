#pragma once
// SlangCompiler.cpp
// A helper class that uses the Slang C API directly to compile shaders
// into GLSL, SPIR-V, or HLSL.


#include <slang.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct ShaderOutput
{
	SlangCompileTarget target = SLANG_TARGET_UNKNOWN;
	std::vector<uint8_t> binaryData; // For SPIR-V

    std::string asText() const
    {
        return std::string(binaryData.begin(), binaryData.end());
	}
};

class SlangCompiler
{
public:
    SlangCompiler();

    ~SlangCompiler();

    // Compile to GLSL text
    std::string compileToGLSL(const std::string& source,
        const std::string& entryPoint,
        const std::string& stage);

    // Compile to HLSL text
    std::string compileToHLSL(const std::string& source,
        const std::string& entryPoint,
        const std::string& stage);

    // Compile to SPIR-V binary
    std::vector<uint8_t> compileToSPIRV(const std::string& source,
        const std::string& entryPoint,
        const std::string& stage);

private:
    slang::IGlobalSession* m_globalSession = nullptr;

    ShaderOutput compile(const std::string& source,
        const std::string& entryPoint,
        const std::string& stage,
        SlangCompileTarget target);

    SlangStage getSlangStage(const std::string& stage);
};


