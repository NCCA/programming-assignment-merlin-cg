# Tom Bates CFGAA Assignment
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Qt OpenGL terrain erosion (chosen idea)
## 1. Introduction
I developed a terrain generator with hydraulic erosion simulation using C++, OpenGL, and Qt. This project demonstrates procedural terrain generation using Perlin noise and realistic terrain erosion through a water droplet simulation algorithm. The application allows real-time adjustment of terrain parameters and visualization of the erosion process.

Key features:
- Procedural terrain generation using Perlin noise
- Realistic hydraulic erosion simulation
- Interactive parameter controls
- Real-time visualization of erosion process
- Object-oriented design following S.O.L.I.D principles
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 2. Research and Background
### Terrain Generation & Idea
My prior research into terrain erosion (For a Maya Python project last year) led me to explore Perlin noise for realistic terrain generation. While basic Perlin noise creates smooth, rolling hills, I enhanced it by applying techniques such as [Fractal Brownian Motion (FBM)](https://www.redblobgames.com/maps/terrain-from-noise/ ) which involves layering multiple octaves of noise at different frequencies and amplitudes. To create more realistic mountainous features, I applied the absolute value to each noise layer to sharpen ridges, then inverted low-frequency, high-amplitude layers to simulate natural erosion patterns and tectonic uplift. However, I only got so far with this method and was limited by the speed on Python which inspired me to create a new terrain generator in C++ that incorporates realistic Hydraulic Erosion.

### Hydraulic Erosion
For the erosion algorithm, I researched several approaches and was particularly influenced by [Sebastian Lague's implementation](https://www.youtube.com/watch?v=eaXk97ujbPQ ) and the academic paper ["Fast Hydraulic Erosion Simulation and Visualization on GPU" by Mei, Decaudin, and Hu](https://hal.inria.fr/inria-00402079/document ).

The hydraulic erosion process simulates:
1. Water droplet creation at random positions
2. Droplet movement based on terrain gradient
3. Sediment pickup based on water velocity and slope
4. Sediment deposition when water slows or evaporates
5. Terrain modification based on erosion and deposition

This process creates realistic features like river beds, valleys, and ridges that are difficult to achieve with noise-based generation alone.
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 3. Design and Architecture
### Class Structure
The main classes in the system are:

- `Plane`: Manages the terrain mesh and coordinates generation and erosion
- `TerrainGenerator`: Interface for terrain generation strategies
- `PerlinNoiseGenerator`: Concrete implementation of terrain generation using Perlin noise
- `HydraulicErosion`: Handles the erosion simulation
- `DropletVisualize`: Visualizes the droplet paths during erosion
- `NGLScene`: Manages OpenGL rendering and camera controls
- `MainWindow`: Provides the Qt user interface
 

### Design Patterns
I implemented the **Strategy Pattern** for terrain generation, allowing different generation algorithms to be swapped at runtime. My terrain generation interface "TerrainGenerator.h" defines a common interface that all terrain generation strategies must implement. Currently, the only strategy for this interface is PerlinNoiseGenerator.h, however, It follows the Open/Close principle so I can easily add new terrain generation algorithms such as Diamond/Square, Voronoi, Manhatten, Blurred DLA. This follows the Open/Closed Principle by making the system extensible without modifying existing code. 


### Data Structures
- Height grid: Stored as a 1D vector of 3D vectors for efficient memory access
- Droplet structure: Models water droplets with position, direction, speed, water content, sediment load, and lifetime properties
- Brush indices and weights: Pre-computed for erosion radius to optimize performance
- Droplet trail points: Vector of 4D vectors (x, y, z, lifetime) for visualization
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 4. Implementation Details

### 4.1 Terrain Generation
The terrain is generated using Perlin noise with configurable frequency and octaves. The implementation uses a height grid where each vertex's y-coordinate is determined by the noise function:

```cpp
float height_normalized = std::abs(perlin.octave2D_01(
    noiseInputX * m_frequency,
    noiseInputZ * m_frequency,
    m_octaves, 0.5));
vertex.m_y = height_normalized * maxHeight;
```

The mesh is then constructed in the Plane class by creating triangles from adjacent vertices in the grid.

### 4.2 Hydraulic Erosion
The erosion algorithm simulates water droplets flowing over the terrain:

1. Droplets are created at random positions
2. For each simulation step:
   - Calculate height and gradient at current position
   - Update droplet direction based on gradient and inertia
   - Move droplet position
   - Calculate new height and determine if droplet is moving uphill or downhill
   - Erode or deposit sediment based on capacity and slope
   - Update droplet speed and water content

The erosion radius determines how widely the erosion effect is distributed around the droplet's position, creating more natural-looking results.


### 4.3 Visualization
The terrain is rendered with a glsl height-based color shader:

```glsl
// Vertex shader
varying float height;
void main() {
    height = gl_Vertex.y / maxTerrainHeight;
    gl_Position = MVP * gl_Vertex;
}

// Fragment shader
varying float height;
void main() {
    vec3 color = mix(vec3(0.2, 0.3, 0.1), vec3(0.8, 0.8, 0.7), height);
    gl_FragColor = vec4(color, 1.0);
}
```

Droplet trails are visualized using point sprites with color based on the droplet's lifetime.
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 5. Object-Oriented Programming Features
### SOLID Principles Application

Throughout the development of this terrain generator, I've refactored to incorporate aspects of SOLID principles. My project evolved from a monolithic Plane class to a more modular design following SOLID principles. By extracting the HydraulicErosion class and implementing the TerrainGenerator interface with a PerlinNoiseGenerator implementation, I separated responsibilities and improved maintainability. The TerrainGenerator interface demonstrates the Open/Closed Principle by allowing new generation algorithms without modifying existing code. While I've partially implemented Dependency Inversion through abstraction-based dependencies, I recognize opportunities for improvement, such as creating an erosion algorithm interface, further separating rendering functions and reducing the coupling of paramaters. 


### Encapsulation
Private member variables with public accessor methods ensure data integrity:

```cpp
private:
    float m_noiseFrequency = 3.0f;
    int m_noiseOctaves = 6;
    int m_maxHeight = 90;

public:
    void setNoiseFrequency(float freq) { /* ... */ }
    float getNoiseFrequency() const { return m_noiseFrequency; }
```

### Polymorphism
The Strategy Pattern for terrain generation demonstrates polymorphism:

```cpp
// Interface
class TerrainGenerator {
public:
    virtual void generateTerrain(std::vector<ngl::Vec3>& heightGrid, 
                               unsigned int width, 
                               unsigned int depth, 
                               float spacing,
                               int maxHeight) = 0;
};

// Implementation
class PerlinNoiseGenerator : public TerrainGenerator {
public:
    void generateTerrain(std::vector<ngl::Vec3>& heightGrid, 
                        unsigned int width, 
                        unsigned int depth, 
                        float spacing,
                        int maxHeight) override;
};
```
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 6. GUI and User Interaction
The application provides a Qt5-based GUI using the [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet) with controls for:

- Terrain dimensions (width, depth)
- Noise parameters (frequency, octaves, height)
- Erosion parameters (droplet count, lifetime)
- Visualization options (wireframe, droplet trails)

Noise & Grid parameter changes are immediately reflected in the terrain, allowing for interactive experimentation.

Keyboard controls can be used for cases such as quick erode(E), toggle wireframe(W) and droplet visualization(V). 
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 7. Progess Screenshots

### Noise generation
![image](https://github.com/user-attachments/assets/c4078c79-37fa-4614-a425-e48d06364d7a)
Applied vertical perlin noise (fractal) on my vertex array using the [PerlinNoise](https://github.com/Reputeless/PerlinNoise) header library. Next step is to build a VAO from this.



![image](https://github.com/user-attachments/assets/2b8ff2f1-6c0c-489e-90a9-5c6d2fc40d3d)
Implemented triangle mesh rendering by looping over the grid of vertices to process each quad, then splits quads into triangles.



![image](https://github.com/user-attachments/assets/a18b0b55-aa3d-4e60-bcf3-327040691784)
Added height shader that takes normalised vertex y coordinate as colour input.


[![UI Demo](https://markdown-videos-api.jorgenkh.no/url?url=https%3A%2F%2Fwww.youtube.com%2Fwatch%3Fv%3D9tNtFw8fZuY)](https://www.youtube.com/watch?v=9tNtFw8fZuY)
Added some UI controls for the grid and noise generation.


### Hydraulic Erosion 
![image](https://github.com/user-attachments/assets/4f1f47f2-3d0e-45a0-9554-4e8daba467b4)
Visualised the droplet simulation, just need to implement sediment erosion and deposition based on each droplet.  

![image](https://github.com/user-attachments/assets/2fb6b1c4-df21-4d7a-bfe2-fa17dcf5c523)
![image](https://github.com/user-attachments/assets/c88e3eb1-f9b6-4e23-9254-c05388783a32)
Finally got sediment erosion and deposition after quite a lot of debugging.
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 8. Results and Visual Impact

### Before erosion vs After (40,000)
![image](https://github.com/user-attachments/assets/954eb2df-1a78-4eff-8c3a-35db2ec38181)

### Comparison to real world example 
![image](https://github.com/user-attachments/assets/b0246ae8-8ae2-4b7d-85dd-76f77f2c7d7e)

### Video Demonstration of program
- https://youtu.be/Fyr37X3A0ic
- https://youtu.be/IjOlhfDJ07Q (with extra UI controls)
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 9. Build instructions
### Prerequisites
- C++17 compatible compiler
- CMake 3.26.5 or higher
- Qt 5.15.2 or higher
- NGL library (Current OpenGL & GLSL versions 4.6.0)

### Building
```bash
# Clone the repository
git clone https://github.com/NCCA/programming-assignment-merlin-cg.git
cd programming-assignment-merlin-cg

# Configure with CMake
mkdir build
cd build
cmake ..

# Build
make
```
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 10. Future Improvements
- Heightmap Image Export
- GPU acceleration for erosion simulation using compute shaders
- Additional terrain generation algorithms (Diamond-Square, Voronoi)
- Texture mapping based on slope and height
- Vegetation simulation based on terrain features
- Water flow map generation
<br>

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## 11. References and Bibliography


- Perlin, K., 1985. An image synthesizer. SIGGRAPH Computer Graphics, 19(3), pp.287–296.
- Musgrave, F.K., Kolb, C.E. and Mace, R.S., 1989. The synthesis and rendering of eroded fractal terrains. ACM SIGGRAPH Computer Graphics, 23(3), pp.41–50.
- Mei, X., Decaudin, P. and Hu, B., 2007. Fast hydraulic erosion simulation and visualization on GPU. In: Pacific Graphics 2007. Pacific Graphics, pp.47–56.
- [Sebastian Lague - Hydraulic Erosion](https://www.youtube.com/watch?v=eaXk97ujbPQ)
- [Redblobgames - Noise-based Map Generation](https://www.redblobgames.com/maps/terrain-from-noise/)
- [PerlinNoise Header Library](https://github.com/Reputeless/PerlinNoise)
- [NGL Graphics Library](http://nccastaff.bournemouth.ac.uk/jmacey/GraphicsLib/index.html)
- [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet)

