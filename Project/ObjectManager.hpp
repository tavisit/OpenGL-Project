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
#include <irrKlang.h>
using namespace irrklang;


namespace gps {
    /// <summary>
    /// Class that manages all the information flow of the main rendered objects
    /// </summary>
    class ObjectManager
    {
    public:
        /// <summary>
        /// Method to render each frame of the application
        /// </summary>
        /// <param name="myWindow">Instance of a gps::Window class</param>
        /// <param name="myCamera">Instance of a gps::Camera class</param>
        /// <param name="deltaTime">Instance of a gps::DeltaTime class</param>
        void renderScene(gps::Window myWindow, gps::Camera myCamera, gps::DeltaTime deltaTime);

        /// <summary>
        /// Method to initialize the object manager variables
        /// </summary>
        /// <param name="myWindow">Instance of a gps::Window class</param>
        /// <param name="myCamera">Instance of a gps::Camera class</param>
        void initObjectManager(gps::Window myWindow, gps::Camera myCamera);

        /// <summary>
        /// Method to compute the resize of the window
        /// </summary>
        /// <param name="myWindow">Instance of a gps::Window class</param>
        void resizeWindow(gps::Window myWindow);
        
        /// <summary>
        /// Setter for the global light intensity
        /// </summary>
        /// <param name="intensity">Global light intensity new value</param>
        void setDirectionalLightIntensity(GLfloat intensity);

        /// <summary>
        /// Getter for the global light intensity
        /// </summary>
        /// <returns>GLfloat containing the global light intensity</returns>
        GLfloat getDirectionalLightIntensity();

        /// <summary>
        /// Setter for the automatic day progression variable
        /// </summary>
        /// <param name="autoDay">Automatic day progression boolean</param>
        void setAutoDay(bool autoDay);
        /// <summary>
        /// Getter for the automatic day progression variable
        /// </summary>
        /// <returns></returns>
        bool getAutoDay();

        /// <summary>
        /// Setter for the spot light activation
        /// </summary>
        /// <param name="index"></param>
        /// <param name="value"></param>
        void setSpotLight(int index,int value);

        /// <summary>
        /// Getter for the view matrix
        /// </summary>
        /// <param name="myCamera"></param>
        /// <returns></returns>
        glm::mat4 getView(gps::Camera myCamera);

        /// <summary>
        /// Getter for the projection matrix
        /// </summary>
        /// <param name="myWindow"></param>
        /// <returns></returns>
        glm::mat4 getProjection(gps::Window myWindow);

    private:

        ////////////////////////////////////////////////////////////////
        // Private methods
        ////////////////////////////////////////////////////////////////

        /// <summary>
        /// Initialize the FBO
        /// </summary>
        void initFBO();

        /// <summary>
        /// Initialize the models locations and lists
        /// </summary>
        void initModels();

        /// <summary>
        /// Load the shaders
        /// </summary>
        void initShaders();

        /// <summary>
        /// Initialize the uniform values
        /// </summary>
        /// <param name="myWindow"></param>
        void initUniforms(gps::Window myWindow);

        /// <summary>
        /// Initialize the objects' initial location, rotation and scale
        /// </summary>
        /// <param name="myWindow"></param>
        /// <param name="myCamera"></param>
        void initObjectsModels(gps::Window myWindow, gps::Camera myCamera);

        /// <summary>
        /// Helper method to compute the light space matrix
        /// </summary>
        /// <param name="myWindow"></param>
        /// <param name="myCamera"></param>
        /// <returns></returns>
        glm::mat4 computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera);

        /// <summary>
        /// Helper method to change the dynamic objects
        /// </summary>
        /// <param name="deltaTime"></param>
        void changeDynamicComponents(gps::DeltaTime deltaTime);

        /// <summary>
        /// Helper method to get the sun position on the path described by the internal formula
        /// </summary>
        /// <param name="intensity"></param>
        /// <returns></returns>
        glm::vec3 getSunPositionByIntensity(GLfloat intensity);

