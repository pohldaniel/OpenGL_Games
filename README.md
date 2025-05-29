# OpenGL_Games

The project 01Platformer is a refactoring of https://github.com/BuildSucceeded/2D-Platformer

The project 02Deathjump will be a refactoring of https://github.com/xSnapi/deathjump based on custom OpenGL. Finally the collisions working as expected. Change the DEBUGCOLLISION flag in Constants.h to see the collider.

![Deathjump](https://user-images.githubusercontent.com/30089026/159373182-ca685728-c27f-4c09-83f8-df297a28a900.png)

The project 03Marvin will be a refactoring of https://github.com/TylerSandman/Marvin. At the moment I spent some time with Box2D and developing a CharachterController. I had to made some sligth changes to this Framework, for enabling kinematic <-> kinematic colliding and track it at the ContactListener.

-> at line 332 in b2_body.h add the function "void SetCollide(bool flag);"  
-> at line 422 in b2_body.h add the "e_collideFlag" to the enum  
-> at line 663 in b2_body.h  define the function "void SetCollide(bool flag){m_flags |= e_collideFlag;}"  
-> at line 397 in b2_body.cpp change the "return false" to "return (m_flags & b2Body::e_collideFlag)"

The project 04Virtueror will be a refactoring of https://github.com/vivaladav/iso-rts.

The project 05UniversityRacer is based on https://github.com/david-sabata/UniversityRacer.

The project 06Voyager uses some aspects from https://github.com/gszauer/GameAnimationProgramming. By my own I managed the basics of "Additive Animation Blending" following the source of Chapter 12 from this repository but using Assimp as model loader.

The project 07Dawn will be a refactoring of https://github.com/iamCode/Dawn using modern OpenGL and batchrendering. Finally the core engine is running, becareful at the moment the code is full of memoryleaks.

The project 08Decals is based on https://github.com/diharaw/texture-space-decals and https://github.com/diharaw/deferred-decals.

The project 09HDRBloom is based on https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom.

The project 10SSS is my second attempt on porting https://github.com/iryoku/separable-sss to OpenGl.
![sss2](https://user-images.githubusercontent.com/30089026/212480333-1af139e9-85cb-4611-bfe9-b6b526633fc5.png)

The transmittance pass "right" and the blur pass "middle right" seems ok for now. But unfrotunataly I have some major issues with the specularities "left" and applying the "Beckmann-Texture" in a correct way.

The project 11VolumetircTexture is a port of https://developer.download.nvidia.com/SDK/10.5/opengl/screenshots/samples/render_texture_3D.html I was abel to exchange some of the "CG-Shader" with "Compute-Shader". To get the same trackball behavior I add some header out of the Nvidia SDK, the copyright is written down in every file. To get rid of the "glut32.lib" I had to do some slightly modifications to this files. 
The project also contains some code from https://prideout.net/blog/old/blog/index.html@p=60.html and https://prideout.net/blog/old/blog/index.html@p=64.html. The models are from https://klacansky.com/open-scivis-datasets

![Volume](https://user-images.githubusercontent.com/30089026/213771775-9ea01f87-61a1-4ad5-b60d-87a6a65e5181.png)

The project 12Thikness is based on https://prideout.net/blog/old/blog/index.html@p=51.html

The project 13DepthPeeling is a port of https://developer.download.nvidia.com/SDK/10.5/opengl/screenshots/samples/dual_depth_peeling.html using modern OpenGl. See also https://developer.download.nvidia.com/SDK/10/opengl/samples.html

The project 21Lugaru is heavily based on https://gitlab.com/osslugaru/lugaru.

The project 22Voyager is heavily based on https://github.com/OnixMarble/Titan-Voyager-Custom-Game-Engine.

The project 23Ballenger is based on https://www.youtube.com/watch?v=AOWDMiX94_M the terrain culling is from http://3dcpptutorials.sk/index.php?id=58 the volumetric clouds are from https://github.com/fede-vaccaro/TerrainEngine-OpenGL the backend noisegen is from https://github.com/wangyiyun/Cloud-Generation. Another cloud approach is from https://github.com/simondevyoutube/Shaders_Clouds1.

The project 24Ragdoll contains files from https://github.com/urho3d/Urho3D and https://github.com/cadaver/turso3d

The project 25FallingSand is based on https://github.com/PieKing1215/FallingSandSurvival

The project 26SDF is based on https://github.com/diharaw/sdf-baking. I also had to modify my TdrDelay described here https://helpx.adobe.com/substance-3d-painter/technical-support/technical-issues/gpu-issues/gpu-drivers-crash-with-long-computations-tdr-crash.html According to the long SDF computation time.

The project 27JellyCar is based on https://github.com/DrakonPL/JellyCar. 

The project 28TowerDefense is from https://www.funneractic.com/

The project 30Robot is from https://github.com/guillaume-haerinck/imac-tower-defense. The needed SDK's are from https://www.fmod.com/ and https://www.noesisengine.com/forums/viewtopic.php?f=14&t=1491.

The project 31ECSLua is from https://github.com/danielmapar/Cpp2dGameEngine

The project Hexagonal contains files from https://github.com/tmf7/Engine-of-Evil

The project 32CrimsonRush is from https://github.com/WilKam01/LuaCGame

The project 33AntWare is from https://github.com/YamanQD/AntWare

The project 35Octree contains source code and assets from https://github.com/A-Ribeiro/OpenGLStarter, https://github.com/simondevyoutube/ThreeJS_Tutorial_ParticleSystems, https://github.com/Vulpinii/grass-tutorial_finalcode and https://github.com/LesleyLai/GLGrassRenderer/tree/main, https://github.com/matus-chochlik/oglplus, https://github.com/byhj/OpenGL-Redbook, https://github.com/cadaver/turso3d, https://github.com/Lumak/Urho3D-KinematicCharacterController, https://github.com/Lumak/Urho3D-Skinned-Armor, https://github.com/urho3d/urho3d, https://github.com/visionary-3d/three-raymarch, https://github.com/iryoku/separable-sss, https://github.com/Rodousse/SSSGems, https://www.mbsoftworks.sk/tutorials/opengl3/27-occlusion-query and https://github.com/FairZ/Flame-Sim-OpenGL.

One rendering I have achieved with this iteration

![SSS_2](https://github.com/pohldaniel/OpenGL_Games/assets/30089026/70c913f0-1470-4cbc-b42f-55debead256d)

The project 36Kart is from https://github.com/jasonmzx/cppkart

The project 37MonsterHunter is from https://github.com/clear-code-projects/Python-Monsters and https://www.youtube.com/watch?v=fo4e3njyGy0

![screen1](https://github.com/user-attachments/assets/f23bf17f-bba5-4466-8baf-41a91455868d)
![antware](https://github.com/user-attachments/assets/21147167-cc31-4ca4-b68e-88bbfe5314bd)

The project 38Vulkan is a really simple vulkan example a more polished one can be foud here https://github.com/pohldaniel/Vulkan

The project 39Adrian is from https://code.google.com/archive/p/adrian/. For building the origin on windows take a look here https://github.com/pohldaniel/Adrian

The project 40RecastDemo is from https://github.com/recastnavigation/recastnavigation and serves as "look up" for some library features 
