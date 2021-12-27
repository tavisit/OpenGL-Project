#ifndef ObjectManager_hpp
#define ObjectManager_hpp

#include "Constants.h"
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "DeltaTime.hpp"
#include "InGameObject.hpp"
#include "Colors.hpp"


namespace gps {
    class ObjectManager
    {
    public:
        void renderScene(gps::Window myWindow, gps::Camera myCamera, DeltaTime deltaTime);
        void initObjectManager(gps::Window myWindow, gps::Camera myCamera);
        void resizeWindow(gps::Window myWindow);

        glm::mat4 getModel();
        glm::mat4 getView(gps::Camera myCamera);
        glm::mat4 getProjection(gps::Window myWindow);
    private:

        void initFBO();
        void initModels();
        void initShaders();
        void initUniforms(gps::Window myWindow);
        void initObjectsModels(gps::Window myWindow, gps::Camera myCamera);
        glm::mat4 computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera);
        void changeDynamicComponents();

        gps::Shader mainShader;
        gps::Shader depthMapShader;
        gps::Shader waterShader;

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

        #define WALLS_NUMBER 12
        #define GATES_NUMBER 4
        #define STREETS_NUMBER 40
        #define GRASS_NUMBER 220
        #define BUILDINGS_NUMBER 20

        gps::Model3D grass;
        gps::Model3D wall;
        gps::Model3D wallGate;
        gps::Model3D street;
        gps::Model3D building;
        gps::Model3D waterFountain;
        gps::Model3D waterPool;

        GLfloat lightAngle;

        std::vector<gps::InGameObject> wallObjects;
        std::vector<gps::InGameObject> gatesObjects;
        std::vector<gps::InGameObject> streetsObjects;
        std::vector<gps::InGameObject> grassObjects;
        std::vector<gps::InGameObject> buildingsObjects;
        gps::InGameObject waterFountainObject;
        gps::InGameObject waterPoolObject;

        Colors colorParser;

        // point lights
        struct POINT_LIGHT
        {
            glm::vec3 location = glm::vec3(3.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
        };
        #define LIGHT_MAX 2
        POINT_LIGHT pointLights[LIGHT_MAX];
        GLuint pointLightLoc;
        GLuint pointLightColorLoc;
        GLuint pointLightIntensityLoc;
    };
}

#endif // !ObjectManager_hpp
