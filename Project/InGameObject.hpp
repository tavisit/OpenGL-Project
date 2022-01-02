#ifndef InGameObject_hpp
#define InGameObject_hpp

#include "Constants.h"
#include "Model3D.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include <vector>
#include <iostream>
#include <string>

namespace gps {
    /// <summary>
    /// Abstractization of the object location, rotation and scale matrices, as well as the generalization of the render flow
    /// </summary>
    class InGameObject
    {
    public:
        /// <summary>
        /// Object constructor
        /// </summary>
        InGameObject();

        /// <summary>
        /// Getter for the location vector
        /// </summary>
        /// <returns></returns>
        glm::vec3 getLocation();

        /// <summary>
        /// Getter for the rotation vector
        /// </summary>
        /// <returns></returns>
        glm::vec3 getRotation();

        /// <summary>
        /// Getter for the scale vector
        /// </summary>
        /// <returns></returns>
        glm::vec3 getScale();

        /// <summary>
        /// Getter for the model matrix
        /// </summary>
        /// <returns></returns>
        glm::mat4 getModel();

        /// <summary>
        /// Method that is called each time the object has changed one of its parameters
        /// </summary>
        void objectChanged();

        /// <summary>
        /// Translate the object on a direction with a given distance and speed
        /// </summary>
        /// <param name="distance></param>
        /// <param name="moveDirection"></param>
        /// <param name="speed"></param>
        void translateDistance(GLfloat distance, MOVE_DIRECTION moveDirection, GLfloat speed = 1.0f);

        /// <summary>
        /// Rotate the object on a direction with a given angle and speed
        /// </summary>
        /// <param name="angle"></param>
        /// <param name="rotationDirection"></param>
        /// <param name="speed"></param>
        void rotationDistance(GLfloat angle, ROTATION_DIRECTION rotationDirection, GLfloat speed = 1.0f);
        /// <summary>
        /// Scale the object a given relative scale
        /// </summary>
        /// <param name="scale"></param>
        void scaleDistance(GLfloat scale);

        /// <summary>
        /// Translate the object to a location in 3D space with a given speed
        /// </summary>
        /// <param name="translation"></param>
        /// <param name="speed"></param>
        void translateAbsolute(glm::vec3 translation, GLfloat speed = 1.0f);

        /// <summary>
        /// Rotate the object with a given rotation in 3D space with speed
        /// </summary>
        /// <param name="rotation"></param>
        /// <param name="speed"></param>
        void rotationAbsolute(glm::vec3 rotation, GLfloat speed = 1.0f);

        /// <summary>
        /// Scale the object with a given scale vector
        /// </summary>
        /// <param name="scale"></param>
        void scaleAbsolute(glm::vec3 scale);

        /// <summary>
        /// Render the object with given parameters
        /// </summary>
        /// <param name="normalMatrixLoc">normal matrix location</param>
        /// <param name="view">View matrix</param>
        /// <param name="model3D">3D model that will be used</param>
        /// <param name="shader">The shader that is used (color or shadow map)</param>
        /// <param name="calculateNormals">Boolean that dictates if it needs to calculate the normals</param>
        void drawObject(GLuint normalMatrixLoc, glm::mat4 view, gps::Model3D* model3D, gps::Shader* shader, bool calculateNormals);
        
        /// <summary>
        /// Setter for the alpha parameter
        /// </summary>
        /// <param name="alpha"></param>
        void setAlpha(float alpha);
    private:

        ////////////////////////////////////////////////////////////////
        // Private variables
        ////////////////////////////////////////////////////////////////

        glm::mat4 model;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        float alpha = 1.0f;
    };
}


#endif // !InGameObject_hpp
