#include "Moon.h"

namespace gps
{
    void Moon::update(float deltaTime, float speedMultiplier)
    {
        if (!parentPlanet)
            return;

        // Update orbit angle
        currentAngle += baseOrbitSpeed * speedMultiplier * deltaTime;

        // Get parent planet's position as orbit center
        glm::vec3 parentPos = parentPlanet->getPosition();

        // Calculate position relative to parent planet
        position.x = parentPos.x + orbitRadius * cos(currentAngle);
        position.z = parentPos.z + orbitRadius * sin(currentAngle);
        position.y = parentPos.y; // Stay in the same plane as parent
    }
}