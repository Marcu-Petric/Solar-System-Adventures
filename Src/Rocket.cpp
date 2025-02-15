#include "Rocket.h"
#include <iostream>

namespace gps
{
    Rocket::Rocket(const std::string &modelPath,
                   const glm::vec3 &platformPosition,
                   float initialScale)
        : CelestialBody(modelPath,
                        glm::vec3(platformPosition.x,
                                  platformPosition.y + LAUNCH_HEIGHT,
                                  platformPosition.z),
                        initialScale,
                        0.0f, 0.0f),
          isLaunched(false),
          velocity(0.0f),
          platformPosition(platformPosition)
    {
    }

    Rocket::~Rocket()
    {
        // No specific cleanup needed
    }

    void Rocket::launch(const glm::vec3 &direction, float speed)
    {
        if (!isLaunched)
        {
            std::cout << "Rocket launched with speed: " << speed << std::endl;
            isLaunched = true;
            velocity = glm::normalize(direction) * speed;
        }
    }

    void Rocket::resetToLaunchPad()
    {
        position = glm::vec3(platformPosition.x,
                             platformPosition.y + LAUNCH_HEIGHT,
                             platformPosition.z);
        isLaunched = false;
        velocity = glm::vec3(0.0f);
    }

    void Rocket::update(float deltaTime, float speedMultiplier)
    {
        if (isLaunched)
        {
            velocity.y += GRAVITY * deltaTime;
            position += velocity * deltaTime;

            if (position.y <= platformPosition.y)
            {
                resetToLaunchPad();
            }
        }
        else
        {
            position = glm::vec3(platformPosition.x,
                                 platformPosition.y + LAUNCH_HEIGHT,
                                 platformPosition.z);
        }
    }
}