#ifndef Music_hpp
#define Music_hpp

#include "Camera.hpp"
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

        ISound* cityAmbiance;
        ISound* waterFountainSound;
        ISound* waresMerchant;
        ISound* wineMerchant;
    };
}

#endif
