//
// Created by XBY on 2021/12/25.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
using namespace std;

int main() {
    //画布
    std::vector<vec3> square = {vec3(-1, -1, 0), vec3(1, -1, 0),
                                vec3(-1, 1, 0),  vec3(1, 1, 0),
                                vec3(-1, 1, 0),  vec3(1, -1, 0)};
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
}