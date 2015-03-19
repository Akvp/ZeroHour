# OpenGL demo
Made in C++ using OpenGL API 

This is an OpenGL demo showcasing many rasterizing techniques used in games such as

 - Phong Shading
 - Diffuse/Specular Texture Mapping
 - Particle System using transform feedback
 - Terrain generated using heightmap
 - Instancing
 - Multiple lighting techniques such as directional sun light, point light and spotlight

##(Ready to go)
Download the demo [**HERE**](https://drive.google.com/file/d/0BwiufLZ-yoAOR1prR3Zod1hwOFE/view?usp=sharing)
**OR**
Clone this repo and run the executable in Release folder


**Disclaimer:** This demo has only been extensively tested on nVidia GPUs 
and may cause unexpected visual artifacts on AMD or Intel GPUs

A video demo will be available soon

##(Development-friendly)
This project was built using Visual Studio 2013 

Requirement to build this project:

 * SDL 2.0
  - SDL2_image
  - SDL2_ttf
  - SDL2_mixer
 * Assimp Model Loader
 * GLM OpenGL Mathematics library
 * GLEW OpenGL Extension Wrangler Library

Many loading parameters can be modified in params.ini file located in the same folder as the executable

##Version
__0.2.3a__

For more information please refer to changelog.txt file 

##TODO List
- ~~Add background music & sound effects~~
- ~~Add text rendering using orthogonal projection~~
- ~~Add normal mapping~~
- Implement realistic water with reflection and refraction
- Implement OPTION button in pause menu to change graphics options