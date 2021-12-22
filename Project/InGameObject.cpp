#include "InGameObject.h"

void gps::InGameObject::initializeObject(gps::Model3D gameObject)
{
    this->gameObject = gameObject;

    location = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    model = glm::mat4(1);
}

void gps::InGameObject::initializeObject(std::string objectPath, std::string objectFile)
{
    this->gameObject.LoadModel(objectPath, objectFile);

    location = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    model = glm::mat4(1);
}

void gps::InGameObject::initializeUniforms(gps::Shader shader, gps::Camera myCamera, gps::Window myWindow)
{
    shader.useShaderProgram();

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

void gps::InGameObject::translateObject(gps::Shader shader, GLfloat distance, MOVE_DIRECTION moveDirection, float speed)
{
    // create model matrix for teapot
    glm::vec3 newDirection;
    switch (moveDirection)
    {
    case MOVE_BACKWARD:
        newDirection = glm::vec3(0, 0, distance);
        break;
    case MOVE_FORWARD:
        newDirection = glm::vec3(0, 0, -distance);
        break;
    case MOVE_RIGHT:
        newDirection = glm::vec3(distance, 0, 0);
        break;
    case MOVE_LEFT:
        newDirection = glm::vec3(-distance, 0, 0);
        break;
    case MOVE_UP:
        newDirection = glm::vec3(0, distance, 0);
        break;
    case MOVE_DOWN:
        newDirection = glm::vec3(0, -distance, 0);
        break;
    default:
        break;
    }
    newDirection *= speed;
    location += newDirection;
    objectTransformed(shader);
}

void gps::InGameObject::translateObject(gps::Shader shader, glm::vec3 absolutePosition)
{
    location = absolutePosition;
    objectTransformed(shader);
}

void gps::InGameObject::rotateObject(gps::Shader shader, glm::vec3 rotationAngles)
{
    rotation = rotationAngles;
    objectTransformed(shader);
}

void gps::InGameObject::rotateObjectWithAngle(gps::Shader shader, glm::vec3 rotationAngles)
{
    rotation += rotationAngles;
    rotateObject(shader, rotation);
}

void gps::InGameObject::scaleObject(gps::Shader shader, glm::vec3 scale)
{
    this->scale = scale;
    objectTransformed(shader);
}

void gps::InGameObject::renderObject(gps::Shader shader)
{
    shader.useShaderProgram();

    //send object model matrix data to shader
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    gameObject.Draw(shader);
}

void gps::InGameObject::renderShadow(gps::Shader shaderShadow)
{
    shaderShadow.useShaderProgram();
    //send object model matrix data to shader
    modelLoc = glGetUniformLocation(shaderShadow.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    gameObject.Draw(shaderShadow);
}


void gps::InGameObject::cameraMoved(gps::Shader shader, gps::Camera myCamera)
{
    //update view matrix
    view = myCamera.getViewMatrix();

    //send object model matrix data to shader
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
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

glm::vec3 gps::InGameObject::getLocation()
{
    return this->location;
}

glm::vec3 gps::InGameObject::getRotation()
{
    return this->rotation;
}

glm::vec3 gps::InGameObject::getScale()
{
    return this->scale;
}

void gps::InGameObject::objectTransformed(gps::Shader shader)
{
    glm::mat4 modelRotate = glm::mat4(1.0f);
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelRotate = glm::rotate(modelRotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 modelTranslate = glm::translate(glm::mat4(1.0f), location);
    glm::mat4 modelScaled = glm::scale(glm::mat4(1.0f), scale);

    model = modelTranslate * modelScaled * modelRotate;

    shader.useShaderProgram();
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(shader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}
