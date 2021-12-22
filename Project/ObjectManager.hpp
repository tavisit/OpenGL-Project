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
#include "InGameObject.h"

#include <iostream>
#include <string>

#define STREET_LENGTH 20
#define WALL_LENGTH 20
#define GATES_LENGTH 4
#define BUILDINGS_LENGTH 20
#define LIGHTS_LENGTH 20

namespace gps {

    class ObjectManager
    {
    public:
        ObjectManager();
        ObjectManager(gps::Camera myCamera);
        void initFBO();
        void initializeShaders();
        void renderObjects();
        void initModels();
        void initShaders();
        void initUniforms(gps::Camera myCamera, gps::Window myWindow);
        void renderScene(gps::Window myWindow);

        void setCamera(gps::Camera myCamera);
    private:

        glm::mat4 computeLightSpaceTrMatrix();
        const unsigned int SHADOW_WIDTH = 4096;
        const unsigned int SHADOW_HEIGHT = 2048;

        // object
        gps::InGameObject streetsObject[STREET_LENGTH];
        gps::InGameObject wallsObject[WALL_LENGTH];
        gps::InGameObject gatesObject[GATES_LENGTH];
        gps::InGameObject buildingsObject[BUILDINGS_LENGTH];
        gps::InGameObject lights[LIGHTS_LENGTH];

        // shaders
        gps::Shader shader;
        gps::Shader lightShader;
        gps::Shader depthMapShader;

        // matrices
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat3 normalMatrix;
        glm::mat3 lightDirMatrix;

        // shader uniforms
        GLuint modelLoc;
        GLuint viewLoc;
        GLuint normalMatrixLoc;
        GLuint lightDirMatrixLoc;
        GLuint projectionLoc;

        // light
        glm::vec3 lightDir;
        GLuint lightDirLoc;
        glm::vec3 lightColor;
        GLuint lightColorLoc;

        //shadows
        GLuint shadowMapFBO;
        GLuint depthMapTexture;


        // spotlight
        float spotlight1;
        float spotlight2;
        glm::vec3 spotLightDirection;
        glm::vec3 spotLightPosition;


        // point lights
        glm::vec3 lightPos1;
        GLuint lightPos1Loc;
        glm::vec3 lightPos2;
        GLuint lightPos2Loc;

        gps::Camera myCamera;

    };
}