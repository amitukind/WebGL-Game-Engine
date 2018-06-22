#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec3 vertexPosition;
    layout(location = 1) in vec4 vertexColor;
    
    out vec4 fragmentColor;

    void main()
    {
        gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
        fragmentColor = vertexColor;
    }
#endif

#if defined(FRAGMENT_SHADER)
    in vec4 fragmentColor;
    out vec4 finalColor;

    void main()
    {
        finalColor = fragmentColor;
    }
#endif
