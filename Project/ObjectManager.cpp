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
    void ObjectManager::renderScene(gps::Window myWindow, gps::Camera myCamera, gps::DeltaTime deltaTime)
    {
        changeDynamicComponents(deltaTime); 
           
        lightSpaceTrMatrix = computeLightSpaceTrMatrix(myWindow,myCamera);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        depthMapShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(lightSpaceTrMatrix));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        waterFountainObject.drawShadow(&waterFountain, &depthMapShader);
        
        forumObject.drawShadow(&forum, &depthMapShader);
        waterPoolsObject[0].drawShadow(&waterFountainWater, &depthMapShader);
        for (int index = 1; index < WATER_NUMBER; index++)
        {
            waterPoolsObject[index].drawShadow(&waterPool, &depthMapShader);
        }
        for (int index = 0; index < BUILDINGS_NUMBER; index++)
        {
            insulaRomanaObjects[index].drawShadow( &insulaRomana, &depthMapShader);
        }
        for (int index = 0; index < GATES_NUMBER; index++)
        {
            gatesObjects[index].drawShadow(  &wallGate, &depthMapShader);
        }
        for (int index = 0; index < WALLS_NUMBER; index++)
        {
            wallObjects[index].drawShadow(  &wall, &depthMapShader);
        }
        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            grassObjects[index].drawShadow(  &grass, &depthMapShader);
        }
        for (int index = 0; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].drawShadow(  &street, &depthMapShader);
        }
        for (int index = 0; index < LIGHT_MAX; index++)
        {
            streetLampsObjects[index].drawShadow(  &streetLamps, &depthMapShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        mainShader.useShaderProgram();

        // send lightSpace matrix to shader
        glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));

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



        waterFountainObject.drawObject( view, &waterFountain, &mainShader);
        forumObject.drawObject( view, &forum, &mainShader);
        for (int index = 0; index < BUILDINGS_NUMBER; index++)
        {
            insulaRomanaObjects[index].drawObject(view, &insulaRomana, &mainShader);
        }
        for (int index = 0; index < GATES_NUMBER; index++)
        {
            gatesObjects[index].drawObject( view, &wallGate, &mainShader);
        }
        for (int index = 0; index < WALLS_NUMBER; index++)
        {
            wallObjects[index].drawObject( view, &wall, &mainShader);
        }
        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            grassObjects[index].drawObject( view, &grass, &mainShader);
        }
        for (int index = 0; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].drawObject(view, &street, &mainShader);
        }
        for (int index = 0; index < LIGHT_MAX; index++)
        {
            streetLampsObjects[index].drawObject( view, &streetLamps, &mainShader);
        }
        directionalLightSphereObject.translateAbsolute(directionalLightPosition);
        directionalLightSphereObject.drawObject( view, &directionalLightSphere, &mainShader);

        for (int i = 0; i < LIGHT_MAX; i++)
        {
            glUniform3fv(i * 3, 1, glm::value_ptr(pointLights[i].location));
            glUniform3fv(i * 3 + 1, 1, glm::value_ptr(pointLights[i].color));
            glUniform1f(i * 3 + 2, pointLights[i].intensity);
        }
        glUniform3fv(glGetUniformLocation(mainShader.shaderProgram, "directionalLightPosition"), 1, glm::value_ptr(directionalLightPosition));
        directionalLightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "directionalLightColor");
        glUniform3fv(directionalLightColorLoc, 1, glm::value_ptr(directionalLightColor));

        spotLights[0].spotLightDirection = myCamera.getCameraFrontDirection();
        spotLights[0].spotLightPosition = myCamera.getCameraPosition();

        for (int i = 0; i < SPOT_LIGHTS_MAX; i++)
        {
            glUniform1i(i * 3+ spotLightLocationBegin, spotLights[i].spotinit);
            glUniform3fv(i * 3 + 1 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightDirection));
            glUniform3fv(i * 3 + 2 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightPosition));
        }

        waterShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix));
        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
        glUniformMatrix3fv(glGetUniformLocation(waterShader.shaderProgram, "lightDirMatrix"), 1, GL_FALSE, glm::value_ptr(lightDirMatrix));
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
        waterShader.useShaderProgram();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(waterShader.shaderProgram, "shadowMap"), 3);
        glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "time"), sin(glfwGetTime()));
        waterPoolsObject[0].drawObject(view, &waterFountainWater, &waterShader);
        for (int index = 1; index < WATER_NUMBER; index++)
        {
            waterPoolsObject[index].drawObject(view, &waterPool, &waterShader);
        }
        for (int i = 0; i < SPOT_LIGHTS_MAX; i++)
        {
            glUniform1i(i * 3 + spotLightLocationBegin, spotLights[i].spotinit);
            glUniform3fv(i * 3 + 1 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightDirection));
            glUniform3fv(i * 3 + 2 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightPosition));
        }
        for (int i = 0; i < LIGHT_MAX; i++)
        {
            glUniform3fv(i * 3, 1, glm::value_ptr(pointLights[i].location));
            glUniform3fv(i * 3 + 1, 1, glm::value_ptr(pointLights[i].color));
            glUniform1f(i * 3 + 2, pointLights[i].intensity);
        }

        glUniform3fv(glGetUniformLocation(mainShader.shaderProgram, "directionalLightPosition"), 1, glm::value_ptr(directionalLightPosition));
        directionalLightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "directionalLightColor");
        glUniform3fv(directionalLightColorLoc, 1, glm::value_ptr(directionalLightColor));
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

        waterPoolsObject[0].translateDistance(80, gps::MOVE_BACKWARD, 1);
        waterPoolsObject[0].translateDistance(1.5, gps::MOVE_DOWN, 1);
        waterPoolsObject[0].scaleDistance(3.6f);

        waterPoolsObject[1].translateAbsolute(glm::vec3(85.0f,-2.8f,25.0f));
        waterPoolsObject[1].scaleDistance(1.5f);
        waterPoolsObject[2].translateAbsolute(glm::vec3(85.0f, -2.8f,45.0f));
        waterPoolsObject[2].scaleDistance(1.5f);

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
            streetsObjects[index].scaleDistance( 2);
            streetsObjects[index].translateDistance( index * 12 * 2, gps::MOVE_FORWARD, 1);
        }
        for (int index = STREETS_NUMBER / 4; index < STREETS_NUMBER / 2; index++)
        {
            streetsObjects[index].translateDistance( 3, MOVE_DOWN, 1);
            streetsObjects[index].scaleDistance(2);
            streetsObjects[index].translateDistance( (index - STREETS_NUMBER / 4 + 1) * 12 * 2, gps::MOVE_BACKWARD, 1);
        }
        for (int index = STREETS_NUMBER / 2; index < 3 * STREETS_NUMBER / 4; index++)
        {
            streetsObjects[index].translateDistance( 3, gps::MOVE_DOWN, 1);
            streetsObjects[index].translateDistance( 80, gps::MOVE_BACKWARD, 1);
            streetsObjects[index].scaleDistance(2);
            streetsObjects[index].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
            streetsObjects[index].translateDistance( (index - STREETS_NUMBER / 2) * 12 * 2, gps::MOVE_RIGHT, 1);
        }
        for (int index = 3 * STREETS_NUMBER / 4; index < STREETS_NUMBER; index++)
        {
            streetsObjects[index].translateDistance( 3, gps::MOVE_DOWN, 1);
            streetsObjects[index].translateDistance( 80, gps::MOVE_BACKWARD, 1);
            streetsObjects[index].scaleDistance(2);
            streetsObjects[index].rotationAbsolute( glm::vec3(0.0f, 90.0f, 0.0f));
            streetsObjects[index].translateDistance( (index - 3 * STREETS_NUMBER / 4 + 1) * 12 * 2, gps::MOVE_LEFT, 1);
        }

        for (int index = 0; index < GRASS_NUMBER; index++)
        {
            
            grassObjects[index].translateDistance( 3.05, gps::MOVE_DOWN, 1);
            grassObjects[index].scaleDistance(8);
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

        int hideElevation = 40;
        for (int index = 0; index < BUILDINGS_NUMBER; index++)
        {
            insulaRomanaObjects[index].scaleDistance(30);
            insulaRomanaObjects[index].translateDistance(hideElevation, gps::MOVE_DOWN, 1);
            // it takes 26(long face) * height * 30(short face)
        }
        int insulaIndex = 0;
        while(insulaIndex < 16)
        {
            if (insulaIndex != 4)
            {
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex)*10, gps::MOVE_RIGHT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(15, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex-1) * 10, gps::MOVE_RIGHT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(15 + 26, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            }
            else
            {
                insulaIndex += 2;
            }
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex - 2) * 10, gps::MOVE_RIGHT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(120, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex - 3) * 10, gps::MOVE_RIGHT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(120 + 26, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            
        }
        while(insulaIndex < 16*2)
        {
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex- 16) * 10, gps::MOVE_LEFT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(15, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex - 1- 16) * 10, gps::MOVE_LEFT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(15 + 26, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex - 2- 16) * 10, gps::MOVE_LEFT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(120, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
            insulaRomanaObjects[insulaIndex].translateDistance(12 + hideElevation, gps::MOVE_UP, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(25 + (insulaIndex - 3- 16) * 10, gps::MOVE_LEFT, 1);
            insulaRomanaObjects[insulaIndex].translateDistance(120 + 26, gps::MOVE_BACKWARD, 1);
            insulaIndex++;
        }

        forumObject.scaleDistance(4);
        forumObject.translateDistance(2.5, gps::MOVE_DOWN, 1);
        forumObject.translateAbsolute(glm::vec3(65.0f,forumObject.getLocation().y,35.0f));


        for (int index = 0; index < LIGHT_MAX; index++)
        {
            pointLights[index].color = colorParser.convertFromEnumToVector(colorParser.ORANGE);
            pointLights[index].intensity = 0.7f;
        }
        for (int index = 0; index < LIGHT_MAX/2; index++)
        {
            streetLampsObjects[index].translateAbsolute(glm::vec3((index+1)*50, -10.0f, 60.0f));
            pointLights[index].location = glm::vec3(glm::vec3((index + 1) * 40, 2.0f, 55.0f));
        }
        for (int index = LIGHT_MAX / 2; index < LIGHT_MAX; index++)
        {
            streetLampsObjects[index].translateAbsolute(glm::vec3((index + 1 - LIGHT_MAX / 2) * (- 50), -10.0f, 60.0f));
            pointLights[index].location = glm::vec3((index + 1 - LIGHT_MAX / 2) * (-40), 2.0f, 55.0f);
        }
    }
    void ObjectManager::resizeWindow(gps::Window myWindow)
    {
        projection = glm::perspective(glm::radians(FOV), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);

        mainShader.useShaderProgram();
        GLint projLoc = glGetUniformLocation(mainShader.shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));      

        waterShader.useShaderProgram();
        projLoc = glGetUniformLocation(waterShader.shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }
    glm::mat4 ObjectManager::getView(gps::Camera myCamera)
    {
        view = myCamera.getViewMatrix();
        return view;
    }
    glm::mat4 ObjectManager::getProjection(gps::Window myWindow)
    {
        projection = glm::perspective(glm::radians(FOV), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
        return projection;
    }
    void ObjectManager::setDirectionalLightIntensity(GLfloat intensity)
    {
        directionalLightPosition = getSunPositionByIntensity(intensity);
        if (intensity >= 1.75)
        {
            directionalLightColor = colorParser.convertFromHEXToVector("5c2d04");
            for (int i = 0; i < LIGHT_MAX; i++)
            {
                pointLights[i].intensity = 1.0f;
            }
        }
        else if (intensity >= 1.5)
        {
            directionalLightColor = colorParser.convertFromHEXToVector("ffe4c9");
            for (int i = 0; i < LIGHT_MAX; i++)
            {
                pointLights[i].intensity = 0.4f;
            }
        }
        else if (intensity >= 0.5f)
        {
            directionalLightColor = colorParser.convertFromEnumToVector(colorParser.WHITE);
            for (int i = 0; i < LIGHT_MAX; i++)
            {
                pointLights[i].intensity = 0.0f;
            }
        }
        else if (intensity >= 0.25f)
        {
            directionalLightColor = colorParser.convertFromHEXToVector("ffe4c9");
            for (int i = 0; i < LIGHT_MAX; i++)
            {
                pointLights[i].intensity = 0.4f;
            }
        }
        else
        {
            directionalLightColor = colorParser.convertFromHEXToVector("5c2d04");
            for (int i = 0; i < LIGHT_MAX; i++)
            {
                pointLights[i].intensity = 1.0f;
            }
        }
        directionalLightIntensity = intensity;
    }
    GLfloat ObjectManager::getDirectionalLightIntensity()
    {
        return this->directionalLightIntensity;
    }

    void ObjectManager::setAutoDay(bool autoDay)
    {
        this->autoDay = autoDay;
    }

    bool ObjectManager::getAutoDay()
    {
        return autoDay;
    }

    void ObjectManager::setSpotLight(int index,int value)
    {
        if (index < SPOT_LIGHTS_MAX)
        {
            this->spotLights[index].spotinit = value;
        }
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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
        waterFountainWater.LoadModel("models/water/water.obj");
        waterPool.LoadModel("models/water/pond.obj");
        insulaRomana.LoadModel("models/buildings/Insulae/Insula.obj");
        forum.LoadModel("models/road/forum.obj");
        streetLamps.LoadModel("models/street_lamp/street_lamp.obj");
        directionalLightSphere.LoadModel("models/cube/sun.obj");

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
        for (int i = 0; i < BUILDINGS_NUMBER; i++)
        {
            InGameObject genericObject;
            insulaRomanaObjects.push_back(genericObject);
        }
        for (int i = 0; i < LIGHT_MAX; i++)
        {
            InGameObject genericObject;
            streetLampsObjects.push_back(genericObject);
        }
        for (int i = 0; i < WATER_NUMBER; i++)
        {
            InGameObject genericObject;
            genericObject.setAlpha(0.9f);
            waterPoolsObject.push_back(genericObject);
        }
    }
    void ObjectManager::initShaders()
    {
        mainShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
        depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
        waterShader.loadShader("shaders/waterShader.vert", "shaders/waterShader.frag");
    }
    void ObjectManager::initUniforms(gps::Window myWindow)
    {
        projection = glm::perspective(glm::radians(FOV), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
        // set light color
        setDirectionalLightIntensity(0.4f);
        mainShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        for (int i = 0; i < LIGHT_MAX; i++)
        {
            glUniform3fv(i * 3, 1, glm::value_ptr(pointLights[i].location));
            glUniform3fv(i * 3 + 1, 1, glm::value_ptr(pointLights[i].color));
            glUniform1f(i * 3 + 2, pointLights[i].intensity);
        }
        glUniform3fv(glGetUniformLocation(mainShader.shaderProgram, "directionalLightPosition"), 1, glm::value_ptr(directionalLightPosition));
        directionalLightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "directionalLightColor");
        glUniform3fv(directionalLightColorLoc, 1, glm::value_ptr(directionalLightColor));


        for (int i = 0; i < SPOT_LIGHTS_MAX; i++)
        {
            glUniform1i(i * 3 + spotLightLocationBegin, spotLights[i].spotinit);
            glUniform3fv(i * 3 + 1 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightDirection));
            glUniform3fv(i * 3 + 2 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightPosition));
        }

        waterShader.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


         for (int i = 0; i < SPOT_LIGHTS_MAX; i++)
        {
            glUniform1i(i * 3 + spotLightLocationBegin, spotLights[i].spotinit);
            glUniform3fv(i * 3 + 1 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightDirection));
            glUniform3fv(i * 3 + 2 + spotLightLocationBegin, 1, glm::value_ptr(spotLights[i].spotLightPosition));
        }

        for (int i = 0; i < LIGHT_MAX; i++)
        {
            glUniform3fv(i * 3, 1, glm::value_ptr(pointLights[i].location));
            glUniform3fv(i * 3 + 1, 1, glm::value_ptr(pointLights[i].color));
            glUniform1f(i * 3 + 2, pointLights[i].intensity);
        }
        glUniform3fv(glGetUniformLocation(waterShader.shaderProgram, "directionalLightPosition"), 1, glm::value_ptr(directionalLightPosition));
        directionalLightColorLoc = glGetUniformLocation(waterShader.shaderProgram, "directionalLightColor");
        glUniform3fv(directionalLightColorLoc, 1, glm::value_ptr(directionalLightColor));

        
    }

    glm::mat4 ObjectManager::computeLightSpaceTrMatrix(gps::Window myWindow, gps::Camera myCamera)
    {
        const GLfloat near_plane = 0.1f, far_plane = 300.0f;
        glm::mat4 lightProjection = glm::ortho(
            -500.0f,
            500.0f,
            -500.0f,
            500.0f,
            near_plane,
            far_plane);

        glm::mat4 lightView = glm::lookAt(
            directionalLightPosition,
            glm::vec3(0, -2.0f, 80.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        return lightProjection * lightView;
    }

    void ObjectManager::changeDynamicComponents(gps::DeltaTime deltaTime)
    {
        if (autoDay)
        {
            if (directionalLightIntensity <= 0.0f)
            {
                directionalLightIntensity = 2.0f;
            }
            setDirectionalLightIntensity(directionalLightIntensity - 0.01f * deltaTime.getDeltaTime());
        }
    }

    glm::vec3 ObjectManager::getSunPositionByIntensity(GLfloat intensity)
    {
        glm::vec3 newPosition;
        if (intensity > 1.0f)
        {
            newPosition = glm::vec3(-0.5f * 100 * (intensity-1) + 10, -1 * 100 * (intensity-1) + 100, -3 * 100 * (intensity-1) + 10);
        }
        else
        {
            intensity = 1 - intensity;
            newPosition = glm::vec3(0.5f * 100 * intensity + 10, -1 * 100 * intensity + 100, 3 * 100 * intensity + 10);
        }
        return newPosition;
    }
}