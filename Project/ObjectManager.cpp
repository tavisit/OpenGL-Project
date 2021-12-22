#include "ObjectManager.hpp"

namespace gps {
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
		for (int index = 0; index < FIELD_LENGTH; index++)
		{
			fieldObjects[index].initializeObject("models/grass/grass.obj", "models/grass/");
		}
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].initializeObject("models/road/road.obj","models/road/");
		}
		for (int index = 0; index < WALL_LENGTH; index++)
		{
			wallsObject[index].initializeObject("models/buildings/Wall.obj","models/buildings/");
		}
		for (int index = 0; index < GATES_LENGTH; index++)
		{
			gatesObject[index].initializeObject("models/buildings/WallGate.obj", "models/buildings/");
		}
		for (int index = 0; index < BUILDINGS_LENGTH; index++)
		{
			buildingsObject[index].initializeObject("models/buildings/WallGate.obj", "models/buildings/");
		}
	}
	void ObjectManager::initShaders()
	{
		shader.loadShader("shaders/basic.vert", "shaders/basic.frag");
		depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
	}
	void ObjectManager::initUniforms(gps::Camera myCamera, gps::Window myWindow)
	{

		shader.useShaderProgram();

		modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

		viewLoc = glGetUniformLocation(shader.shaderProgram, "view");

		normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

		lightDirMatrixLoc = glGetUniformLocation(shader.shaderProgram, "lightDirMatrix");

		projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
		projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// set the light direction (direction towards the light)
		lightDir = glm::vec3(0.0f, 2.5f, 0.5f) * 20.0f;
		lightDirLoc = glGetUniformLocation(shader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

		// set light color
		lightColor = glm::vec3(0.5f, 0.5f, 0.5f); //white light
		lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		for (int index = 0; index < FIELD_LENGTH; index++)
		{
			fieldObjects[index].initializeUniforms(shader, myCamera, myWindow);
		}
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].initializeUniforms(shader,myCamera,myWindow);
		}
		for (int index = 0; index < GATES_LENGTH; index++)
		{
			gatesObject[index].initializeUniforms(shader, myCamera, myWindow);
		}
	}
	void ObjectManager::renderScene(gps::Camera myCamera,gps::Window myWindow)
	{
		depthMapShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix(myCamera)));
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (int index = 0; index < FIELD_LENGTH; index++)
		{
			fieldObjects[index].renderShadow(depthMapShader);
		}
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].renderShadow(depthMapShader);
		}
		for (int index = 0; index < GATES_LENGTH; index++)
		{
			gatesObject[index].renderShadow(depthMapShader);
		}
		glCheckError();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
		glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));
		// bind the depth map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix(myCamera)));

		for (int index = 0; index < FIELD_LENGTH; index++)
		{
			fieldObjects[index].renderObject(shader);
		}
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].renderObject(shader);
		}
		for (int index = 0; index < GATES_LENGTH; index++)
		{
			gatesObject[index].renderObject(shader);
		}
		glCheckError();
	
	}
	void ObjectManager::refreshContent(gps::Camera myCamera)
	{
		for (int index = 0; index < FIELD_LENGTH; index++)
		{
			fieldObjects[index].cameraMoved(shader, myCamera);
		}
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].cameraMoved(shader, myCamera);
		}
		for (int index = 0; index < GATES_LENGTH; index++)
		{
			gatesObject[index].cameraMoved(shader, myCamera);
		}
		for (int index = 0; index < WALL_LENGTH; index++)
		{
			wallsObject[index].cameraMoved(shader, myCamera);
		}
		for (int index = 0; index < BUILDINGS_LENGTH; index++)
		{
			buildingsObject[index].cameraMoved(shader, myCamera);
		}
	}
	
	glm::mat4 ObjectManager::computeLightSpaceTrMatrix(gps::Camera myCamera)
	{
		const GLfloat near_plane = 1.0f, far_plane = 200.0f;
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		glm::mat3 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glm::vec3 lightDirTr = glm::vec3(rotation * glm::vec4(lightDir, 1.0f));
		glm::mat4 lightView = glm::lookAt(lightDirTr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		return lightProjection * lightView;
	}

	
}