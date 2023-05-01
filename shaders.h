

const char *vertex_shader_0 = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "void main()\n"
    "{\n"
    "    gl_Position.xyz = vertexPosition_modelspace;\n"
    "    gl_Position.w = 1.0;\n"
    "}\n";

const char *fragment_shader_0 =
"#version 330 core\n"
"out vec3 color;\n"
"void main(){\n"
"  color = vec3(1,0,0);\n"
"}\n";