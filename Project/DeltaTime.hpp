#ifndef DeltaTime_hpp
#define DeltaTime_hpp
#include "GLFW/glfw3.h"
#include "iostream"
namespace gps {

    /// <summary>
    /// Class that stores and calculates the delta time
    /// </summary>
    class DeltaTime
    {
    public:

        /// <summary>
        /// Initialize the delta time variables
        /// </summary>
        void initializeDeltaTime();

        /// <summary>
        /// Calculate the current frame delta time
        /// </summary>
        /// <param name="printFps">Boolean that dictates if the FPS should be printed on console</param>
        void calculateDeltaTime(bool printFps);

        /// <summary>
        /// Getter for the delta time variable
        /// </summary>
        /// <returns></returns>
        float getDeltaTime();

        /// <summary>
        /// Getter for the custom translation speed
        /// </summary>
        /// <returns>The product of a translation speed constant with delta time</returns>
        float getTranslationSpeed();

        /// <summary>
        /// Getter for the custom rotation speed
        /// </summary>
        /// <returns>The product of a rotation speed constant with delta time</returns>
        float getRotationSpeed();

        /// <summary>
        /// Getter for the fps count
        /// </summary>
        /// <returns></returns>
        int getFps();

    private:


        ////////////////////////////////////////////////////////////////
        // Private variables and initialization
        ////////////////////////////////////////////////////////////////

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