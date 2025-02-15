#pragma once
#include "CelestialBody.h"

namespace gps
{
    class Moon : public CelestialBody
    {
    public:
        Moon(const std::string &modelPath,
             const glm::vec3 &initialPosition,
             float initialScale = 1.0f,
             float orbitRadius = 0.0f,
             float orbitSpeed = 1.0f)
            : CelestialBody(modelPath, initialPosition, initialScale, orbitRadius, orbitSpeed) {}

        void setParentPlanet(CelestialBody *parent) { parentPlanet = parent; }
        void update(float deltaTime, float speedMultiplier) override;

    private:
        CelestialBody *parentPlanet = nullptr;
    };
}