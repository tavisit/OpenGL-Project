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

        void setDirectionalLightIntensity(GLfloat intensity);
        GLfloat getDirectionalLightIntensity();

        void setAutoDay(bool autoDay);
        bool getAutoDay();

        void setSpotLight(int index,int value);

    private:

        void initFBO();
        void initModels();
        void initShaders();
        void initUniforms(gps::Window myWindow);
        void initObjectsModels(gps::Window myWindow, gps::Camera myCamera);
        glm::mat4 computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera);
        void changeDynamicComponents(gps::DeltaTime deltaTime);
        glm::vec3 getSunPositionByIntensity(GLfloat intensity);

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

        bool autoDay = true;
        gps::Model3D directionalLightSphere;
        gps::InGameObject directionalLightSphereObject;
        glm::vec3 directionalLightPosition;
        GLuint directionalLightPositionLoc;
        glm::vec3 directionalLightColor;
        GLuint directionalLightColorLoc;
        GLfloat directionalLightIntensity = 1.0f;

        #define FOV 45.0f

        const unsigned int SHADOW_WIDTH = 1024 * 32;
        const unsigned int SHADOW_HEIGHT = 1024 * 32;

        #define WALLS_NUMBER 12
        #define GATES_NUMBER 4
        #define STREETS_NUMBER 40
        #define GRASS_NUMBER 210
        #define BUILDINGS_NUMBER 30
        #define WATER_NUMBER 3

        gps::Model3D grass;
        gps::Model3D wall;
        gps::Model3D wallGate;
        gps::Model3D street;
        gps::Model3D building;
        gps::Model3D waterFountain;
        gps::Model3D waterFountainWater;
        gps::Model3D waterPool;
        gps::Model3D insulaRomana;
        gps::Model3D forum;

        std::vector<gps::InGameObject> wallObjects;
        std::vector<gps::InGameObject> gatesObjects;
        std::vector<gps::InGameObject> streetsObjects;
        std::vector<gps::InGameObject> grassObjects;
        std::vector<gps::InGameObject> insulaRomanaObjects;
        std::vector<gps::InGameObject> waterPoolsObject;
        std::vector<gps::InGameObject> streetLampsObjects;
        gps::InGameObject waterFountainObject;
        gps::InGameObject forumObject;

        Colors colorParser;

        // point lights
        #define LIGHT_MAX 6
        struct POINT_LIGHT
        {
            glm::vec3 location = glm::vec3(0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
        };
        gps::Model3D streetLamps;
        POINT_LIGHT pointLights[LIGHT_MAX];
        GLuint pointLightLoc;
        GLuint pointLightColorLoc;
        GLuint pointLightIntensityLoc;

        #define SPOT_LIGHTS_MAX 1
        struct SPOT_LIGHT
        {
            int spotinit;
            glm::vec3 spotLightDirection = glm::vec3(0.0f);;
            glm::vec3 spotLightPosition = glm::vec3(0.0f);
        };
        int spotLightLocationBegin = 60;
        SPOT_LIGHT spotLights[SPOT_LIGHTS_MAX];
    };
}

#endif // !ObjectManager_hpp
