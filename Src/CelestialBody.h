#pragma once
#include "Model3D.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace gps
{
    class CelestialBody
    {
    public:
        CelestialBody(const std::string &modelPath, const glm::vec3 &initialPosition, float initialScale = 1.0f, float orbitRadius = 0.0f, float orbitSpeed = 1.0f);
        void draw(gps::Shader &shader);
        virtual void update(float deltaTime, float speedMultiplier);
        void setPosition(const glm::vec3 &newPosition);
        void setScale(float newScale);
        void setOrbitSpeed(float speed);
        glm::vec3 getPosition() const;
        float getScale() const;
        float getOrbitSpeed() const;
        void setOrbitCenter(const glm::vec3 &center) { orbitCenter = center; }
        void setParent(CelestialBody *parent) { parentBody = parent; }
        virtual ~CelestialBody() = default;

    protected:
        Model3D model;
        glm::vec3 position;
        float scale;
        float orbitRadius;                   // Distance from center of orbit
        float orbitSpeed;                    // Revolutions per second
        float currentAngle;                  // Current angle in the orbit
        glm::vec3 orbitCenter;               // Center point of the orbit
        float baseOrbitSpeed;                // Store the original orbit speed
        CelestialBody *parentBody = nullptr; // The body this object orbits around
    };
}
