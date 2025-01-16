#pragma once
#include "CelestialBody.h"

namespace gps
{
    class Rocket : public CelestialBody
    {
    public:
        Rocket(const std::string &modelPath,
               const glm::vec3 &platformPosition,
               float initialScale = 0.1f);
        ~Rocket();

        void launch(const glm::vec3 &direction, float speed);
        virtual void update(float deltaTime, float speedMultiplier) override;
        void resetToLaunchPad();

    private:
        bool isLaunched;
        glm::vec3 velocity;
        const float GRAVITY = -9.81f;
        const glm::vec3 platformPosition;
        const float LAUNCH_HEIGHT = 1.0f;

        // Sound related members
    };
}