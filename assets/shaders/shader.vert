#version 450

layout (location = 0) out vec2 uv;

vec2 vertices[3] = {
    vec2(-0.5,0.5),
    vec2(0,-0.5),
    vec2(0.5,0.5)
};

void main()
{
    gl_Position = vec4(vertices[gl_VertexIndex].xy, 1.0, 1.0);
    //uv = vertices[gl_VertexIndex].zw;
}