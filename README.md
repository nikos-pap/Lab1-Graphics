# OpenGL Graphics Project
## About
This project is a 3D Object Collision Demo developed using OpenGL as part of a university Graphics course. It simulates real-time collision detection between objects in a 3D space, applying basic physics principles like velocity, acceleration, and object interaction upon impact. The project also incorporates shaders to handle lighting and surface effects, which enhance the visual representation of the scene. By using collision algorithms like bounding volumes, this demo demonstrates how collisions are managed in 3D environments. The project provided valuable experience in both collision detection and shader programming, essential for modern graphics and game development.
## Installation

To run the **3D Object Collision Demo**, follow these steps:

1. **Clone the Repository**:

    ```bash
    git clone https://github.com/nikos-pap/Lab1-Graphics.git
    ```

2. **Open the Solution in Visual Studio**:
    - Navigate to the project folder.
    - Open the `.sln` file in Visual Studio.

3. **Add Dependencies**:
    - Dependencies are located in the `dependencies` folder.
    - Detailed instructions for adding them to the project can be found in the `dependencies/README.txt` file.

4. **Build and Run**:
    - Once the dependencies are set up, build the project by selecting **Build > Build Solution**.
    - If the build succeeds, run the demo by pressing **F5** or selecting **Debug > Start Debugging**.
## Execution Instructions
![Example Image](Images/ExampleImage.png)
Once you run the demo, the program will open in the 3D scene. You can move around the scene, spawn small random shapes, and observe the collisions between the objects in real-time. Use the controls below to navigate and interact with the scene.

### Player Controls

| Action              | Control Key    |
|---------------------|:--------------:|
| **Move** Forward    | `W`            |
| **Move** Backward   | `S`            |
| **Move** Left       | `A`            |
| **Move** Right      | `D`            |
| **Move** Up         | `E`            |
| **Move** down       | `X`            |
| **Rotate** Right    | `L`            |
| **Rotate** Left     | `J`            |
| **Rotate** Up       | `I`            |
| **Rotate** Down     | `K`            |


### Sphere Controls
| Action                 | Control Key    |
|------------------------|:--------------:|
| **Move** Up (+Y)       | `Up Arrow`     |
| **Move** Down (-Y)     | `Down Arrow`   |
| **Move** Left (+X)     | `Left Arrow`   |
| **Move** Right (-X)    | `Right Arrow`  |
| **Move** Forward (+Z)  | `Up Arrow`     |
| **Move** Backwards (-Z)| `Down Arrow`   |
| **Toggle** Texture     | `T`            |


### Game Controls
| Action                 | Control Key    |
|------------------------|:--------------:|
| **Spawn** Random Shape | `Space`        |
| **Decrease** Speed     | `<`            |
| **Increase** Speed     | `>`            |
| **Mute** Sounds        | `M`            |
| **Exit**               | `Esc`          |

**TODO**: 

	- Implement more renderers:
		-- Modernize current render process. Utilize batch rendering techniques. Figure out how to store data in SSBOs.
			--- There are 4 shape types. We can use 4 glDrawElementsInstanced calls. Use a different shader for the 2 first shapes, as one is semi-transparent and other is textured.
				---- Store Shape References of each shape in its corresponding shape vector. \/
				---- Code that uploads to SSBOs
				---- Write the shader that uses SSBOs to store all the uniform buffer variables in VRAM and how to upload them efficiently. Use gl_InstanceID to differentiate between objects. \/
					----- Figure out VRAM constraints. Calculate max object count. \/ 200 Bytes per shape. 400KB for 2000 shapes (more than the collision model can handle)
				---- Try Backface culling for objects other than index 1 (large semi-transparent cube).
		-- VulkanRenderer implements Renderer
		-- DX12Renderer implements Renderer
	- Modernize code
		-- Utilize modern OpenGL (Direct State Access)
		-- Utilize modern C++ features (RAII, coroutines?)
		-- Utilize dynamic link libraries instead of a monolithic exe
	- Fix InputController for Windows 11

DONE:

	- Abstract Renderer class
		-- OpenGLRenderer implements Renderer
	- SlangCompiler
		-- Find a way to have the same shader utilized for all solutions(SLang).
			--- Debug ShaderCompiler
			--- Utilize SpirV shaders in OpenGL. Compare SpirV results by compiling GLSL to SpirV and slang to SpirV. \/
	- Removed every shader reference from ApplicationController. All shader commands are in OpenGLShader
	- Removed uniform variables and replaced them with Uniform Buffers.	-	

