# Tom Bates CFGAA Assignment

## Ideas

I have not finalised an idea for the assignment, but here are some of my ideas:


1. WebGPU terrain erosion
2. OpenGL Physics engine


# OpenGL terrain erosion (chosen idea)

## Base noise generation

Applied vertical perlin noise (fractal) on my vertex array. Next step is to build a VAO from this.
![image](https://github.com/user-attachments/assets/c4078c79-37fa-4614-a425-e48d06364d7a)

Implemented triangle mesh rendering by looping over the grid of vertices to process each quad, then splits quads into triangles.

![image](https://github.com/user-attachments/assets/2b8ff2f1-6c0c-489e-90a9-5c6d2fc40d3d)
