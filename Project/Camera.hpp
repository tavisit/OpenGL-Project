#ifndef Camera_hpp
#define Camera_hpp

#include "Constants.h"

#include <string>

namespace gps {
        
    /// <summary>
    /// Abstractization of the camera concept, containing methods for translation and rotation of the camera
    /// </summary>
    class Camera
    {
    public:
        /// <summary>
        /// Camera constructor
        /// </summary>
        /// <param name="cameraPosition"></param>
        /// <param name="cameraTarget"></param>
        /// <param name="cameraUp"></param>
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);

        /// <summary>
        /// Compute the view matrix
        /// </summary>
        /// <returns>the view matrix, using the glm::lookAt() function</returns>
        glm::mat4 getViewMatrix();

        /// <summary>
        /// Updates the camera internal parameters following a camera move event
        /// </summary>
        /// <param name="direction"></param>
        /// <param name="speed"></param>
        void move(MOVE_DIRECTION direction, float speed);

        /// <summary>
        /// Updates the camera internal parameters following a camera move event
        /// </summary>
        /// <param name="position"></param>
        void move(glm::vec3 position);

        /// <summary>
        /// Updates the camera internal parameters following a camera rotate event
        /// </summary>
        /// <param name="pitch">camera rotation around the y axis</param>
        /// <param name="yaw">camera rotation around the x axis</param>
        void rotate(float pitch, float yaw);

        /// <summary>
        /// Getter for the camera position in 3D space
        /// </summary>
        /// <returns></returns>
        glm::vec3 getCameraPosition();

        /// <summary>
        /// Getter for the camera target vector
        /// </summary>
        /// <returns></returns>
        glm::vec3 getCameraTarget();

        /// <summary>
        /// Getter for the camera front direction vector
        /// </summary>
        /// <returns></returns>
        glm::vec3 getCameraFrontDirection();

        /// <summary>
        /// Setter for the walking variables that dictates if the camera should fly or stay on the ground
        /// </summary>
        /// <param name="walkingVar"></param>
        void setWalkingVar(bool walkingVar);

        /// <summary>
        /// Getter for the walking variable
        /// </summary>
        /// <returns></returns>
        bool getWalkingVar();

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        glm::vec2 getRotationAxis();
        
    private:

        ////////////////////////////////////////////////////////////////
        // Private variables
        ////////////////////////////////////////////////////////////////

        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        glm::vec3 worldUp;

        glm::vec3 lastWalkingPosition;
        glm::vec2 rotationAxis;
        bool walking;
        int cityBorders[4] = { 2,158,-158,158 };
    };
    
}

#endif /* Camera_hpp */
