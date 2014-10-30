#include "Tutorial2.h"
#include "Utilities.h"
#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial2::Tutorial2()
{

}

Tutorial2::~Tutorial2()
{

}

bool Tutorial2::onCreate(int a_argc, char* a_argv[])
{

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// The values for the camera and projection matrices had to be changed from the original tutorial example (the values they supplied out of the box).
	// I have no idea how the #$%#@*&$* they got anything to render with the ones they provided cause I'm bad at matrix math.
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));		// Also this was originally set to glm::inverse(glm::lookat(...)) , taking away the inverse
	// ...eventually allowed me to render something to the screen.
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// Load in our shader program
	m_ShaderProgram = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

	GenerateGrid(4, 3);

	// set the clear colour (gray) and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);																		//If you used their camera setup, the camera was actually facing the backs of shapes, so
	//... all 2D shapes were invisible cause their backs (or from our perspective, the "fronts") were 
	//... were being culled.

	return true;
}

void Tutorial2::onUpdate(float a_deltaTime)
{
	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Tutorial2::onDraw()
{

	/*
	Documentation Links:
	http://docs.gl/gl4/glUseProgram
	http://docs.gl/gl4/glUniform
	http://docs.gl/gl4/glPolygonMode
	http://docs.gl/gl4/glDrawElements
	*/

	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_ShaderProgram);										// Runs our compiled shader program once every frame.

	// uniforms for use in the shader
	GLuint projectionViewUniform = glGetUniformLocation(m_ShaderProgram, "ProjectionView");
	GLuint timeUniform = glGetUniformLocation(m_ShaderProgram, "Time");

	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	glUniform1f(timeUniform, Utility::getTotalTime());
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_projectionMatrix * viewMatrix));	// In the original example they forgot to multiply the Projection Matrix
	// ... by the view matrix.
	glBindVertexArray(m_VAO);											// Must be done once per frame in OpenGL 4.0 from what I've read.

	int rows = 4;														// This is the part of the tutorial that really messed up Ian & I.
	int cols = 3;														// They reference the rows and cols values from GenerateGrid(...) , but the values are needed
	int indexCount = (rows - 1) * (cols - 1) * 6;						// ... inside the draw call and not the GenerateGrid (aka mesh initialization) function.

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);							// This enables wireframe rendering.
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);		// Actual draw call. 

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
}

void Tutorial2::onDestroy()
{
	// cleanup
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
	glDeleteProgram(m_ShaderProgram);
	glDeleteVertexArrays(1, &m_VAO);									// fairly certain this needs to be called last.
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial2();

	if (app->create("AIE - Tutorial2", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, argc, argv) == true)
		app->run();

	// explicitly control the destruction of our application
	delete app;

	return 0;
}

//Generate grid function from the tutorial.

void Tutorial2::GenerateGrid(unsigned int rows, unsigned int cols)
{
	/* VERTEX ARRAY */

	/*
	Documentation Links:

	http://docs.gl/gl4/glGenBuffers
	http://docs.gl/gl4/glBindBuffer
	http://docs.gl/gl4/glBufferData

	http://docs.gl/gl4/glGenVertexArrays
	http://docs.gl/gl4/glBindVertexArray
	http://docs.gl/gl4/glEnableVertexAttribArray
	http://docs.gl/gl4/glVertexAttribPointer
	*/

	//Consider reading up on Vertex Attribute Objects, they are different in use for OpenGL 4.0 than in in previous versions.
	//tl:dr - VAO's are the new meta for OpenGL for, you HAVE to have at least one, and with proper setup you only need to re-bind the VAO once per frame
	//		... instead of calling a bunch of other stuff.

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	/* VERTEX BUFFER */
	VertexBasic * aoVertices = new VertexBasic[rows * cols];		//Array of vertexes (which contain a vec4 for pos and color)

	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);				// I had to change this part as well, my camera knowledge is weak		
			// ... so I had it render to XY instead of XZ
			glm::vec3 v3colour = glm::vec3(sinf((c / (float)(cols - 1)) *  (r / (float)(rows - 1))));	// Creating a glm::vec3(...) with ONE variable as the argument sets XYZ to that value.
			// Thus, this loop is going to initialize each vertices color to a grayscale value between solid
			// ...white and solid black, as XYZ will always be the same value between 0 and 1.
			aoVertices[r * cols + c].colour = glm::vec4(v3colour, 1.0f);								// This transforms the vec3 into a vec4, since w(or alpha, in this case) is always 1, the color will always
			// ... be opaque.
		}

	}
	//STEP 1: GENERATE BUFFER
	glGenBuffers(1, &m_VBO);								// Param 1: no. of array object names - how many object names are to be generated
	// Param 2: buffers - an array in which the buffer object names are stored.	

	//STEP 2: BIND THE BUFFER TO SOME BINDING POINT
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);					// Param 1: target - for now we will just be using GL_ARRAY_BUFFER, 
	//       ...this accepts a GLenum (there are tons of obscure specific GLenums, see documentation link)
	// Param 2: buffer - the name of the buffer object we're binding to.

	//STEP 3: CREATE SPACE FOR & COPY DATA FROM THE VERTICES ARRAY WE CREATED EARLIER.
	glBufferData(GL_ARRAY_BUFFER,						// Param 1: target - " "
		(rows * cols) * sizeof(VertexBasic),	// Param 2: size - how many bytes of data the buffer object is going to be storing
		aoVertices,								// Param 3: data - the data that's going to be copied in, NULL if not data is to be copied. In this case we're copying our vertices.
		GL_STATIC_DRAW);						// Param 4: usage - this a hint to OpenGL for optimization purposes. I've read that these are actually often ignored.
	//				... GL_STATIC_DRAW, GL_DYNAMIC_DRAW, and GL_STREAM_DRAW are the most common.

	delete[] aoVertices;					// The vertices have been stuck into our vertex buffer, so we no longer need the "normal" data.

	/* INDEX BUFFER */

	// This section follows the Vertex Buffer section EXACTLY THE SAME (more or less) except for GL_ELEMENT_ARRAY_BUFFER replaces GL_ARRAY_BUFFER
	// create and bind buffers to an index buffer object
	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];	//We're creating an array of indices to control the order in which our vertices are drawn
	//We will need 2 triangles per quad in the grid. 1 Triangle = 3 Vertices. 3 * 2 = 6.
	//Remeber array indices start at 0, so we need to subtract 1 from the row and column values passed into the create
	//grid function.
	//So if we had a grid with 4 rows and 3 columns...
	// (4 - 1) * ( 3 - 1) * 6 = 36
	//TO DO: Go over the mathematical patterns for this later.
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = r * cols + c;

			// triangle 2
			auiIndices[index++] = r * cols + (c + 1);
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + c;
		}
	}

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	/* VERTEX ATTRIBUTES */

	//Position
	glEnableVertexAttribArray(0);												// You must enable an attribute for it to be used. Attributes are disabled by default. If they are disabled OpenGL 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBasic), 0);	//... sends a default value, not an error. (Meaning something will likely just not render).

	//Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBasic), ((char*)0) + 16);

	delete[] auiIndices;

	// Binding these to 0 sets them back to their default state, unsure still on whether this necessary, or has any positive or negative effect on rendering, some tutorials do this and others do not.
	// ... still need to do some research.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}