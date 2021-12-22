#include "DeltaTime.h"

void gps::DeltaTime::initializeDeltaTime()
{
    lastFrame = glfwGetTime();
}

void gps::DeltaTime::calculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

float gps::DeltaTime::getDeltaTime()
{
    return deltaTime;
}

float gps::DeltaTime::getTranslationSpeed()
{
    return translationConstant*deltaTime;
}

float gps::DeltaTime::getRotationSpeed()
{
    return rotationConstant * deltaTime;
}
