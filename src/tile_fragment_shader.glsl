#version 330


in vec2 fragTexCoord;
out vec4 finalColor;


uniform sampler2D gridBuffer0; //int array
uniform sampler2D typeColors; // vec4 array - RGBA32 - id 0 = first id 1 = second etc


uniform vec2 screenSize;
uniform int cellSize;
uniform ivec2 offset;
uniform float test;



void main()
{

    vec2 UV = gl_FragCoord.xy / screenSize;

    //vec2 gridPos = fragTexCoord / vec2(cellSize);

    //vec2 pos = gl_FragCoord.x

    //vec4 buffer0 = texture(gridBuffer0, ivec2(0,0)); // WORKS

    //vec4 buffer0 = texture(gridBuffer0, gl_FragCoord.xy); // BRO WE NEEDED A UV NOT A COORD

    vec4 buffer0 = texture(gridBuffer0, UV);
    
    finalColor.r = mod(buffer0.g,256);
    finalColor.g = buffer0.g - finalColor.r;

    finalColor.b = mod(buffer0.b,256);
    finalColor.a = buffer0.b - finalColor.b;
    
    finalColor = buffer0;
    finalColor.a = 1.0;
    finalColor.r*=255;


    //finalColor = vec4(1.0,0.0,0.0,1.0);

    



    finalColor = vec4(buffer0.r + buffer0.g + buffer0.b + buffer0.a, 0.0,0.0,1.0);

    
    if(buffer0.r == 0.0){
        finalColor = vec4(0.0,0.0,0.0,0.0);
    }
    
    finalColor = buffer0;

    if(test == 1.0){
        finalColor = vec4(0.0,1.0,0.0,1.0);
    }

    
    finalColor = vec4(fragTexCoord/2.0, 0.0, 1.0);
    //finalColor = vec4(gl_FragCoord.xy, 0.0, 1.0);
    finalColor = vec4( UV, 0.0, 1.0);

    //Show buffer debug
    finalColor = buffer0;


    //Show pixel color accurately
    finalColor.r = mod(buffer0.g,256.0);
    finalColor.g = ((buffer0.g - finalColor.r)/256.0);

    finalColor.b = mod(buffer0.b,256.0);
    finalColor.a = ((buffer0.b - finalColor.b)/256.0);

    finalColor /= 255;
    
    //finalColor = vec4(1.0);
    //finalColor.a = 1.0;
}