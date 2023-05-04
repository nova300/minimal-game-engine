

const char *vertex_shader_0 = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "out vec2 uv;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "uniform mat4 MOD;\n"
    "void main()\n"
    "{\n"
    "    mat4 mvp = PRO * VIE * MOD;\n"
    "    gl_Position = mvp * vec4(vertexPosition_modelspace,1);\n"
    "    uv = vertexUV;\n"
    "}\n";

const char *fragment_shader_0 =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec3 color;\n"
"uniform sampler2D texSampler;\n"
"void main(){\n"
"  color = texture( texSampler, uv).rgb;\n"
"}\n";