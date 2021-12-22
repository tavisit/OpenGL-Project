#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "InGameObject.h"
#include "Model3D.hpp"
#include "DeltaTime.h"
#include "SkyBox.hpp"
#include "ObjectManager.hpp"

#include <iostream>

#define TAU (M_PI * 2.0)
// window
gps::Window myWindow;
float screenWidth;
float screenHeight;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

gps::Model3D lightCube;
GLint lightDirLoc;
GLint lightColorLoc;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 1.0f;
GLfloat objectsSpeed = 10.0f;
GLfloat angle;

GLboolean pressedKeys[1024];

// objects
gps::DeltaTime deltaTime;
gps::InGameObject walls[20];
gps::InGameObject wallGate;
//gps::InGameObject mainPlane; 
#define STREET_LENGTH 20
gps::InGameObject road[STREET_LENGTH];
std::vector<const GLchar*> faces;
gps::ObjectManager objectManager;

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
void drawSkyBox()
{
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}
void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    drawSkyBox();
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    screenWidth = width;
    screenHeight = height;

    glfwGetFramebufferSize(window, &width, &height);

    wallGate.getShader().useShaderProgram();
    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].getShader().useShaderProgram();
    }

    initSkyBoxShader();

    // set Viewport transform
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX, lastY;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.15f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);


    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].cameraMoved(myCamera);
    }
    wallGate.cameraMoved(myCamera);
    drawSkyBox();
}


void processMovement() {

    float currentCameraSpeed = cameraSpeed;
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
        currentCameraSpeed *= 4;
    }
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, currentCameraSpeed * deltaTime.getTranslationSpeed());
        wallGate.cameraMoved(myCamera);
        for (int index = 0; index < STREET_LENGTH; index++)
        {
            road[index].cameraMoved(myCamera);
        }
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, currentCameraSpeed * deltaTime.getTranslationSpeed());
        wallGate.cameraMoved(myCamera);
        for (int index = 0; index < STREET_LENGTH; index++)
        {
            road[index].cameraMoved(myCamera);
        }
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, currentCameraSpeed * deltaTime.getTranslationSpeed());
        wallGate.cameraMoved(myCamera);
        for (int index = 0; index < STREET_LENGTH; index++)
        {
            road[index].cameraMoved(myCamera);
        }
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, currentCameraSpeed * deltaTime.getTranslationSpeed());
        wallGate.cameraMoved(myCamera);
        for (int index = 0; index < STREET_LENGTH; index++)
        {
            road[index].cameraMoved(myCamera);
        }
        drawSkyBox();
    }

    //vertices & edges
    if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_EQUAL)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_MINUS)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
    screenWidth = 1024;
    screenHeight = 768;
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initSkybox()
{
    faces.push_back("textures/skybox/posx.jpg");
    faces.push_back("textures/skybox/negx.jpg");
    faces.push_back("textures/skybox/posy.jpg");
    faces.push_back("textures/skybox/negy.jpg");
    faces.push_back("textures/skybox/posz.jpg");
    faces.push_back("textures/skybox/negz.jpg");
}
void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_BLEND); // GL enable color blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void initModels() {
    initSkybox();
    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].initializeObject("models/road/road.obj");
    }
    wallGate.initializeObject("models/buildings/WallGate.obj");
}

void initObjects() {
    wallGate.initializeShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    wallGate.initializeUniforms(myCamera, myWindow);

    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].initializeShader(
            "shaders/basic.vert",
            "shaders/basic.frag");
        road[index].initializeUniforms(myCamera, myWindow);
    }
}

void initUniforms() {    
    wallGate.getShader().useShaderProgram();
    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(wallGate.getShader().shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(wallGate.getShader().shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].getShader().useShaderProgram();
        //set the light direction (direction towards the light)
        lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
        lightDirLoc = glGetUniformLocation(road[index].getShader().shaderProgram, "lightDir");
        // send light dir to shader
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

        //set light color
        lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
        lightColorLoc = glGetUniformLocation(road[index].getShader().shaderProgram, "lightColor");
        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // select active shader program
    wallGate.renderObject();
    for (int index = 0; index < STREET_LENGTH; index++)
    {
        road[index].renderObject();
    }

    mySkyBox.Draw(skyboxShader, view, projection);

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initObjects();
    initUniforms();
    setWindowCallbacks();

    initSkyBoxShader();

    for (int index = 0; index < STREET_LENGTH/2; index++)
    {
        road[index].translateObject(1, road[index].MOVE_DOWN, 1);
        road[index].translateObject(index*12, road[index].MOVE_BACKWARD, 1);
    }
    for (int index = STREET_LENGTH / 2; index < STREET_LENGTH; index++)
    {
        road[index].translateObject(1, road[index].MOVE_DOWN, 1);
        road[index].translateObject((index-STREET_LENGTH/2+1) * 12, road[index].MOVE_FORWARD, 1);
    }
    glCheckError();

    deltaTime.initializeDeltaTime();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        deltaTime.calculateDeltaTime();

        processMovement();
        renderScene();
        glCheckError();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