        /// <summary>
        /// Method which renders all the objects shadows in the shadow pass phase
        /// </summary>
        void renderShadows();

        /// <summary>
        /// Method which renders the colored objects
        /// </summary>
        void renderModels();

        ////////////////////////////////////////////////////////////////
        // Shaders
        ////////////////////////////////////////////////////////////////
        gps::Shader mainShader;
        gps::Shader depthMapShader;
        gps::Shader waterShader;

        ////////////////////////////////////////////////////////////////
        // Shadow maps
        ////////////////////////////////////////////////////////////////
        GLuint shadowMapFBO;
        GLuint depthMapTexture;

        ////////////////////////////////////////////////////////////////
        // General  view/projection variables
        ////////////////////////////////////////////////////////////////
        glm::mat4 view;
        GLuint viewLoc;
        glm::mat4 projection;
        glm::mat3 normalMatrix;
        glm::mat3 lightDirMatrix;
        glm::mat4 lightSpaceTrMatrix;

        ////////////////////////////////////////////////////////////////
        // Directional variables
        ////////////////////////////////////////////////////////////////
        bool autoDay = true;
        gps::Model3D directionalLightSphere;
        gps::InGameObject directionalLightSphereObject;
        glm::vec3 directionalLightPosition;
        GLuint directionalLightPositionLoc;
        glm::vec3 directionalLightColor;
        GLuint directionalLightColorLoc;
        GLfloat directionalLightIntensity = 1.0f;


        ////////////////////////////////////////////////////////////////
        // Point lights
        ////////////////////////////////////////////////////////////////
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

        ////////////////////////////////////////////////////////////////
        // Spot Lights
        ////////////////////////////////////////////////////////////////
        #define SPOT_LIGHTS_MAX 1
        struct SPOT_LIGHT
        {
            int spotinit;
            glm::vec3 spotLightDirection = glm::vec3(0.0f);;
            glm::vec3 spotLightPosition = glm::vec3(0.0f);
        };
        int spotLightLocationBegin = 120;
        SPOT_LIGHT spotLights[SPOT_LIGHTS_MAX];

        ////////////////////////////////////////////////////////////////
        // FOV
        ////////////////////////////////////////////////////////////////
        #define FOV 45.0f

        /// <summary>
        /// Shadow width resolution
        /// </summary>
        const unsigned int SHADOW_WIDTH = 1024 * 8;
        /// <summary>
        /// Shadow height resolution
        /// </summary>
        const unsigned int SHADOW_HEIGHT = 1024 * 8;

        ////////////////////////////////////////////////////////////////
        // Objects numbers
        ////////////////////////////////////////////////////////////////
        #define WALLS_NUMBER 12
        #define GATES_NUMBER 4
        #define STREETS_NUMBER 40
        #define GRASS_NUMBER 210
        #define BUILDINGS_NUMBER 34
        #define WATER_NUMBER 3
        #define STALL_NUMBER 4

        ////////////////////////////////////////////////////////////////
        // Actual models
        ////////////////////////////////////////////////////////////////
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
        gps::Model3D stall;

        ////////////////////////////////////////////////////////////////
        // InGmaeObject instances
        ////////////////////////////////////////////////////////////////
        std::vector<gps::InGameObject> wallObjects;
        std::vector<gps::InGameObject> gatesObjects;
        std::vector<gps::InGameObject> streetsObjects;
        std::vector<gps::InGameObject> grassObjects;
        std::vector<gps::InGameObject> insulaRomanaObjects;
        std::vector<gps::InGameObject> waterPoolsObject;
        std::vector<gps::InGameObject> streetLampsObjects;
        std::vector<gps::InGameObject> stallsObjects;
        gps::InGameObject waterFountainObject;
        gps::InGameObject forumObject;

        ////////////////////////////////////////////////////////////////
        // Color instance
        ////////////////////////////////////////////////////////////////
        Colors colorParser;

        ///////////////////////////////////////////////////////////////
        // Music global variable
        ///////////////////////////////////////////////////////////////
        ISoundEngine* SoundEngine;
    };
}

#endif // !ObjectManager_hpp
