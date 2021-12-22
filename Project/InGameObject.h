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

#include <iostream>
#include <string>

namespace gps {

    class InGameObject
    {
    public:
        enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT,MOVE_UP,MOVE_DOWN };

        void initializeShader(const char* shaderVertPath, const char* shaderFragPath);
        void initializeObject(gps::Model3D gameObject);
        void initializeObject(const char* objectPath);
        void initializeUniforms(gps::Camera myCamera, gps::Window myWindow);
        void translateObject(GLfloat distance, MOVE_DIRECTION moveDirection, float speed);
        void rotateObject(glm::vec3 rotationAngles);
        void rotateObjectWithAngle(glm::vec3 rotationAngles);
        void scaleObject(glm::vec3 scale);
        void renderObject();
        void renderShadow(gps::Shader shaderShadow);
        void cameraMoved(gps::Camera myCamera);


        gps::Shader getShader();
        void setShader(gps::Shader shader);

        glm::mat4 getModel();
        glm::mat4 getView();
        glm::mat4 getProjection();
        
    private:

        // methods
        void objectTransformed();

        // vectors
        glm::vec3 location;
        glm::vec3 rotation;

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

        // shader
        gps::Shader shader;
        
        // object
        gps::Model3D gameObject;

        // constants
        const float farPlane = 200.0f;
    };

}

