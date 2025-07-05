

#ifdef GL_ES
precision mediump float;
#endif

uniform bool hovered;
uniform bool clicked;
uniform vec2 resolution;
uniform float time;
uniform sampler2D background;

void main() {
    vec2 st = gl_FragCoord.st / resolution;
    vec4 color = vec4(st.x * sin(time), st.y * cos(time), 0.5, 0.7) * texture2D(background, gl_TexCoord[0].st);

    if(hovered)
        color.a = 1.0;

    gl_FragColor = color;
}