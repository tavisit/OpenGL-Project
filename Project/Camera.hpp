#ifndef Camera_hpp
#define Camera_hpp

#include "Constants.h"

#include <string>

namespace gps {
        
    class Camera
    {
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

        glm::vec3 getCameraPosition();
        glm::vec3 getCameraTarget();
        glm::vec3 getCameraFrontDirection();
        void setWalkingVar(bool walkingVar);
        bool getWalkingVar();
        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        glm::vec3 worldUp;

        glm::vec3 lastWalkingPosition;
        bool walking;
        int cityBorders[4] = { 2,158,-158,158 };
    };
    
}

#endif /* Camera_hpp */
