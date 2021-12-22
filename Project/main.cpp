#define GLEW_STATIC

#include "Constants.hpp"
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "InGameObject.h"
#include "Model3D.hpp"
#include "DeltaTime.h"
#include "SkyBox.hpp"
#include "ObjectManager.hpp"

// window
gps::Window myWindow;
float screenWidth;
float screenHeight;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
GLint lightColorLoc;

std::vector<const GLchar*> faces;
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

gps::ObjectManager objectManager;

void drawSkyBox()
{
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    GLint viewLoc = glGetUniformLocation(skyboxShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    GLint projectionLoc = glGetUniformLocation(skyboxShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    mySkyBox.Draw(skyboxShader, view, projection);
}
void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    screenWidth = width;
    screenHeight = height;

    glfwGetFramebufferSize(window, &width, &height);

    
    objectManager.refreshContent(myCamera);
    drawSkyBox();

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


    objectManager.refreshContent(myCamera);
    drawSkyBox();
}


void processMovement() {

    float currentCameraSpeed = cameraSpeed;
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
        currentCameraSpeed *= 4;

    }
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(MOVE_FORWARD, currentCameraSpeed * deltaTime.getTranslationSpeed());
        objectManager.refreshContent(myCamera);
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(MOVE_BACKWARD, currentCameraSpeed * deltaTime.getTranslationSpeed());
        objectManager.refreshContent(myCamera);
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(MOVE_LEFT, currentCameraSpeed * deltaTime.getTranslationSpeed());
        objectManager.refreshContent(myCamera);
        drawSkyBox();
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(MOVE_RIGHT, currentCameraSpeed * deltaTime.getTranslationSpeed());
        objectManager.refreshContent(myCamera);
        drawSkyBox();
    }

    //vertices & edges
    if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_EQUAL)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_MINUS)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_0)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_DEPTH);
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

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

void initialPositionOfObjects()
{
    for (int index = 0; index < GATES_LENGTH; index++)
    {
        objectManager.gatesObject[index].translateObject(objectManager.shader, 3, MOVE_DOWN, 1);
    }
    objectManager.gatesObject[1].translateObject(objectManager.shader, 160, MOVE_BACKWARD, 1);
    objectManager.gatesObject[2].translateObject(objectManager.shader, 80, MOVE_BACKWARD, 1);
    objectManager.gatesObject[2].rotateObject(objectManager.shader, glm::vec3(0.0f, 90.0f, 0.0f));
    objectManager.gatesObject[2].translateObject(objectManager.shader, 160, MOVE_RIGHT, 1);
    objectManager.gatesObject[3].translateObject(objectManager.shader, 80, MOVE_BACKWARD, 1);
    objectManager.gatesObject[3].rotateObject(objectManager.shader, glm::vec3(0.0f, -90.0f, 0.0f));
    objectManager.gatesObject[3].translateObject(objectManager.shader, 160, MOVE_LEFT, 1);

    for (int index = 0; index < STREET_LENGTH / 4; index++)
    {
        objectManager.streetsObject[index].translateObject(objectManager.shader, 3, MOVE_DOWN, 1);
        objectManager.streetsObject[index].scaleObject(objectManager.shader, glm::vec3(2));
        objectManager.streetsObject[index].translateObject(objectManager.shader, index * 12 * 2, MOVE_FORWARD, 1);
    }
    for (int index = STREET_LENGTH / 4; index < STREET_LENGTH/2; index++)
    {
        objectManager.streetsObject[index].translateObject(objectManager.shader, 3, MOVE_DOWN, 1);
        objectManager.streetsObject[index].scaleObject(objectManager.shader, glm::vec3(2));
        objectManager.streetsObject[index].translateObject(objectManager.shader, (index - STREET_LENGTH / 4 + 1) * 12 * 2, MOVE_BACKWARD, 1);
    }
    for (int index = STREET_LENGTH / 2; index < 3*STREET_LENGTH / 4; index++)
    {
        objectManager.streetsObject[index].translateObject(objectManager.shader, 3, MOVE_DOWN, 1);
        objectManager.streetsObject[index].translateObject(objectManager.shader, 80, MOVE_BACKWARD, 1);
        objectManager.streetsObject[index].scaleObject(objectManager.shader, glm::vec3(2));
        objectManager.streetsObject[index].rotateObject(objectManager.shader, glm::vec3(0.0f,90.0f,0.0f));
        objectManager.streetsObject[index].translateObject(objectManager.shader, (index - STREET_LENGTH / 2)* 12 * 2, MOVE_RIGHT, 1);
    }
    for (int index = 3 * STREET_LENGTH / 4; index < STREET_LENGTH; index++)
    {
        objectManager.streetsObject[index].translateObject(objectManager.shader, 3, MOVE_DOWN, 1);
        objectManager.streetsObject[index].translateObject(objectManager.shader, 80, MOVE_BACKWARD, 1);
        objectManager.streetsObject[index].scaleObject(objectManager.shader, glm::vec3(2));
        objectManager.streetsObject[index].rotateObject(objectManager.shader, glm::vec3(0.0f, 90.0f, 0.0f));
        objectManager.streetsObject[index].translateObject(objectManager.shader, (index - 3 * STREET_LENGTH / 4 + 1) * 12 * 2, MOVE_LEFT, 1);
    }


    for (int index = 0; index < FIELD_LENGTH; index++)
    {
        objectManager.fieldObjects[index].translateObject(objectManager.shader, 3.05, MOVE_DOWN, 1);
        objectManager.fieldObjects[index].scaleObject(objectManager.shader, glm::vec3(8.0f, 8.0f, 8.0f));
    }
    int index = 0;
    for (int indexc = 0; indexc < 10; indexc++)
    {
        for (int indexr = 0; indexr < 10; indexr++)
        {
            objectManager.fieldObjects[index].translateObject(objectManager.shader, glm::vec3((float)(5 * 12 * 4 - indexc * 12 * 4), objectManager.fieldObjects[index].getLocation().y, (float)(5 * 12 * 4 - indexr * 12 * 4)));
            index++;
        }
    }
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

    objectManager.initFBO();
    objectManager.initModels();
    objectManager.initShaders();
    objectManager.initUniforms(myCamera,myWindow);
    initialPositionOfObjects();

    initSkybox();
    initSkyBoxShader();

    setWindowCallbacks();
    int frames = 0;
    float toASecond = 1.0f;
    
    deltaTime.initializeDeltaTime();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        deltaTime.calculateDeltaTime();

        toASecond -= deltaTime.getDeltaTime();
        if (toASecond < 0.0f)
        {
            printf("FPS: %d\n", frames);
            toASecond = 1.0f;
            frames = 0;
        }
        else
        {
            frames++;
        }

        processMovement();
        
        objectManager.renderScene(myCamera, myWindow);
        drawSkyBox();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
        
    }

    cleanup();

    return EXIT_SUCCESS;
}
