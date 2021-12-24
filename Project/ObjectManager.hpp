#ifndef ObjectManager_hpp
#define ObjectManager_hpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Constants.h"
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "DeltaTime.h"
#include "InGameObject.hpp"


namespace gps {
    class ObjectManager
    {
    public:
        void renderScene(gps::Window myWindow, gps::Camera myCamera, DeltaTime deltaTime);
        void initObjectManager(gps::Window myWindow);
        void initObjectsModels(gps::Window myWindow, gps::Camera myCamera, DeltaTime deltaTime);
        void resizeWindow(gps::Window myWindow);



        glm::mat4 getModel();
        glm::mat4 getView(gps::Camera myCamera);
        glm::mat4 getProjection(gps::Window myWindow);
    private:

        void initFBO();
        void initModels();
        void initShaders();
        void initUniforms(gps::Window myWindow);
        glm::mat4 computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera);

        gps::Shader myCustomShader;
        gps::Shader lightShader;
        gps::Shader depthMapShader;

        GLuint shadowMapFBO;
        GLuint depthMapTexture;

        glm::mat4 model;
        GLuint modelLoc;
        glm::mat4 view;
        GLuint viewLoc;
        glm::mat4 projection;
        glm::mat3 normalMatrix;
        glm::mat3 lightDirMatrix;

        glm::vec3 lightDir;
        GLuint lightDirLoc;
        glm::vec3 lightColor;
        GLuint lightColorLoc;

        const unsigned int SHADOW_WIDTH = 1024 * 16;
        const unsigned int SHADOW_HEIGHT = 1024 * 16;

        #define WALLS_NUMBER 20
        #define GATES_NUMBER 4
        #define STREETS_NUMBER 40
        #define GRASS_NUMBER 220
        #define BUILDINGS_NUMBER 20

        gps::Model3D lightCube;
        gps::Model3D grass;
        gps::Model3D wall;
        gps::Model3D wallGate;
        gps::Model3D street;
        gps::Model3D building;

        GLfloat lightAngle;

        std::vector<gps::InGameObject> wallObjects;
        std::vector<gps::InGameObject> gatesObjects;
        std::vector<gps::InGameObject> streetsObjects;
        std::vector<gps::InGameObject> grassObjects;
        std::vector<gps::InGameObject> buildingsObjects;


    };
}

#endif // !ObjectManager_hpp
