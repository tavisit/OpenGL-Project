#include "Music.hpp"

namespace gps {
    Music::Music()
    {
        SoundEngine = createIrrKlangDevice();

        cityAmbiance = SoundEngine->play2D("Music/city_humming.wav", true, false, true);
        cityAmbiance->setVolume(0.2);

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
        for (int i = 0; i < WARES_NUMBER; i++)
        {
            ISound* genericSound;
            genericSound = SoundEngine->play3D("Music/Wares.mp3", vec3df(0, 0, 80), true, false, true); //position at the fountain
            genericSound->setVolume(1);
            genericSound->setMinDistance(2.0f);
            waresMerchant.push_back(genericSound);
        }
        waresMerchant[0]->setPosition(vec3df(55, -2.4f, 15));
        waresMerchant[1]->setPosition(vec3df(-80, -3.0f, 60));
        for (int i = 0; i < WINE_NUMBER; i++)
        {
            ISound* genericSound;
            genericSound = SoundEngine->play3D("Music/Wine.mp3", vec3df(0, 0, 80), true, false, true); //position at the fountain
            genericSound->setVolume(1);
            genericSound->setMinDistance(2.0f);
            wineMerchant.push_back(genericSound);
        }
        wineMerchant[0]->setPosition(vec3df(-70, -3.0f, 100));
        wineMerchant[1]->setPosition(vec3df(70, -3.0f, 100));
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