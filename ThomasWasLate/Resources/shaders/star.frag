#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D texture;
uniform float time;
uniform vec3 starColor;
uniform float intensity;
uniform float spikes;
uniform float sharpness;
uniform vec2 spriteSize; // frame size in pixels

void main()
{
    vec2 uv = clamp(gl_TexCoord[0].xy, 0.0, 1.0);
    vec2 center = vec2(0.5, 0.5);
    vec2 p = uv - center;
    p.x *= spriteSize.x / spriteSize.y; // aspect correction

    float r = length(p);
    float a = atan(p.y, p.x);

    float anglePattern = abs(cos(a * spikes * 0.5 + time * 2.0));
    float rays = pow(anglePattern, sharpness);

    float radial = 1.0 / (1.0 + r * 4.0); // tweak if needed
    float flicker = 0.6 + 0.4 * sin(time * 12.0 + r * 60.0);
    float star = rays * radial * flicker * intensity;

    vec4 base = texture2D(texture, uv);
    vec3 glow = starColor * star * base.a;
    vec3 outRGB = clamp(base.rgb + glow, 0.0, 1.0);

    gl_FragColor = vec4(outRGB, base.a);
}
