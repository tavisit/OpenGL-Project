#include "Music.hpp"

namespace gps {
    Music::Music()
    {
        SoundEngine = createIrrKlangDevice();

        cityAmbiance = SoundEngine->play2D("Music/city_humming.wav", true, false, true);
        cityAmbiance->setVolume(0.1);

        for (int i = 0; i < WATER_NUMBER; i++)
        {
            ISound* genericSound;
            genericSound = SoundEngine->play3D("Music/fountain.mp3", vec3df(0, 0, 80), true, false, true); //position at the fountain
            genericSound->setVolume(1);
            genericSound->setMinDistance(10.0f);
            waterSounds.push_back(genericSound);
        }
        waterSounds[1]->setPosition(vec3df(85.0f, -2.8f, 25.0f));
        waterSounds[1]->setMinDistance(5.0f);
        waterSounds[2]->setPosition(vec3df(85.0f, -2.8f, 45.0f));
        waterSounds[2]->setMinDistance(5.0f);

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