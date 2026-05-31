#version 330


in vec2 fragTexCoord;
out vec4 finalColor;


uniform sampler2D gridBuffer0; //int array
uniform sampler2D typeColors; // vec4 array - RGBA32 - id 0 = first id 1 = second etc
uniform int cellSize;
uniform ivec2 offset;



void main()
{
    vec2 gridPos = fragTexCoord / vec2(cellSize);
    vec4 buffer0 = texture(gridBuffer0, gridPos);
    
    finalColor.g = mod(buffer0.g,256);
    finalColor.r = buffer0.g - finalColor.g;

    finalColor.a = mod(buffer0.b,256);
    finalColor.b = buffer0.b - finalColor.a;

    //finalColor = vec4(1.0,0.0,0.0,1.0);
}