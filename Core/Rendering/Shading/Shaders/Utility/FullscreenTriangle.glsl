#ifndef TRIANGLE_GLSL
#define TRIANGLE_GLSL

#define DRAW_FULLSCREEN_TRIANGLE(UV_OUTPUT)                             \
    UV_OUTPUT.xy = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2); \
	gl_Position = vec4(UV_OUTPUT.xy * 2.0f - 1.0f, 0.0f, 1.0f);         \

#endif