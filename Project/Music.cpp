#include "Music.hpp"

namespace gps {
    Music::Music()
    {
        SoundEngine = createIrrKlangDevice();

        cityAmbiance = SoundEngine->play2D("Music/city_humming.wav", true, false, true);
        cityAmbiance->setVolume(0.1);

        waterFountainSound = SoundEngine->play3D("Music/fountain.mp3", vec3df(0, 0, 80), true, false, true); //position at the fountain
        waterFountainSound->setVolume(1);
        waterFountainSound->setMinDistance(10.0f);

        wineMerchant = SoundEngine->play3D("Music/Wine.mp3", vec3df(80, 0, 80), true, false, true);
        wineMerchant->setVolume(1);
        wineMerchant->setMinDistance(1.0f);

        waresMerchant = SoundEngine->play3D("Music/Wares.mp3", vec3df(-80, 0, 80), true, false, true);
        waresMerchant->setVolume(1);
        waresMerchant->setMinDistance(1.0f);
    }
    void Music::update(gps::Camera myCamera)
    {

        vec3df cameraPosition = vec3df(myCamera.getCameraPosition().x, myCamera.getCameraPosition().y, myCamera.getCameraPosition().z);
        vec3df cameraDirection = vec3df(myCamera.getCameraFrontDirection().x, myCamera.getCameraFrontDirection().y, myCamera.getCameraFrontDirection().z);

        SoundEngine->setListenerPosition(cameraPosition, cameraDirection);
    }
    Music::~Music()
    {
    }
}