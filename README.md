# Tom Bates CFGAA Assignment

# Qt OpenGL terrain erosion (chosen idea)


## Idea
My idea of erosion came when trying to improve realism on terrain generation, terrain generation can be used for many cases such as games, cinematic landscapes or virtual production. In past experience I had already played around with modifying perlin noise with methods such as [FBM](https://www.redblobgames.com/maps/terrain-from-noise/) which involves layering multiple octaves of noise. To enhance mountainous features, I applied the absolute value to each noise layer to sharpen ridges, then inverted the low-frequency, high-amplitude layers to simulate natural erosion and uplift. 

## 1. Introduction
I developed a terrain generator with hydraulic erosion simulation using C++, OpenGL, and Qt. This project demonstrates procedural terrain generation using Perlin noise and realistic terrain erosion through a water droplet simulation algorithm. The application allows real-time adjustment of terrain parameters and visualization of the erosion process.

Key features:
- Procedural terrain generation using Perlin noise
- Realistic hydraulic erosion simulation
- Interactive parameter controls
- Real-time visualization of erosion process
- Object-oriented design following SOLID principles

## 2. Research and Background

### Terrain Generation & Idea
My previous research into terrain erosion (For a Maya Python project last year) led me to explore Perlin noise as a foundation. While basic Perlin noise creates smooth, rolling hills, I enhanced it by applying techniques such as [Fractal Brownian Motion (FBM)](https://www.redblobgames.com/maps/terrain-from-noise/ ) which involves layering multiple octaves of noise at different frequencies and amplitudes. To create more realistic mountainous features, I applied the absolute value to each noise layer to sharpen ridges, then inverted low-frequency, high-amplitude layers to simulate natural erosion patterns and tectonic uplift. However, I only got so far with this method and was limited by the speed on Python which inspired me to create a new terrain generator in C++ that incorporates realistic Hydraulic Erosion.

### Hydraulic Erosion
For the erosion algorithm, I researched several approaches and was particularly influenced by [Sebastian Lague's implementation](https://www.youtube.com/watch?v=eaXk97ujbPQ ) and the academic paper ["Fast Hydraulic Erosion Simulation and Visualization on GPU" by Mei, Decaudin, and Hu](https://hal.inria.fr/inria-00402079/document ).

The hydraulic erosion process simulates:
1. Water droplet creation at random positions
2. Droplet movement based on terrain gradient
3. Sediment pickup based on water velocity and slope
4. Sediment deposition when water slows or evaporates
5. Terrain modification based on erosion and deposition

This process creates realistic features like river beds, valleys, and ridges that are difficult to achieve with noise-based generation alone.

## Object oriented design examples
For this assignment and for the maintainability of my code it is important that I follow the S.O.L.I.D Object-Oriented Programming Principles.

One example is that I created a Terrain generation interface "TerrainGenerator.h". This defines a common interface that all terrain generation strategies must implement. Currently, the only strategy for this interface is PerlinNoiseGenerator.h, however, It follows the Open/Close principle so I can easily add new terrain generation algorithms such as Diamond/Square, Voronoi, Manhatten, Blurred DLA.


## Base noise generation


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

![image](https://github.com/user-attachments/assets/2fb6b1c4-df21-4d7a-bfe2-fa17dcf5c523)
Before erosion vs After (60,000 droplets)
![image](https://github.com/user-attachments/assets/c88e3eb1-f9b6-4e23-9254-c05388783a32)
a
b
![image](https://github.com/user-attachments/assets/29075580-eec0-4aa5-a0e2-4ad942ba1665)

![image](https://github.com/user-attachments/assets/954eb2df-1a78-4eff-8c3a-35db2ec38181)
20,000 droplets
![image](https://github.com/user-attachments/assets/b0246ae8-8ae2-4b7d-85dd-76f77f2c7d7e)

https://youtu.be/Fyr37X3A0ic

