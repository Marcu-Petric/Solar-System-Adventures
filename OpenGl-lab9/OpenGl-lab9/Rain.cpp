#include "Rain.hpp"
#include <cstdlib>
#include <glm/gtc/random.hpp>

Rain::Rain(const glm::vec3 &basePoint, int particleCount)
    : particleCount(particleCount), systemActive(false), basePoint(basePoint)
{
    initSystem();
}

Rain::~Rain()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Rain::initSystem()
{
    elements.clear();
    elements.reserve(particleCount);

    const float spreadX = 100.0f;
    const float spreadZ = 100.0f;
    const float heightOffset = 100.0f;

    for (int i = 0; i < particleCount; ++i)
    {
        RainParticle element;
        float startHeight = basePoint.y + heightOffset + (rand() % 50);

        element.worldPos = glm::vec3(
            basePoint.x + (rand() % (int)spreadX - spreadX / 2),
            startHeight,
            basePoint.z + (rand() % (int)spreadZ - spreadZ / 2));

        element.motion = glm::vec3(0.0f, -25.0f, 0.0f);
        element.duration = 10.0f + ((startHeight - basePoint.y) / 15.0f);
        element.scale = 2.0f + (rand() % 20) / 10.0f;
        elements.push_back(element);
    }

    configureGraphics();
}

void Rain::configureGraphics()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, elements.size() * (sizeof(glm::vec3) + sizeof(float)), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(float), (void *)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Rain::processFrame(float deltaTime, const glm::vec3 &windDirection, float windStrength)
{
    if (!systemActive)
        return;

    const float windFactor = windStrength * 0.2f;
    const float velocityDamping = 0.95f;
    static bool wasWindEnabled = false;
    bool isWindEnabled = windStrength > 0.0f;

    if (wasWindEnabled && !isWindEnabled)
    {
        for (auto &element : elements)
        {
            element.motion = glm::vec3(0.0f, -25.0f, 0.0f);
        }
    }
    wasWindEnabled = isWindEnabled;

    for (auto &element : elements)
    {
        if (isWindEnabled)
        {
            element.motion.x = element.motion.x * velocityDamping + windDirection.x * windFactor;
            element.motion.z = element.motion.z * velocityDamping + windDirection.z * windFactor;
        }
        else
        {
            element.motion.x = 0.0f;
            element.motion.z = 0.0f;
            element.motion.y = -25.0f;
        }

        element.worldPos += element.motion * deltaTime;
        element.duration -= deltaTime;

        // Reset when hitting platform or lifetime expires
        if (element.duration <= 0.0f || element.worldPos.y < basePoint.y)
        {
            float newHeight = basePoint.y + 100.0f + (rand() % 50);

            element.worldPos = glm::vec3(
                basePoint.x + (rand() % 100 - 50),
                newHeight,
                basePoint.z + (rand() % 100 - 50));

            if (isWindEnabled)
            {
                element.motion = glm::vec3(
                    windDirection.x * windStrength * 0.1f,
                    -25.0f,
                    windDirection.z * windStrength * 0.1f);
            }
            else
            {
                element.motion = glm::vec3(0.0f, -25.0f, 0.0f);
            }

            element.duration = 20.0f + ((newHeight - basePoint.y) / 15.0f);
        }
    }

    refreshGraphics();
}

void Rain::refreshGraphics()
{
    static std::vector<float> data;
    data.clear();
    data.reserve(elements.size() * 4);

    for (const auto &element : elements)
    {
        data.push_back(element.worldPos.x);
        data.push_back(element.worldPos.y);
        data.push_back(element.worldPos.z);
        data.push_back(element.scale);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}

void Rain::display()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, elements.size());
}