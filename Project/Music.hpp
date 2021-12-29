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
    class Music {
    public:
        Music();
        void update(gps::Camera myCamera);
        ~Music();
    private:
        ISoundEngine* SoundEngine;

        #define WATER_NUMBER 3

        ISound* cityAmbiance;
        std::vector<ISound*> waterSounds;
        ISound* waresMerchant;
        ISound* wineMerchant;
    };
}

#endif
