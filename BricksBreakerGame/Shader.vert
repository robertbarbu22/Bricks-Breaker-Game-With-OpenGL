// Vertex Shader - affects the geometry of the scene;

#version 330 core

// Input variables (from the main program);
layout(location = 0) in vec4 in_Position;   // Retrieved from the buffer at the first position (0), the attribute containing coordinates;
layout(location = 1) in vec4 in_Color;      // Retrieved from the buffer at the second position (1), the attribute containing color;
layout(location = 2) in vec2 texCoord;      // Retrieved from the buffer at the third position (2), the attribute containing texture coordinates;

// Output variables;
//out vec4 gl_Position;   // Transmits the updated position to the main program;
out vec4 ex_Color;      // Transmits the color (to be modified in Shader.frag);
out vec2 tex_Coord;     // Transmits the texture coordinates (to be modified in Shader.frag);

// Uniform variables;
uniform mat4 myMatrix;

void main(void)
{
    gl_Position = myMatrix * in_Position;
    ex_Color = in_Color;
    tex_Coord = vec2(texCoord.x, 1 - texCoord.y);
}
