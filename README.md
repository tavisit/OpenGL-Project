![UTCN](https://doctorat.utcluj.ro/images/utcn-logo.png)
## Student Octavian-Mihai Matei
## Group 3043

<div style="page-break-after: always;"></div>

# OpenGL project C++

# Table of contents

* [Subject specification](#specifications)
* [Scenario](#scenario)
  * [Scene and objects description](#scene-and-objects-description)
  * [Functionalities](#functionalities)
* [Implementation details](#implementation-details)
  * [Functions and special algorithms](#functions-and-special-algorithms)
  * [Graphics model](#graphics-model)
  * [Data structures](#data-structures)
  * [Class hierarchy](#class-hierarchy)
* [Graphical user interface presentation / user manual](#graphical-user-interface-presentation)
* [Conclusions](#conclusions)
* [References](#references)

<div style="page-break-after: always;"></div>

### QR to the [github repository](https://github.com/tavisit/OpenGL-Project)
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/qr-code.png?raw=true)

<div style="page-break-after: always;"></div>

# Specifications

The subject of the project consists in the photorealistic presentation of 3D objects using OpenGL library. The user directly manipulates by mouse and keyboard inputs the scene of objects.
For this purpose, an ancient scene was created where the user can interact with the camera, move around the town, day-night cycle and many more functionalities.

<div style="page-break-after: always;"></div>

# Scenario
## Scene and objects description

The scene is composed of a town with buildings, a town obelisk square, a statue plazza, some stalls, street lights and city walls. In order to make the city "alive", the environment depicts a quent ancient/medieval city with ambiental city sounds, water splashing sound effects and merchants sounds around the stalls.
The town has the following elements:
* City Walls
* City Gate Walls
* Streets
* Street Lamps with dynamic lighting
* A monument at the center of it
* A city forum where the people of the city could meet and trade
* Merchant stalls
* Grass
* Skybox

These elements were combined in order to produce the following demo images:
* Map view of the city
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/map_demo.png?raw=true)
* Night time atmosphere
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/night_time.png?raw=true)
* View of the forum
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/water.png?raw=true)

## Functionalities

The player has many ways to control the world in this project. A clear way to see these controls is to start the game, because they are written in the cmd before the game opens GLWindow. For the ease of explanation, these are:
* WASD for movement
* Mouse to look around
* Scroll to change FOV
* F fullscreen/windowed mode
* M Map mode
* K walking/fly mode
* N autoday on/off
* if autoday off then:
  * \- decrease time of day
  * \+ increase time of day
* , decrease movement speed
* . increase movement speed
* 1 turn on flashlight
* 2 turn off flashlight
* 7 GL_LINE vision mode
* 8 GL_POINT vision mode
* 9 GL_FILL vision mode

A small animation will play everytime the user enters or exists the map mode. In the map mode, the movement of the camera is blocked, the mouse events are disabled, the user cannot switch between walking and fly mode and the player cannot turn off/on the flashlight.

<div style="page-break-after: always;"></div>

# Implementation details

## Functions and special algorithms

A range of algorithms were used to create the atmosphere and the look of the application:
1. Blinn–Phong reflection model
2. Point light
3. Spotlight light
4. Dynamic Percentage-closer filtering
5. Day-Night cycle with moving Sun and responsive street lights
6. Map Animation

### Blinn–Phong reflection model

### Pointlight

### Spotlight

### Dynamic Percentage-closer filtering

### Day-Night cycle with moving Sun and responsive street lights

### Map Animation

## Graphics model

Almost every model was created by me in Blender using textures from internet[^2^3](references). These sites only provided the ambient texture, so I've used the free tool NormalMap-Online[^4](#references) in order to create the Normal, Specular and sometimes the displacement maps. This part was very fun, because I really enjoyed creating something from scratch and seeing it in the world that I've created. The only model that was imported from internet[^1](#references) was the statue, because I do not posess such artistic skills.

For the sound effects, Freesound.org[^5](#references) and Pixabay.com[^2](#references) were the main sources, but I've also recorded my own voice for the merchants shouts.

## Data structures

An array of data structures were used. The most useful and interesting were:
1. POINT_LIGHT struct in .frag and Objectmanager.cpp
```
struct POINT_LIGHT
{
    vec3 location; // location of the light
    vec3 color;    // color of the light
    float intensity; // intensity of the light
	// 3 locations
};
```
3. SPOT_LIGHT struct in .frag and Objectmanager.cpp
```
struct SPOT_LIGHT
{
    int spotinit; // if the spot light is on/off
    vec3 spotLightDirection; // vector for the light direction
    vec3 spotLightPosition; // location of the light
	// 3 locations
};
```
5. std::vector<gps::InGameObject> object

<div style="page-break-after: always;"></div>

## Class hierarchy
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/uml.png?raw=true)

<div style="page-break-after: always;"></div>

# Graphical user interface presentation

<div style="page-break-after: always;"></div>

# Conclusions

<div style="page-break-after: always;"></div>

# References

1. [Low Poly Roman Insula 1 by lexferreira89](https://sketchfab.com/3d-models/low-poly-roman-insula-1-wip-6562e2ffeb3c4bd8ae5b714c6600db20)
2. [Pixabay](https://pixabay.com/)
3. [Solar texture](https://www.solarsystemscope.com/textures/)
4. [NormalMap-Online](https://cpetry.github.io/NormalMap-Online/)
5. [Freesound](https://freesound.org/)
