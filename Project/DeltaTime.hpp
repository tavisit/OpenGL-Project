#ifndef DeltaTime_hpp
#define DeltaTime_hpp
#include "GLFW/glfw3.h"
#include "iostream"
namespace gps {
    class DeltaTime
    {
    public:
        void initializeDeltaTime();
        void calculateDeltaTime(bool printFps);
        float getDeltaTime();
        float getTranslationSpeed();
        float getRotationSpeed();
        int getFps();

    private:

        float deltaTime = 0.0f;
        float lastFrame;
        float rotationConstant = 20.0f;
        float translationConstant = 20.0f;

        int framesPassed = 0;
        float timePassed = 1.0f;
        int FPS = 0;
    };
}
#endif