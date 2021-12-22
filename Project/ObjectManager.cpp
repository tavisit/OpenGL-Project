#include "ObjectManager.hpp"

namespace gps {
	ObjectManager::ObjectManager()
	{
		this->myCamera = gps::Camera(
			glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 0.0f, -10.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
	}
	ObjectManager::ObjectManager(gps::Camera myCamera)
	{
		this->myCamera = myCamera;
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
    void ObjectManager::initializeShaders()
    {
    }
    void ObjectManager::renderObjects()
    {
    }
	void ObjectManager::initModels()
	{
		lights[0].initializeObject("models/cube/cube.obj");

		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].initializeObject("models/road/road.obj");
		}
		wallsObject[0].initializeObject("models/buildings/WallGate.obj");
	}
	void ObjectManager::initShaders()
	{

		shader.loadShader("shaders/basic.vert", "shaders/basic.frag");
		lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
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
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		// pointlight
		lightPos1 = glm::vec3(4.8f, 1.77f, -50.75f); // la sperietoare
		lightPos1Loc = glGetUniformLocation(shader.shaderProgram, "lightPos1");
		glUniform3fv(lightPos1Loc, 1, glm::value_ptr(lightPos1));

		lightPos2 = glm::vec3(-30.3f, 3.77f, -42.0f); // la butoaie
		lightPos2Loc = glGetUniformLocation(shader.shaderProgram, "lightPos2");
		glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));


		// spotlight
		spotlight1 = glm::cos(glm::radians(40.5f));
		spotlight2 = glm::cos(glm::radians(100.5f));

		spotLightDirection = glm::vec3(0, -1, 0);
		spotLightPosition = glm::vec3(32.0f, 1.0f, -33.0f);

		glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight1"), spotlight1);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "spotlight2"), spotlight2);

		glUniform3fv(glGetUniformLocation(shader.shaderProgram, "spotLightDirection"), 1, glm::value_ptr(spotLightDirection));
		glUniform3fv(glGetUniformLocation(shader.shaderProgram, "spotLightPosition"), 1, glm::value_ptr(spotLightPosition));


		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}
	void ObjectManager::renderScene(gps::Window myWindow)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1st step: render the scene to the depth buffer 

		depthMapShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);


		// draw the raccoon
		depthMapShader.useShaderProgram();
		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].renderShadow(depthMapShader);
		}
		wallsObject[0].renderShadow(depthMapShader);

		/////////////////////////////////////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2nd step: render the scene

		shader.useShaderProgram();

		// send lightSpace matrix to shader
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

		// send view matrix to shader
		view = myCamera.getViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// compute light direction transformation matrix
		lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
		// send lightDir matrix data to shader
		glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

		glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
		shader.useShaderProgram();

		// bind the depth map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "shadowMap"), 3);


		for (int index = 0; index < STREET_LENGTH; index++)
		{
			streetsObject[index].renderObject();
		}
		wallsObject[0].renderObject();
	

		// draw a white cube around the light
		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, lightDir);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f) * 6.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lights[0].renderObject();
	}
	void ObjectManager::setCamera(gps::Camera myCamera)
	{
		this->myCamera = myCamera;
	}
	
	glm::mat4 ObjectManager::computeLightSpaceTrMatrix()
	{
		const GLfloat near_plane = 1.0f, far_plane = 200.0f;
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

		return lightProjection * lightView;
	}

	
}