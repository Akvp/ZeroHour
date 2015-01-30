# OpenGL demo
Made in C++ using OpenGL API <br>
This is an OpenGL demo showcasing many rasterizing techniques used in games such as

 - Phong Shading
 - Diffuse/Normal/Specular Texture Mapping (normal mapping not fully implemented)
 - Particle System using transform feedback
 - Terrain generated using heightmap
 - Multiple lighting techniques such as directional sun light, point light and spotlight

##(Ready to go)
To run the demo, go to the Release folder and execute astria.exe <br> <br>
**Disclaimer: the project has only been extensively tested on nVidia GPU <br> 
May cause unexpected graphical glitches on AMD or Intel GPU**
<br><br>
A video demo will be available soon

##(Development-friendly)
This project was built using Visual Studio 2013 <br>
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
__0.1.9__<br><br>
For more information please refer to changelog.txt file 

##TODO List
- Fix a small bug about specular texture mapped onto meshes without specular textures
- Add background music & sound effects
- Add normal mapping
- Implement realistic water with reflection and refraction
- Implement OPTION button in pause menu to change graphics options