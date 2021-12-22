#include "InGameObject.h"

void gps::InGameObject::initializeShader(const char* shaderVertPath, const char* shaderFragPath)
{
    shader.loadShader(
        shaderVertPath,
        shaderFragPath);
}

void gps::InGameObject::initializeObject(gps::Model3D gameObject)
{
    this->gameObject = gameObject;
}

void gps::InGameObject::initializeObject(const char* objectPath)
{
    this->gameObject.LoadModel(objectPath);
}

void gps::InGameObject::initializeUniforms(gps::Camera myCamera, gps::Window myWindow)
{
    shader.useShaderProgram();

    location = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    model = glm::mat4(1);

    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, farPlane);
    projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void gps::InGameObject::translateObject(GLfloat distance, MOVE_DIRECTION moveDirection, float speed)
{
    // create model matrix for teapot
    glm::vec3 newDirection;
    switch (moveDirection)
    {
        case gps::InGameObject::MOVE_FORWARD:
            newDirection = glm::vec3(0, 0, distance);
            break;
        case gps::InGameObject::MOVE_BACKWARD:
            newDirection = glm::vec3(0, 0, -distance);
            break;
        case gps::InGameObject::MOVE_RIGHT:
            newDirection = glm::vec3(distance, 0, 0);
            break;
        case gps::InGameObject::MOVE_LEFT:
            newDirection = glm::vec3(-distance, 0, 0);
            break;
        case gps::InGameObject::MOVE_UP:
            newDirection = glm::vec3(0, distance, 0);
            break;
        case gps::InGameObject::MOVE_DOWN:
            newDirection = glm::vec3(0, -distance, 0);
            break;
        default:
            break;
    }
    newDirection *= speed;
    location += newDirection;
    objectTransformed();
}

void gps::InGameObject::rotateObject(glm::vec3 rotationAngles)
{
    rotation = rotationAngles;
    objectTransformed();
}

void gps::InGameObject::rotateObjectWithAngle(glm::vec3 rotationAngles)
{
    rotation += rotationAngles;
    rotateObject(rotation);
}

void gps::InGameObject::scaleObject(glm::vec3 scale)
{
    model = glm::scale(scale);
}

void gps::InGameObject::renderObject()
{
    shader.useShaderProgram();
    //send object model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    gameObject.Draw(shader);
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void gps::InGameObject::renderShadow(gps::Shader shaderShadow)
{
    //send object model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    gameObject.Draw(shaderShadow);

}


void gps::InGameObject::cameraMoved(gps::Camera myCamera)
{
    //update view matrix
    view = myCamera.getViewMatrix();
    shader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

gps::Shader gps::InGameObject::getShader()
{
    return shader;
}

void gps::InGameObject::setShader(gps::Shader)
{
    this->shader = shader;
}

glm::mat4 gps::InGameObject::getModel()
{
    return model;
}

glm::mat4 gps::InGameObject::getView()
{
    return view;
}

glm::mat4 gps::InGameObject::getProjection()
{
    return projection;
}

void gps::InGameObject::objectTransformed()
{

    glm::mat4 modelRotate = glm::mat4(1.0f);
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 modelTranslate = glm::translate(glm::mat4(1.0f), location);

    model = modelTranslate * modelRotate;
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    // update normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    shader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}
