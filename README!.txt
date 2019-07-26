Documentation Author: Niko Procopi 2019

This tutorial was designed for Visual Studio 2017 / 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Welcome to the Geometry Shader Particles Tutorial!
Prerequesites: 2D Particles

This tutorial is identical to the previous tutorial, except
the for the use of Instanced Rendering vs Geometry Shaders.
Geometry shaders are less efficient than Instanced Render,
but allow for more particles to be rendered to the screen.
This tutorial will draw 8192 particles.

In this tutorial, we process all particles on the CPU,
and then we make one draw for all of our partilces. Each
particle (sinlge point) is sent to the pipeline, all with 
one draw call, goes through vertex shader to get the screen 
position of the single point, then the geometry shader turns 
each point into a full quad, then fragment shader colors the 
pixels, then screen it goes to the screen.

To add a stage in our pipeline for the geometry shader, 
we need to add support for geometry shaders in our shaderProgram.cpp
file. In ShaderProgram::AttachShader, we add the ability
to link a shader, if the detected type is GL_GEOMETRY_SHADER.

In ParticleSystem.cpp, in the constructor, you can see that
we load our Vertex Shader with type GL_VERTEX_SHADER, then
we the Geometry Shader is loaded with GL_GEOEMTRY_SHADER,
and lastly the Fragmet Shader is loaded, just as usual.

In ParticleSystem::Draw(), we put all of our particles into
the Vertex Buffer. When we use glBufferData, we use GL_DYNAMIC_DRAW,
to let OpenGL know that we want to change this buffer often. The
reason we put all our particles into the vertex buffer, is because
the Vertex Shader will only be handling points, not geometry. As
stated earlier, the vertex shader handles one point for each particle,
and then the geometry shader turns each point into a quad.

We need to setup Vertex Attributes for every piece of the particle:
Position, Velocity, Color, rotation, angular-velocity, and age.
After that, we use GL_POINTS for our draw call:
	glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
	
After the Vertex Shader finishes with the points, it automatically
passes them to the geometry shader, because they are linked together
in the same Shader Program (shaderProgram.cpp), no code is written
to transfer the points from one shader to the other. After the Geometry
Shader finishes, then the polygons go to the rasterizer, then Fragment.

[Vertex Shader]
On line 29, we take in the cameraView matrix as a uniform
Lines 32 - 37: We pull all the particle data from this particular vertex
Lines 39 - 40: We create variables that will be passed to the geometry shader.
The position of each particle will automatically be accessible by the 
geometry shader because of gl_Position, so we don't need to manually pass
that to the geometry shader, but we do need to manually pass color, rotation,
and age. This is just like how we previously passed uv from Vertex Shader to 
Fragment Shader, except now we pass to Geometry Shader.

In the main() function, we multiply position by cameraView to get the screen-
position of the particle, then we set the output values (to geometry) to be
equal to the input that we got from the vertex buffer. If you want to 
adjust lines 48-51 to warp the values, feel free to experiment.

[Geometry Shader]
Lines 28-29, we take in two uniforms: one for particle size and one for viewport.

Next, we tell the geometry shader that it should take in a series of points from
the vertex shader. Geometry Shaders can take in points, triangles, or lines:
	layout(points) in;
	
We tell the geometry shader to take in an array of colors, rotations, and ages.
This array is a collection of all the values that it got from every vertex in the VS:
	in vec4 vertOutColor[];
	in float vertOutRotation[];
	in float vertOutAge[];
Notice the names of the variables have "out" in them, that is because the names
here need to match the names that were in the Vertex Shader. The input of the GS
is the output of the VS, so don't get confused. We are not outputting these
values to the rasterizer or fragment shader.

We tell the shader that the topology it should export is a triangle strip, and 
that it should export 4 vertices for every 1 vertex that it takes in. As discussed
in previous tutorials, 4 vertices in a triangle strip can make 1 quad (if done right).
	layout(triangle_strip, max_vertices = 4) out;
	
We tell the shader that we want to export UV and Color, we do this here,
instead of the vertex shader (where we did it before). They will be 
exported from GS, to rasterizer, to FS
	out vec2 uv;
	out vec4 color;
	
We calculate the color of the quad that we are about to generate:
	color = vertOutColor[0] * clamp(vertOutAge[0], 0, 1);
Notice the index is [0], this confsues a lot of beginners. Trust us,
it should be [0], that is not a mistake. Feel free to change it and experiment

We get the gl_Position that was exported by the Vertex Shader
	vec4 pos = gl_in[0].gl_Position;
	
We create variables c, s, scale, and T, exactly like the 
Vertex Shader in the last tutorial. After that, we make 4 vertices

We create a loop that will repeat 4 times.
UV is calcualted the same way,
offset is calculated the same way,
gl_position does not include cameraview, because
	"pos" in this case, is from the output of the
	vertex shader, which already multiplied the
	point "pos" by the cameraView matrix.
	
After that, we call the GLSL function EmitVertex()
to add a vertex to this triangle strip

After 4 vertices are emitted, we call EndPrimitive()
to finish this triangle strip (which is a quad).
This shader repeats until all quads are generated,
then all quads go to the rasterizer, and then 
the fragment shader.
	
[Fragment Shader]
Juat as easy as always, take a UV, a color, and a texture
to make each color of each pixel on each quad
	gl_FragColor = texture(tex, uv) * color; 

How to improve:
Use a compute shader to handle the physics of all particles
