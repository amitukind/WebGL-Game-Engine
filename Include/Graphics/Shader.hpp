/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderContext.hpp"

namespace System
{
    class FileSystem;
}

/*
    Shader
    
    Loads and links GLSL shaders into an OpenGL program object.
    Supports geometry, vertex and fragment shaders.
*/

namespace Graphics
{
    class RenderContext;

    class Shader final : private Common::NonCopyable
    {
    public:
        struct LoadFromString
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string shaderCode;
        };

        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string filePath;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedFilePathResolve,
            InvalidFileContent,
            FailedShaderCreation,
            FailedShaderCompilation,
            FailedProgramCreation,
            FailedProgramLinkage,
        };

        using CreateResult = Common::Result<std::unique_ptr<Shader>, CreateErrors>;
        static CreateResult Create(const LoadFromString& params);
        static CreateResult Create(const LoadFromFile& params);

    public:
        ~Shader();

        template<typename Type>
        void SetUniform(std::string name, const Type& value);

        GLint GetAttributeIndex(std::string name) const;
        GLint GetUniformIndex(std::string name) const;
        GLuint GetHandle() const;

    private:
        Shader();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
    };

    using ShaderPtr = std::shared_ptr<Shader>;

    template<>
    inline void Shader::SetUniform(std::string name, const GLint& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniform1i(GetUniformIndex(name), value);
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::vec2& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniform2fv(GetUniformIndex(name), 1, glm::value_ptr(value));
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::mat4& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniformMatrix4fv(GetUniformIndex(name), 1, GL_FALSE, glm::value_ptr(value));
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }
}
