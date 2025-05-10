# Tom Bates CFGAA Assignment
TODO: 
removing the Emitter.h class without breaking the program
Updating the Qt UI to customise Plane class.

## Ideas

I have not finalised an idea for the assignment, but here are some of my ideas:


1. WebGPU terrain erosion
2. OpenGL Physics engine

Edit: I will be doing OpenGL Terrain Erosion

# OpenGL terrain erosion (chosen idea)

## Base noise generation

Applied vertical perlin noise (fractal) on my vertex array using the [PerlinNoise](https://github.com/Reputeless/PerlinNoise) header library. Next step is to build a VAO from this.
![image](https://github.com/user-attachments/assets/c4078c79-37fa-4614-a425-e48d06364d7a)

Implemented triangle mesh rendering by looping over the grid of vertices to process each quad, then splits quads into triangles.

![image](https://github.com/user-attachments/assets/2b8ff2f1-6c0c-489e-90a9-5c6d2fc40d3d)

Added height shader that takes normalised vertex y coordinate as colour input.

![image](https://github.com/user-attachments/assets/a18b0b55-aa3d-4e60-bcf3-327040691784)
