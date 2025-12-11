#version 130
precision mediump float;

uniform sampler2D texture;
uniform int paletteIndex; // 0..3
uniform float time;       // optionally used for flicker (not required)

vec3 rgb2hsv(vec3 c) {
    float maxC = max(max(c.r, c.g), c.b);
    float minC = min(min(c.r, c.g), c.b);
    float d = maxC - minC;
    float h = 0.0;
    if (d > 0.00001) {
        if (maxC == c.r) h = mod((c.g - c.b) / d, 6.0);
        else if (maxC == c.g) h = (c.b - c.r) / d + 2.0;
        else h = (c.r - c.g) / d + 4.0;
        h /= 6.0;
        if (h < 0.0) h += 1.0;
    }
    float s = (maxC <= 0.00001) ? 0.0 : d / maxC;
    float v = maxC;
    return vec3(h, s, v);
}

vec3 hsv2rgb(vec3 c) {
    float h = c.x * 6.0;
    float s = c.y;
    float v = c.z;
    float i = floor(h);
    float f = h - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * f);
    float t = v * (1.0 - s * (1.0 - f));
    if(i < 0.0) i = 0.0;
    if(i < 1.0) return vec3(v, t, p);
    else if(i < 2.0) return vec3(q, v, p);
    else if(i < 3.0) return vec3(p, v, t);
    else if(i < 4.0) return vec3(p, q, v);
    else if(i < 5.0) return vec3(t, p, v);
    else return vec3(v, p, q);
}

// reference (palette0) and target palettes (palette1..3) as constants
const vec3 A0 = vec3(0.7098039, 0.1921569, 0.1254902);
const vec3 A1 = vec3(0.9176471, 0.6196078, 0.1333333);
const vec3 A2 = vec3(0.4196078, 0.4274510, 0.0);

const vec3 P1_0 = vec3(0.0470588, 0.5764706, 0.0);
const vec3 P1_1 = vec3(1.0, 0.9960784, 1.0);
const vec3 P1_2 = vec3(0.9176471, 0.6196078, 0.1333333);

const vec3 P2_0 = vec3(0.7098039, 0.1921569, 0.1254902);
const vec3 P2_1 = vec3(1.0, 0.9960784, 1.0);
const vec3 P2_2 = vec3(0.9176471, 0.6196078, 0.1333333);

const vec3 P3_0 = vec3(0.0, 0.0, 0.0);
const vec3 P3_1 = vec3(0.9960784, 0.8, 0.773);
const vec3 P3_2 = vec3(0.6, 0.3058824, 0.0);

void main()
{
    vec2 uv = clamp(gl_TexCoord[0].xy, 0.0, 1.0);
    vec4 base = texture2D(texture, uv);

    if (base.a < 0.01) {
        gl_FragColor = base;
        return;
    }

    // compute distances to each reference palette color (in RGB)
    float d0 = distance(base.rgb, A0);
    float d1 = distance(base.rgb, A1);
    float d2 = distance(base.rgb, A2);

    // choose nearest index
    int idx = 0;
    float md = d0;
    if (d1 < md) { md = d1; idx = 1; }
    if (d2 < md) { md = d2; idx = 2; }

    // target palette selection
    vec3 tgt;
    if (paletteIndex == 0) {
        // no swap
        gl_FragColor = base;
        return;
    } else if (paletteIndex == 1) {
        if (idx == 0) tgt = P1_0;
        else if (idx == 1) tgt = P1_1;
        else tgt = P1_2;
    } else if (paletteIndex == 2) {
        if (idx == 0) tgt = P2_0;
        else if (idx == 1) tgt = P2_1;
        else tgt = P2_2;
    } else { // paletteIndex == 3
        if (idx == 0) tgt = P3_0;
        else if (idx == 1) tgt = P3_1;
        else tgt = P3_2;
    }

    // compute HSV deltas between reference and target for this index
    vec3 ref;
    if (idx == 0) ref = A0;
    else if (idx == 1) ref = A1;
    else ref = A2;

    vec3 refHSV = rgb2hsv(ref);
    vec3 tgtHSV = rgb2hsv(tgt);
    // delta: wrap hue difference correctly
    float dh = tgtHSV.x - refHSV.x;
    if (dh > 0.5) dh -= 1.0;
    if (dh < -0.5) dh += 1.0;
    vec3 delta = vec3(dh, tgtHSV.y - refHSV.y, tgtHSV.z - refHSV.z);

    // pixel HSV and apply delta
    vec3 pixHSV = rgb2hsv(base.rgb);
    float newH = pixHSV.x + delta.x;
    // wrap hue
    if (newH < 0.0) newH += 1.0;
    if (newH > 1.0) newH -= 1.0;
    float newS = clamp(pixHSV.y + delta.y, 0.0, 1.0);
    float newV = clamp(pixHSV.z + delta.z, 0.0, 1.0);
    vec3 mapped = hsv2rgb(vec3(newH, newS, newV));

    // blend weight based on distance to reference color (soft edges)
    // choose a normalization factor empirically (0.4 works well for these palettes)
    float maxDist = 0.6;
    float weight = clamp(1.0 - (md / maxDist), 0.0, 1.0);
    // optional: sharpen the transition
    weight = pow(weight, 1.0);

    vec3 outColor = mix(base.rgb, mapped, weight);

    gl_FragColor = vec4(outColor, base.a);
}
