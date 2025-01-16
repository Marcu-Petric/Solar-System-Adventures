#include "CelestialBody.h"

namespace gps
{
    CelestialBody::CelestialBody(const std::string &modelPath, const glm::vec3 &initialPosition, float initialScale, float orbitRadius, float orbitSpeed)
        : position(initialPosition), scale(initialScale), orbitRadius(orbitRadius), orbitSpeed(orbitSpeed), baseOrbitSpeed(orbitSpeed), currentAngle(0.0f), orbitCenter(glm::vec3(0.0f)) // Default orbit around origin
    {
        model.LoadModel(modelPath);
    }

    void CelestialBody::update(float deltaTime, float speedMultiplier)
    {
        // Update orbit angle using the speed multiplier
        currentAngle += baseOrbitSpeed * speedMultiplier * deltaTime;

        // Calculate new position
        if (orbitRadius > 0.0f)
        { // Only orbit if radius > 0
            if (parentBody)
            {
                // Orbit around parent body's position
                orbitCenter = parentBody->getPosition();
            }
            position.x = orbitCenter.x + orbitRadius * cos(currentAngle);
            position.z = orbitCenter.z + orbitRadius * sin(currentAngle);
            // Y position remains unchanged for flat orbital plane
        }
    }

    void CelestialBody::draw(gps::Shader &shader)
    {
        shader.useShaderProgram();

        // Calculate model matrix with scale
        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, position);
        model_matrix = glm::scale(model_matrix, glm::vec3(scale));

        // Set model matrix uniform
        glUniformMatrix4fv(
            glGetUniformLocation(shader.shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model_matrix));

        // Draw the model
        model.Draw(shader);
    }

    void CelestialBody::setPosition(const glm::vec3 &newPosition)
    {
        position = newPosition;
    }

    void CelestialBody::setScale(float newScale)
    {
        scale = newScale;
    }

    void CelestialBody::setOrbitSpeed(float speed)
    {
        baseOrbitSpeed = speed; // Update the base speed
    }

    glm::vec3 CelestialBody::getPosition() const
    {
        return position;
    }

    float CelestialBody::getScale() const
    {
        return scale;
    }

    float CelestialBody::getOrbitSpeed() const
    {
        return baseOrbitSpeed; // Return the base speed
    }
}
