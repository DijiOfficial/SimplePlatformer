#ifdef GL_ES
precision mediump float;
#endif

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#version 130

uniform sampler2D texture;
uniform float time;
uniform int paletteIndex;  // 0, 1, 2, or 3

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(texture, texCoord);
    
    if (pixel.a > 0.1)
    {
        // Convert RGB to HSV
        vec3 rgb = pixel.rgb;
        float r = rgb.r;
        float g = rgb.g;
        float b = rgb.b;
        
        float maxC = max(max(r, g), b);
        float minC = min(min(r, g), b);
        float delta = maxC - minC;
        
        float h = 0.0;
        if (delta != 0.0) {
            if (maxC == r) {
                h = mod((g - b) / delta, 6.0) / 6.0;
            } else if (maxC == g) {
                h = ((b - r) / delta + 2.0) / 6.0;
            } else {
                h = ((r - g) / delta + 4.0) / 6.0;
            }
        }
        
        float s = maxC == 0.0 ? 0.0 : delta / maxC;
        float v = maxC;
        
        // Apply palette transformations
        vec3 newRgb = rgb;
        
        if (paletteIndex == 1) {
            // Green palette: Hue shift red/orange to green
            if (h > 0.0 && h < 0.15) {  // Red/Orange range
                h = 0.33;  // Shift to green
                s = s * 0.95;  // Maintain saturation
            }
        } 
        else if (paletteIndex == 2) {
            // Red palette: Darken orange, keep red
            if (h > 0.08 && h < 0.15) {  // Orange range
                h = 0.02;  // Shift to red
                v = v * 0.65;  // Darken significantly
                s = s * 0.8;
            }
        }
        else if (paletteIndex == 3) {
            // Brown/Peach palette: Shift to browns
            if (h > 0.0 && h < 0.15) {  // Red/Orange range
                if (h < 0.05) {  // Pure red -> black/dark
                    v = v * 0.01;  // Very dark
                } else {  // Orange -> brown
                    h = 0.08;
                    v = v * 0.6;  // Darken
                    s = s * 1.0;
                }
            }
        }
        
        // Convert back to RGB
        float c = v * s;
        float hPrime = h * 6.0;
        float x = c * (1.0 - abs(mod(hPrime, 2.0) - 1.0));
        
        vec3 temp;
        if (hPrime < 1.0) temp = vec3(c, x, 0.0);
        else if (hPrime < 2.0) temp = vec3(x, c, 0.0);
        else if (hPrime < 3.0) temp = vec3(0.0, c, x);
        else if (hPrime < 4.0) temp = vec3(0.0, x, c);
        else if (hPrime < 5.0) temp = vec3(x, 0.0, c);
        else temp = vec3(c, 0.0, x);
        
        float m = v - c;
        newRgb = temp + m;
        
        pixel.rgb = newRgb;
    }
    
    gl_FragColor = pixel;
}
