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
1. Point light
2. Spotlight light
3. Dynamic Percentage-closer filtering
4. Water movement
5. Day-Night cycle with moving Sun and responsive street lights
6. Map Animation
7. Music and positional sounds


### Pointlight

![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/night_time.png?raw=true)

### Spotlight

![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/flashlight.png?raw=true)

### Dynamic Percentage-closer filtering

It is used to create soft shadows or to create the apparent increase in shadow map resolution. In the project, it is implemented a special flavour of this algorithm, in the sense that it is calculated the distance from the fragment to the camera and depending on that, the resolution of the PCF will vary from a map of 1x1 (or a single pass) to a map resolution of 11x11 in order to create smooth and soft shadows when the player is close to an object. If the user is at a distance greater than 50 units, then the resolution will be 1x1. As the player gets closer, the resolution increases, until the camera is at 5 units, then the resolution remains at 11x11, in order not to burden the GPU with more calculations than it is necessary. This was implemented in order to simulate the multiple shadow map passes used in modern computer graphics. 

```
float shadow = 0.0;
vec2 texelSize = 1.0/textureSize(shadowMap,0);
int rangeShadow = max(0, min(5,int(-0.111111*length(fPosEye) + 5.555556)));

for(int x=-rangeShadow;x<=rangeShadow;++x)
{
	for(int y=-rangeShadow;y<=rangeShadow;++y)
	{ 
		float pcfDepth = texture(shadowMap,normalizedCoords.xy+vec2(x,y)*texelSize).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
	}
}
```
A comparison between two PCF resolutions can be observed by looking at the shadow cast near the camera and on the water level:
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/PCF.png?raw=true)

### Water movement

Water level varies pseudorandomly using a LFSR random generator function[^6](#reference). The function creates a random number with the seed representing the current time and fragment vertex position. Then, it gets added to a variable that assures that the water level flows in a pretty realistic way, without breakes in the average mesh of the object.

```
vec2 resolution = vec2(1024,1024);
vec2 cPos = -1.0 + 2.0 * vPosition.xy / resolution.xy;
float cLength = length(cPos);
vec2 uv = vPosition.xy/resolution.xy+(cPos/cLength)*cos(cLength*12.0-time*4.0) * 0.05;
vec3 newPosition = vPosition;
float disp = noise3f(newPosition*time);
newPosition.y = newPosition.y  + sin(disp + uv.y)/50.0f;
```

### Day-Night cycle with moving Sun and responsive street lights

### Map Animation

### Music and positional sounds

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
2. SPOT_LIGHT struct in .frag and Objectmanager.cpp
```
struct SPOT_LIGHT
{
    int spotinit; // if the spot light is on/off
    vec3 spotLightDirection; // vector for the light direction
    vec3 spotLightPosition; // location of the light
	// 3 locations
};
```
3. std::vector<gps::InGameObject> object

This data structure was used in order to create enough instances of InGameObject classes to store the information of each object. It was chosen this behavior, in order to save resources when the application runs, as well as future proofing the implementation if a frustum culling or similar implementation is added later, which might require the programmer to dynamically control all the objects in the scene.

<div style="page-break-after: always;"></div>

## Class hierarchy
![image](https://github.com/tavisit/OpenGL-Project/blob/main/git_resources/uml.png?raw=true)

<div style="page-break-after: always;"></div>

# Conclusions

<div style="page-break-after: always;"></div>

# References

1. [Low Poly Roman Insula 1 by lexferreira89](https://sketchfab.com/3d-models/low-poly-roman-insula-1-wip-6562e2ffeb3c4bd8ae5b714c6600db20)
2. [Pixabay](https://pixabay.com/)
3. [Solar texture](https://www.solarsystemscope.com/textures/)
4. [NormalMap-Online](https://cpetry.github.io/NormalMap-Online/)
5. [Freesound](https://freesound.org/)
6. [Water waves pattern](https://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/)
