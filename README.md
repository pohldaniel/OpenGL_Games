# OpenGL_Games

The project 01Platformer is a refactoring of https://github.com/BuildSucceeded/2D-Platformer

The project 02Deathjump will be a refactoring of https://github.com/xSnapi/deathjump based on custom OpenGL. Finally the collisions working as expected. Change the DEBUGCOLLISION flag in Constants.h to see the collider.

![Deathjump](https://user-images.githubusercontent.com/30089026/159373182-ca685728-c27f-4c09-83f8-df297a28a900.png)

The project 03Marvin will be a refactoring of https://github.com/TylerSandman/Marvin. At the moment I spent some time with Box2D and developing a CharachterController. I had to made some sligth changes to this Framework, for enabling kinematic <-> kinematic colliding and track it at the ContactListener.

-> at line 332 in b2_body.h add the function "void SetCollide(bool flag);"  
-> at line 422 in b2_body.h add the "e_collideFlag" to the enum  
-> at line 663 in b2_body.h  define the function "void SetCollide(bool flag){m_flags |= e_collideFlag;}"  
-> at line 397 in b2_body.cpp change the "return false" to "return (m_flags & b2Body::e_collideFlag)"

The project 04Virtueror will be a refactoring of https://github.com/vivaladav/iso-rts

The project 06Voyager needs some assets (Woman.gltf, Woman.png) from https://github.com/gszauer/GameAnimationProgramming I am pretty sure there is a Copyright on it. By my own I managed the basics of "Additive Animation Blending" following the source of Chapter 12 from this repository but using Assimp as model loader.

The project 07Dawn will be a refactoring of https://github.com/iamCode/Dawn using modern OpenGL and batchrendering. Finally the core engine is running, becareful at the moment the code is full of memoryleaks.

The project 08Decals is based on https://github.com/diharaw/texture-space-decals and https://github.com/diharaw/deferred-decals.
