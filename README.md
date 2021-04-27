Copyright Accelit.

I. Introduction
Omega is a 6 months game engine project made during the end of our studies.
It is written by Victor JORE and Thibaut PONCHON between January 2020 and June 2020.

This Game Engine is powered by Vulkan 1.2.131.2 and made for performance and realistic rendering. As such, Raytracing is part of the rendering capabilities of this game engine using a Turing GPU.
Obviously this game engine is able to do classic rendering with the rasterization being the default rendering pipeline at launch.

It uses our own math library, assimp, irrklang, PhysX, stb_image, GLFW.

Also, be aware of the needed models and textures inside the main.cpp which may cause a crash if not in the Resources folders. You can use your owns.
glm is not added in dependencies but is needed for raytracing. Please add it manually for now since it will be removed lately.
![Capture d’écran 2021-04-27 191349](https://user-images.githubusercontent.com/44799583/116284157-0b7e8380-a78d-11eb-939c-046463d0497e.png)
![Capture d’écran 2021-04-27 191309](https://user-images.githubusercontent.com/44799583/116284160-0c171a00-a78d-11eb-90d7-a6fa25621c4f.png)
![Capture d’écran 2021-04-27 191337](https://user-images.githubusercontent.com/44799583/116284162-0cafb080-a78d-11eb-9d3c-dd917c2a0e35.png)
