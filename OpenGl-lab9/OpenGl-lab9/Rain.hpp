#ifndef RAIN_HPP
#define RAIN_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Rain
{
public:
    struct Droplet
    {
        glm::vec3 pos;
        float size;
    };

    Rain(int count = 10000);
    ~Rain();

    void init();
    void update(float deltaTime);
    void draw();
    void toggle() { isActive = !isActive; }
    bool getStatus() { return isActive; }

private:
    std::vector<Droplet> droplets;
    GLuint vao, vbo;
    bool isActive;
    int particleCount;
};

#endif