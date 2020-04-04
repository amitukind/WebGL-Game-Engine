/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderContext.hpp"

// Forward declaration.
namespace System
{
    class FileSystem;
}

/*
    Graphics Shader
    
    Loads and links GLSL shaders into an OpenGL program object.
    Supports geometry, vertex and fragment shaders.
    
    void ExampleGraphicsShader(Graphics::RenderContext* renderContext)
    {
        // Create a shader instance.
        ShaderLoadInfo info;
        info.filePath = "Data/Shader.glsl";

        Graphics::Shader shader;
        shader.Initialize(renderContext, info);

        // Use the created shader in our rendering pipeline.
        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniformIndex("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    }

    ExampleShader.glsl
    [
        \#version 330
        
        #if defined(VERTEX_SHADER)
            uniform mat4 vertexTransform;
        
            layout(location = 0) in vec3 vertexPosition;
        
            void main()
            {
                gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
            }
        #endif
        
        #if defined(FRAGMENT_SHADER)
            out vec4 finalColor;
        
            void main()
            {
                finalColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        #endif
    ]
*/

namespace Graphics
{
    // Forward declarations.
    class RenderContext;

    // Shader class.
    class Shader : private NonCopyable
    {
    public:
        // Load shader from a string.
        struct LoadFromString
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string shaderCode;
        };

        // Load shader from a file.
        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string filePath;
        };

    public:
        Shader();
        ~Shader();

        // Move constructor and assignment.
        Shader(Shader&& other);
        Shader& operator=(Shader&& other);

        // Compiles the shader from a string.
        bool Initialize(const LoadFromString& params);

        // Loads the shader from a file.
        bool Initialize(const LoadFromFile& params);

        // Sets an uniform shader variable.
        template<typename Type>
        void SetUniform(std::string name, const Type& value);

        // Gets an attribute index from the shader program.
        GLint GetAttributeIndex(std::string name) const;

        // Gets an uniform index from the shader program.
        GLint GetUniformIndex(std::string name) const;

        // Gets the shader program handle.
        GLuint GetHandle() const;

        // Checks if the shader is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Program handle.
        GLuint m_handle;
    };

    // Pointer type.
    using ShaderPtr = std::shared_ptr<Shader>;

    // Template implementations.
    template<>
    inline void Shader::SetUniform(std::string name, const GLint& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniform1i(GetUniformIndex(name), value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::vec2& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniform2fv(GetUniformIndex(name), 1, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::mat4& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniformMatrix4fv(GetUniformIndex(name), 1, GL_FALSE, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }
}
