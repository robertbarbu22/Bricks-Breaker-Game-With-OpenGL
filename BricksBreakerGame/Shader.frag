// Fragment Shader - affects the color of pixels;

#version 330 core

// Input variables (from Shader.vert);
in vec4 ex_Color;

// Output variables (to the main program);
out vec4 out_Color;    // Updated color;

// Uniform variables;
uniform int codCol;

// Update the color based on codCol;

// Uniform variables;
uniform sampler2D myTexture;

in vec2 tex_Coord;     // Texture coordinate;

void main(void)
{
    switch (codCol)
    {
        case 0:
            out_Color = ex_Color;
            break;
        case 1:
            out_Color = mix(vec4(1.0, 0.65, 0.0, 0.0), vec4(1.0, 1.0, 0.0, 0.0), gl_FragCoord.y / 600.0);
            break;
        case 2:
            out_Color = mix(mix(vec4(0.0, 0.0, 0.8, 0.0), vec4(1.0, 1.0, 1.0, 0.0), 0.5), vec4(0.4, 0.0, 0.8, 0.0), gl_FragCoord.y / 600.0);
            break;
        case 3:
            out_Color = mix(vec4(1.0, 0.0, 0.0, 0.0), vec4(1.0, 0.3, 0.0, 0.0), gl_FragCoord.y / 600.0);
            break;
        case 4:
            out_Color = mix(texture(myTexture, tex_Coord), ex_Color, 0.2);
            break;
        case 5:
            out_Color = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), gl_FragCoord.y / 600.0);
            break;
        case 6:
            out_Color = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, 0.8, 1.0), gl_FragCoord.y / 600.0);
            break;
        default:
            break;
    };
}
