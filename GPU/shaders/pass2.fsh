#version 330 core

in vec3 pix;
out vec4 fragColor;
out vec4 FragData[gl_MaxDrawBuffers];

uniform sampler2D texPass0;
uniform sampler2D texPass1;
uniform sampler2D texPass2;
uniform sampler2D texPass3;
uniform sampler2D texPass4;
uniform sampler2D texPass5;
uniform sampler2D texPass6;

void main() {
    FragData[0] = vec4(texture(texPass0, pix.xy*0.5+0.5).rgb, 1.0);
}
