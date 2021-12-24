#include "DeltaTime.h"

namespace gps {
    void DeltaTime::initializeDeltaTime()
    {
        lastFrame = glfwGetTime();
    }

    void DeltaTime::calculateDeltaTime(bool printFps)
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timePassed -= deltaTime;
        if (timePassed < 0.0f)
        {
            timePassed = 1.0f;
            FPS = framesPassed;
            framesPassed = 0;
            if (printFps)
            {
                printf("FPS %d\n", FPS);
            }
        }
        framesPassed++;
    }

    float DeltaTime::getDeltaTime()
    {
        return deltaTime;
    }

    float DeltaTime::getTranslationSpeed()
    {
        return translationConstant * deltaTime;
    }

    float DeltaTime::getRotationSpeed()
    {
        return rotationConstant * deltaTime;
    }

    int DeltaTime::getFps()
    {
        return FPS;
    }

}
