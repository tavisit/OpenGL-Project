#define GLEW_STATIC
#include "wtypes.h"

#include "Constants.h"
#include "Window.h"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "DeltaTime.hpp"
#include "ObjectManager.hpp"
#include "Music.hpp"

#include <iostream>

int horizontalMonitor;
int verticalMonitor;
int horizontalWindowed = 720;
int verticalWindowed = 1280;
// window
gps::Window myWindow;
// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.5f, 5.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

gps::ObjectManager objectManager;
// for skybox
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
gps::DeltaTime deltaTime;
gps::Music programMusic;
void GetDesktopResolution(int& horizontal, int& vertical);

void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(objectManager.getView(myCamera)));

    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(objectManager.getProjection(myWindow)));
}
void initFaces()
{
    faces.push_back("skybox/Variant1/posx.jpg");
    faces.push_back("skybox/Variant1/negx.jpg");
    faces.push_back("skybox/Variant1/posy.jpg");
    faces.push_back("skybox/Variant1/negy.jpg");
    faces.push_back("skybox/Variant1/posz.jpg");
    faces.push_back("skybox/Variant1/negz.jpg");
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {

    if (width < 10 || height < 10)
        return;

	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    WindowDimensions newDimensions;
    newDimensions.width = width;
    newDimensions.height = height;

    myWindow.setWindowDimensions(newDimensions);
    objectManager.resizeWindow(myWindow);
    // set Viewport transform
    glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}
bool mouse = true;

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mouse)
    {
        lastX = xpos;
        lastY = ypos;
        mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f* deltaTime.getRotationSpeed();
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
}
void processMovement() {
    float actualMovementSpeed = cameraSpeed * deltaTime.getTranslationSpeed();

    if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
        actualMovementSpeed *= 16;
    }

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, actualMovementSpeed);
        
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, actualMovementSpeed);
        
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, actualMovementSpeed);
        
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, actualMovementSpeed);
        
	}
    if (pressedKeys[GLFW_KEY_G]) {
        glfwSetWindowMonitor(myWindow.getWindow(), nullptr, 100, 100, verticalWindowed, horizontalWindowed, GLFW_DONT_CARE);
    }
    if (pressedKeys[GLFW_KEY_F]) {
        glfwSetWindowMonitor(myWindow.getWindow(), glfwGetPrimaryMonitor(), 0, 0, verticalMonitor, horizontalMonitor, GLFW_DONT_CARE);
    }

    if (pressedKeys[GLFW_KEY_K])
    {
        myCamera.setWalkingVar(false);
    }
    if (pressedKeys[GLFW_KEY_L])
    {
        myCamera.setWalkingVar(true);
    }

    // line view
    if (pressedKeys[GLFW_KEY_7]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // point view
    if (pressedKeys[GLFW_KEY_8]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    // normal view
    if (pressedKeys[GLFW_KEY_9]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void initOpenGLWindow() {
    myWindow.Create(verticalWindowed, horizontalWindowed, "City");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
}
void renderScene() {
    objectManager.renderScene(myWindow, myCamera, deltaTime);
    mySkyBox.Draw(skyboxShader, objectManager.getView(myCamera), objectManager.getProjection(myWindow));
}

void cleanup() {
    myWindow.Delete();
    programMusic.~Music();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {
    std::cout << "\n\n\n            Credits\n\n";
    std::cout << "\n\n            Models\n\n";
    std::cout << "Low Poly Roman Insula 1 (WIP) (https://skfb.ly/6WEtF) by lexferreira89 is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).\n";
    std::cout << "\n\n            Sound\n\n";
    std::cout << "https://pixabay.com/ \n";
    std::cout << "https://freesound.org/ \n";
    std::cout << "\n\n            Textures\n\n";
    std::cout << "Creation of texture maps: https://cpetry.github.io/NormalMap-Online/ \n";
    std::cout << "\n\n\n";


    GetDesktopResolution(horizontalMonitor, verticalMonitor);

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    objectManager.initObjectManager(myWindow, myCamera);
    setWindowCallbacks();

    initFaces();
    initSkyBoxShader();
    deltaTime.initializeDeltaTime();

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        deltaTime.calculateDeltaTime(true);

        programMusic.update(myCamera);

        processMovement();
        renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());
    }
	cleanup();

    return EXIT_SUCCESS;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}