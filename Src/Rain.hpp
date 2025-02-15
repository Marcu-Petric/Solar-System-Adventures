#ifndef RAIN_HPP
#define RAIN_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Rain
{
public:
    struct RainParticle
    {
        glm::vec3 worldPos;
        glm::vec3 motion;
        float duration;
        float scale;
    };

    Rain(const glm::vec3 &basePoint, int particleCount = 100000);
    ~Rain();

    void initSystem();
    void configureGraphics();
    void processFrame(float deltaTime, const glm::vec3 &airFlow = glm::vec3(0.0f), float flowStrength = 0.0f);
    void refreshGraphics();
    void display();
    void show() { display(); }
    void switchState() { systemActive = !systemActive; }

    bool isEnabled() const { return systemActive; }
    bool isLightningActive() const { return lightningActive; }
    std::vector<RainParticle> &getParticles() { return elements; }

    void triggerLightning();

private:
    std::vector<RainParticle> elements;
    int particleCount;
    GLuint vao, vbo;
    bool systemActive;
    glm::vec3 basePoint;
    bool lightningActive = false;
    float lightningTimer = 0.0f;
    float nextLightningTime = 5.0f; // Time until next lightning
};

#endif