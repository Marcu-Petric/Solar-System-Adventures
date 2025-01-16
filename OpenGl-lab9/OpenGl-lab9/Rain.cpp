#include "Rain.hpp"
#include <random>

Rain::Rain(int count) : particleCount(count), isActive(false)
{
    init();
}

Rain::~Rain()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Rain::init()
{
    droplets.clear();
    droplets.reserve(particleCount);

    // Create many more droplets in a huge volume
    for (int i = 0; i < particleCount; i++)
    {
        Droplet d;
        d.pos = glm::vec3(
            (rand() % 4000 - 2000), // X: -2000 to 2000 (much wider spread)
            (rand() % 1000 + 500),  // Y: 500 to 1500 (much higher spawn)
            (rand() % 4000 - 2000)  // Z: -2000 to 2000 (much wider spread)
        );
        d.size = 8.0f + (rand() % 40) / 10.0f; // Much bigger droplets: 8.0 to 12.0
        droplets.push_back(d);
    }

    // Setup OpenGL buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, droplets.size() * sizeof(Droplet), droplets.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Droplet), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Droplet), (void *)offsetof(Droplet, size));
}

void Rain::update(float deltaTime)
{
    if (!isActive)
        return;

    for (auto &d : droplets)
    {
        // Much faster falling rain
        d.pos.y -= 1000.0f * deltaTime; // Dramatically increased from 500 to 1000

        // Reset droplets that fall below ground
        if (d.pos.y < -20.0f)
        {
            d.pos.y = rand() % 1000 + 500; // Higher respawn
            d.pos.x = rand() % 4000 - 2000;
            d.pos.z = rand() % 4000 - 2000;
        }
    }

    // Update GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, droplets.size() * sizeof(Droplet), droplets.data());
}

void Rain::draw()
{
    if (!isActive)
        return;
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, droplets.size());
}