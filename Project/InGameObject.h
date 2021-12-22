#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "Constants.hpp"

#include <iostream>
#include <string>

namespace gps {

    class InGameObject
    {
    public:
        void initializeObject(gps::Model3D gameObject);
        void initializeObject(std::string objectPath, std::string objectFile);
        void initializeUniforms(gps::Shader shader, gps::Camera myCamera, gps::Window myWindow);
        void translateObject(gps::Shader shader, GLfloat distance, MOVE_DIRECTION moveDirection, float speed);
        void translateObject(gps::Shader shader, glm::vec3 absolutePosition);
        void rotateObject(gps::Shader shader,glm::vec3 rotationAngles);
        void rotateObjectWithAngle(gps::Shader shader, glm::vec3 rotationAngles);
        void scaleObject(gps::Shader shader, glm::vec3 scale);
        void renderObject(gps::Shader shader);
        void renderShadow(gps::Shader shaderShadow);
        void cameraMoved(gps::Shader shader, gps::Camera myCamera);

        glm::mat4 getModel();
        glm::mat4 getView();
        glm::mat4 getProjection();
        glm::vec3 getLocation();
        glm::vec3 getRotation();
        glm::vec3 getScale();
        
    private:

        // methods
        void objectTransformed(gps::Shader shader);

        // vectors
        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        // matrices
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;


        // shader uniform locations
        GLint modelLoc;
        GLint viewLoc;
        GLint projectionLoc;
        glm::mat3 normalMatrix;
        GLint normalMatrixLoc;
        
        // object
        gps::Model3D gameObject;

        // constants
        const float farPlane = 200.0f;
    };

}

