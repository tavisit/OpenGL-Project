#pragma once
#include "GLFW/glfw3.h"
namespace gps {
    class DeltaTime
    {
    public:
        void initializeDeltaTime();
        void calculateDeltaTime();
        float getDeltaTime();
        float getTranslationSpeed();
        float getRotationSpeed();

    private:

        float deltaTime = 0.0f;
        float lastFrame;
        float rotationConstant = 10.0f;
        float translationConstant = 3.0f;
    };
}