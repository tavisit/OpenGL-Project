#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraUpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        this->worldUp = cameraUp;
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
        this->lastWalkingPosition = this->cameraPosition;
        this->walking = true;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 potentialNewPosition = this->cameraPosition;
        switch (direction) {
        case MOVE_FORWARD:
            potentialNewPosition += this->cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            potentialNewPosition -= this->cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            potentialNewPosition += this->cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            potentialNewPosition -= this->cameraRightDirection * speed;
            break;
        }
        // keep the camera on street level
        if (this->walking)
        {
            if (potentialNewPosition.z < cityBorders[0] || potentialNewPosition.z > cityBorders[1])
                return;
            if (potentialNewPosition.x < cityBorders[2] || potentialNewPosition.x > cityBorders[3])
                return;

            this->cameraPosition.x = potentialNewPosition.x;
            this->cameraPosition.z = potentialNewPosition.z;
        }
        else
        {
            this->cameraPosition = potentialNewPosition;
        }
    }

    void Camera::move(glm::vec3 position)
    {
        this->cameraPosition = position;
    }

    //update the camera internal parameters following a camera rotate event
    //pitch - camera rotation around the x axis
    //yaw - camera rotation around the y axis
    void Camera::rotate(float pitch, float yaw) {

        if (pitch == -90.0f && yaw == 90.0f)
        {
            this->cameraFrontDirection = glm::vec3(0, -1, 0);
            this->cameraRightDirection = glm::vec3(-1,0,0); 
            this->cameraUpDirection = glm::vec3(0, 0, 1);
        }
        else
        {
            this->cameraFrontDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            this->cameraFrontDirection.y = sin(glm::radians(pitch));
            this->cameraFrontDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, this->worldUp));
            this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, this->cameraFrontDirection));
            rotationAxis = glm::vec2(pitch, yaw);
        }
    }
    glm::vec3 Camera::getCameraPosition()
    {
        return this->cameraPosition;
    }
    glm::vec3 Camera::getCameraTarget()
    {
        return this->cameraTarget;
    }
    glm::vec3 Camera::getCameraFrontDirection()
    {
        return cameraFrontDirection;
    }
    void Camera::setWalkingVar(bool walkingVar)
    {
        if (walkingVar != this->walking)
        {
            this->walking = walkingVar;
            if (this->walking)
            {
                this->cameraPosition = this->lastWalkingPosition;
            }
            else
            {
                lastWalkingPosition = this->cameraPosition;

                this->cameraPosition.y = 4.0f;
            }
        }
        
    }
    bool Camera::getWalkingVar()
    {
        return this->walking;
    }
    glm::vec2 Camera::getRotationAxis()
    {
        return rotationAxis;
    }
}