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
    class InGameObject
    {
    public:
        InGameObject();

        glm::vec3 getLocation();
        glm::vec3 getRotation();
        glm::vec3 getScale();

        glm::mat4 getModel();
        void objectChanged();

        void translateDistance(GLfloat distance, MOVE_DIRECTION moveDirection, GLfloat speed = 1.0f);
        void rotationDistance(GLfloat angle, ROTATION_DIRECTION rotationDirection, GLfloat speed = 1.0f);
        void scaleDistance(GLfloat scale);

        void translateAbsolute(glm::vec3 translation, GLfloat speed = 1.0f);
        void rotationAbsolute(glm::vec3 rotation, GLfloat speed = 1.0f);
        void scaleAbsolute(glm::vec3 scale);

        void drawObject(GLuint normalMatrixLoc, glm::mat4 view, gps::Model3D* model3D, gps::Shader* shader, bool calculateNormals);

        void setAlpha(float alpha);
    private:
        glm::mat4 model;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        float alpha = 1.0f;
    };
}


#endif // !InGameObject_hpp
