#ifndef Music_hpp
#define Music_hpp

#include "Camera.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <irrKlang.h>
using namespace irrklang;


namespace gps
{
    /// <summary>
    /// Music object that controls the sound of the application
    /// </summary>
    class Music {
    public:
        /// <summary>
        /// Object constructor
        /// </summary>
        Music();
        /// <summary>
        /// Method that updates the location of the music related to the camera
        /// </summary>
        /// <param name="myCamera"></param>
        void update(gps::Camera myCamera);

        /// <summary>
        /// Object deconstructor
        /// </summary>
        ~Music();
    private:

        ////////////////////////////////////////////////////////////////
        // Private variables
        ////////////////////////////////////////////////////////////////

        ISoundEngine* SoundEngine;

        #define WATER_NUMBER 3
        #define WARES_NUMBER 2
        #define WINE_NUMBER 2

        ISound* cityAmbiance;
        std::vector<ISound*> waterSounds;
        std::vector<ISound*> waresMerchant;
        std::vector<ISound*> wineMerchant;
    };
}

#endif
