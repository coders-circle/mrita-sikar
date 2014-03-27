#version 330


layout (location = 0) in vec4 vPosition;

uniform mat4 mvp;

out vec3 fTexCoord;

void main(void) {
    fTexCoord.xyz = normalize(vPosition.xyz);
    gl_Position = mvp * vPosition;
}

