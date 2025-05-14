# Tom Bates CFGAA Assignment

# Qt OpenGL terrain erosion (chosen idea)

## Base noise generation
TODO: Add m_maxHeight and implement in UI.


![image](https://github.com/user-attachments/assets/c4078c79-37fa-4614-a425-e48d06364d7a)
Applied vertical perlin noise (fractal) on my vertex array using the [PerlinNoise](https://github.com/Reputeless/PerlinNoise) header library. Next step is to build a VAO from this.



![image](https://github.com/user-attachments/assets/2b8ff2f1-6c0c-489e-90a9-5c6d2fc40d3d)
Implemented triangle mesh rendering by looping over the grid of vertices to process each quad, then splits quads into triangles.



![image](https://github.com/user-attachments/assets/a18b0b55-aa3d-4e60-bcf3-327040691784)
Added height shader that takes normalised vertex y coordinate as colour input.


[![UI Demo](https://markdown-videos-api.jorgenkh.no/url?url=https%3A%2F%2Fwww.youtube.com%2Fwatch%3Fv%3D9tNtFw8fZuY)](https://www.youtube.com/watch?v=9tNtFw8fZuY)
Added some UI controls for the grid and noise generation.


## Hydraulic Erosion (In progress)
![image](https://github.com/user-attachments/assets/4f1f47f2-3d0e-45a0-9554-4e8daba467b4)
Visualised the droplet simulation, just need to implement sediment erosion and deposition based on each droplet.  
