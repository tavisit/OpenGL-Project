#include "ObjectManager.hpp"
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
namespace gps {
    void ObjectManager::renderScene(gps::Window myWindow, gps::Camera myCamera, DeltaTime deltaTime)
    {
        changeDynamicComponents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        depthMapShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix(myWindow,myCamera)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glCheckError();
        waterFountainObject.drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &waterFountain, &depthMapShader, false);
        for (int index = 0; index < GATES_NUMBER; index++)
        {
            gatesObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &wallGate, &depthMapShader, false);
        }
        for (int index = 0; index < WALLS_NUMBER; index++)
        {
            wallObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &wall, &depthMapShader, false);
        }
        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            grassObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &grass, &depthMapShader, false);
        }
        for (int index = 0; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &street, &depthMapShader, false);
        }
      

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mainShader.useShaderProgram();

        // send lightSpace matrix to shader
        glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix(myWindow, myCamera)));

        // send view matrix to shader
        view = myCamera.getViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // compute light direction transformation matrix
        lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
        // send lightDir matrix data to shader
        glUniformMatrix3fv(glGetUniformLocation(mainShader.shaderProgram, "lightDirMatrix"), 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
        mainShader.useShaderProgram();

        // bind the depth map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(mainShader.shaderProgram, "shadowMap"), 3);
        glCheckError();

        waterFountainObject.drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &waterFountain, &mainShader, true);
        for (int index = 0; index < GATES_NUMBER; index++)
        {
            gatesObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &wallGate, &mainShader, true);
        }
        for (int index = 0; index < WALLS_NUMBER; index++)
        {
            wallObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &wall, &mainShader, true);
        }
        glCheckError();
        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            grassObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &grass, &mainShader, true);
        }
        for (int index = 0; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].drawObject(glGetUniformLocation(mainShader.shaderProgram, "normalMatrix"), view, &street, &mainShader, true);
        }

        pointLightIntensityLoc = glGetUniformLocation(mainShader.shaderProgram, "lightIntensities");
        glUniform1fv(pointLightIntensityLoc, LIGHT_MAX, pointLightsIntensity);
        pointLightLoc = glGetUniformLocation(mainShader.shaderProgram, "lightsPos");
        glUniform3fv(pointLightLoc, LIGHT_MAX, glm::value_ptr(pointLight[0]));
        pointLightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "lightsColors");
        glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
    }
    void ObjectManager::initObjectManager(gps::Window myWindow, gps::Camera myCamera)
    {
        srand(time(NULL));
        initModels();
        initObjectsModels(myWindow, myCamera);
        initFBO();
        initShaders();
        initUniforms(myWindow);
    }
    void ObjectManager::initObjectsModels(gps::Window myWindow, gps::Camera myCamera)
    {
        lightAngle = 20.0f;
        for (int index = 0; index < GATES_NUMBER; index++)
        {
            gatesObjects[index].translateDistance(3, gps::MOVE_DOWN, 1);
            gatesObjects[index].scaleDistance(1.20f);
        }
        gatesObjects[1].translateDistance(160, gps::MOVE_BACKWARD, 1);
        gatesObjects[2].translateDistance( 80, gps::MOVE_BACKWARD, 1);
        gatesObjects[2].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
        gatesObjects[2].translateDistance( 160, gps::MOVE_RIGHT, 1);
        gatesObjects[3].translateDistance( 80, gps::MOVE_BACKWARD, 1);
        gatesObjects[3].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
        gatesObjects[3].translateDistance( 160, gps::MOVE_LEFT, 1);

        waterFountainObject.translateDistance(80, gps::MOVE_BACKWARD, 1);
        waterFountainObject.translateDistance(3, gps::MOVE_DOWN, 1);
        waterFountainObject.scaleDistance(4);

        for (int index = 0; index < WALLS_NUMBER; index++)
        {
            wallObjects[index].translateDistance(3, gps::MOVE_DOWN, 1);
            wallObjects[index].scaleDistance(1.20f);
        }
        for (int index = 0; index < 2; index++)
        {
            wallObjects[index].translateDistance((index + 1) * 79.8, gps::MOVE_RIGHT, 1);
        }
        wallObjects[2].translateDistance(160, gps::MOVE_RIGHT, 1);
        wallObjects[2].rotationAbsolute(glm::vec3(0.0f,90.0f,0.0f));
        wallObjects[3].translateDistance(160, gps::MOVE_RIGHT, 1);
        wallObjects[3].rotationAbsolute(glm::vec3(0.0f, 90.0f, 0.0f));
        wallObjects[3].translateDistance(160, gps::MOVE_BACKWARD, 1);
        for (int index = 4; index < 6; index++)
        {
            wallObjects[index].translateDistance((index -3) * 79.8, gps::MOVE_RIGHT, 1);
            wallObjects[index].translateDistance(160, gps::MOVE_BACKWARD, 1);
        }
        for (int index = 6; index < 8; index++)
        {
            wallObjects[index].translateDistance((index - 5) * 79.8, gps::MOVE_LEFT, 1);
            wallObjects[index].translateDistance(160, gps::MOVE_BACKWARD, 1);
        }
        wallObjects[8].translateDistance(160, gps::MOVE_LEFT, 1);
        wallObjects[8].rotationAbsolute(glm::vec3(0.0f, 90.0f, 0.0f));
        wallObjects[9].translateDistance(160, gps::MOVE_LEFT, 1);
        wallObjects[9].rotationAbsolute(glm::vec3(0.0f, 90.0f, 0.0f));
        wallObjects[9].translateDistance(160, gps::MOVE_BACKWARD, 1);
        for (int index = 10; index < 12; index++)
        {
            wallObjects[index].translateDistance((index -9) * 79.8, gps::MOVE_LEFT, 1);
        }

        for (int index = 0; index < STREETS_NUMBER / 4; index++)
        {
            streetsObjects[index].translateDistance( 3, gps::MOVE_DOWN, 1);
            streetsObjects[index].scaleAbsolute( glm::vec3(2));
            streetsObjects[index].translateDistance( index * 12 * 2, gps::MOVE_FORWARD, 1);
        }
        for (int index = STREETS_NUMBER / 4; index < STREETS_NUMBER / 2; index++)
        {
            streetsObjects[index].translateDistance( 3, MOVE_DOWN, 1);
            streetsObjects[index].scaleAbsolute( glm::vec3(2));
            streetsObjects[index].translateDistance( (index - STREETS_NUMBER / 4 + 1) * 12 * 2, gps::MOVE_BACKWARD, 1);
        }
        for (int index = STREETS_NUMBER / 2; index < 3 * STREETS_NUMBER / 4; index++)
        {
            streetsObjects[index].translateDistance( 3, gps::MOVE_DOWN, 1);
            streetsObjects[index].translateDistance( 80, gps::MOVE_BACKWARD, 1);
            streetsObjects[index].scaleAbsolute( glm::vec3(2));
            streetsObjects[index].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
            streetsObjects[index].translateDistance( (index - STREETS_NUMBER / 2) * 12 * 2, gps::MOVE_RIGHT, 1);
        }
        for (int index = 3 * STREETS_NUMBER / 4; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].translateDistance( 3, gps::MOVE_DOWN, 1);
            streetsObjects[index].translateDistance( 80, gps::MOVE_BACKWARD, 1);
            streetsObjects[index].scaleAbsolute( glm::vec3(2));
            streetsObjects[index].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
            streetsObjects[index].translateDistance( (index - 3 * STREETS_NUMBER / 4 + 1) * 12 * 2, gps::MOVE_LEFT, 1);
        }

        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            
            grassObjects[index].translateDistance( 3.05, gps::MOVE_DOWN, 1);
            grassObjects[index].scaleAbsolute( glm::vec3(8.0f, 8.0f, 8.0f));
        }
        int index = 0;
        for (int indexc = 0; indexc < 10; indexc++)
        {
            for (int indexr = 0; indexr < 10; indexr++)
            {
                grassObjects[index].translateAbsolute( glm::vec3((float)(5 * 12 * 4 - indexc * 12 * 4), grassObjects[index].getLocation().y, (float)(5 * 12 * 4 - indexr * 12 * 4)));
                index++;
            }
        }
    }
    void ObjectManager::resizeWindow(gps::Window myWindow)
    {
        mainShader.useShaderProgram();
        // set projection matrix
        projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
        //send matrix data to shader
        GLint projLoc = glGetUniformLocation(mainShader.shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));       
    }
    glm::mat4 ObjectManager::getModel()
    {
        return model;
    }
    glm::mat4 ObjectManager::getView(gps::Camera myCamera)
    {
        view = myCamera.getViewMatrix();
        return view;
    }
    glm::mat4 ObjectManager::getProjection(gps::Window myWindow)
    {
        projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
        return projection;
    }
    void ObjectManager::initFBO()
    {
        //generate FBO ID
        glGenFramebuffers(1, &shadowMapFBO);

        //create depth texture for FBO
        glGenTextures(1, &depthMapTexture);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //attach texture to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void ObjectManager::initModels()
    {
        wallGate.LoadModel("models/buildings/WallGate.obj");
        wall.LoadModel("models/buildings/Wall.obj");
        grass.LoadModel("models/grass/grass.obj");
        street.LoadModel("models/road/road.obj");
        waterFountain.LoadModel("models/buildings/waterFountain.obj");

        for (int i = 0; i < WALLS_NUMBER; i++)
        {
            InGameObject genericObject;
            wallObjects.push_back(genericObject);
        }
        for (int i = 0; i < GATES_NUMBER; i++)
        {
            gps::InGameObject genericObject;
            gatesObjects.push_back(genericObject);
        }
        for (int i = 0; i < STREETS_NUMBER; i++)
        {
            InGameObject genericObject;
            streetsObjects.push_back(genericObject);
        }
        for (int i = 0; i < GRASS_NUMBER; i++)
        {
            InGameObject genericObject;
            grassObjects.push_back(genericObject);
        }
        /*
        for (int i = 0; i < BUILDINGS_NUMBER; i++)
        {
            InGameObject genericObject;
            buildingsObjects.push_back(genericObject);
            gps::Model3D wallGate;
            wallGate.LoadModel("models/buildings/WallGate.obj");
            wallGates.push_back(wallGate);
        }*/
    }
    void ObjectManager::initShaders()
    {
        mainShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
        depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
    }
    void ObjectManager::initUniforms(gps::Window myWindow)
    {
        mainShader.useShaderProgram();
        modelLoc = glGetUniformLocation(mainShader.shaderProgram, "model");
        viewLoc = glGetUniformLocation(mainShader.shaderProgram, "view");
        projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
        glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for (int i = 0; i < LIGHT_MAX; i++)
        {
            pointLightsIntensity[i] = 1.0f;
        }
        pointLightIntensityLoc = glGetUniformLocation(mainShader.shaderProgram, "lightIntensities");
        glUniform1fv(pointLightIntensityLoc, LIGHT_MAX, pointLightsIntensity);

        pointLight[0] = glm::vec3(0.0f, 2.0f, 40.0f);
        pointLight[1] = glm::vec3(2.0f, 2.0f, -3.0f);
        pointLightLoc = glGetUniformLocation(mainShader.shaderProgram, "lightsPos");
        glUniform3fv(pointLightLoc, LIGHT_MAX, glm::value_ptr(pointLight[0]));

        pointLightColor[0] = colorParser.convertFromEnumToVector(colorParser.WHITE);
        pointLightColor[1] = colorParser.convertFromEnumToVector(colorParser.WHITE);
        pointLightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "lightsColors");
        glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));

        lightDir = glm::vec3(1.0f, 2.5f, 3.5f) * 20.0f;
        lightDirLoc = glGetUniformLocation(mainShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

        // set light color
        lightColor = colorParser.convertFromEnumToVector(colorParser.ORANGE) * glm::vec3(0.4f);
        lightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }

    glm::mat4 ObjectManager::computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera)
    {
        const GLfloat near_plane = 1.0f, far_plane = 200.0f;
        glm::mat4 lightProjection = glm::ortho(
            -100.0f, 
            100.0f,
            -100.0f,
            100.0f,
            near_plane,
            far_plane);

        glm::mat4 lightPosition = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::vec3 lightDirTr = glm::vec3(lightPosition * glm::vec4(lightDir, 1.0f));
        glm::mat4 lightView = glm::lookAt(lightDirTr, glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f));

        return lightProjection * lightView;
    }
    void ObjectManager::changeDynamicComponents()
    {
        float randomVariance = 1/(float)(rand() % 1000)+0.5f;
        pointLightsIntensity[0] = randomVariance;
    }
}