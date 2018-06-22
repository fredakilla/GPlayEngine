#pragma once

#include "../core/Base.h"
#include "../renderer/BGFXUniform.h"

namespace gplay
{

class BGFXGpuProgram
{
public:
    BGFXGpuProgram();
    ~BGFXGpuProgram();
    bool set(const char* vshPath, const char* fshPath, const char* defines);
    void bind();
    const bgfx::ProgramHandle getProgram() const;
    const std::vector<Uniform::UniformInfo> getUniformsInfo() const { return _uniformsInfo; }

    bool reload();

    const char* getVertexShaderFile() { return _vshFile.c_str(); }
    const char* getFragmentShaderFile() { return _fshFile.c_str(); }

protected:
    std::vector<Uniform::UniformInfo> _uniformsInfo;

private:
    void getUniformsFromShader(bgfx::ShaderHandle shaderHandle);

    bgfx::ShaderHandle _vsh;
    bgfx::ShaderHandle _fsh;
    bgfx::ShaderHandle _csh;
    bgfx::ProgramHandle _program;

    // use for hot reloading
    std::string _vshFile;
    std::string _fshFile;
    std::string _defines;
};

}

