#version 330

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


in vec2 fragTexCoord;
out vec4 finalColor;


uniform sampler2D gridBuffer0; //int array
uniform sampler2D typeColors; // vec4 array - RGBA32 - id 0 = first id 1 = second etc


uniform vec2 screenSize;
uniform int cellSize;
uniform ivec2 offset;
uniform float test;
uniform float time;



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

    vec3 col = rgb2hsv(finalColor.rgb);
    col.x = mod(col.x + (gl_FragCoord.x + gl_FragCoord.y)/100.0 + time*0.2, 1.0);
    finalColor.rgb = hsv2rgb(col);
}