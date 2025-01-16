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
        glm::vec3 position;
        glm::vec3 velocity;
        float lifetime;
        float size;
    };

    Rain(const glm::vec3 &platformPosition, int numParticles = 100000);
    ~Rain();

    void initialize();
    void setupBuffers();
    void update(float deltaTime, const glm::vec3 &windDirection = glm::vec3(0.0f), float windStrength = 0.0f);
    void updateBuffer();
    void render();
    void draw() { render(); }
    void toggle() { rainEnabled = !rainEnabled; }

    bool isEnabled() const { return rainEnabled; }

    std::vector<RainParticle> &getParticles() { return rainParticles; }

private:
    std::vector<RainParticle> rainParticles;
    int numParticles;
    GLuint rainVAO, rainVBO;
    bool rainEnabled;
    glm::vec3 platformPos;
};

#endif