#ifndef Camera_hpp
#define Camera_hpp

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps
{

    enum MOVE_DIRECTION
    {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_RIGHT,
        MOVE_LEFT
    };

    class Camera
    {

    public:
        // Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        // return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        // update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        // update the camera internal parameters following a camera rotate event
        // yaw - camera rotation around the y axis
        // pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

        // Add zoom method
        void zoom(float yoffset);

        glm::vec3 getCameraPosition() const { return cameraPosition; }
        void setPosition(const glm::vec3 &position);
        void lookAt(const glm::vec3 &target);

        // Add this method to get the FOV
        float getFov() const { return fov; }

        void setTarget(const glm::vec3 &target);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;

        // Add FOV for zoom
        float fov;
        const float MIN_FOV = 1.0f;
        const float MAX_FOV = 90.0f;
        const float ZOOM_SENSITIVITY = 2.0f;
    };
}

#endif /* Camera_hpp */
