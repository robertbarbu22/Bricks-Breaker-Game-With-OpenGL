// Libraries
#include <iostream>         // Necessary library for console output
#include <windows.h>        // Using Windows system functions (creating windows, handling files and directories)
#include <stdlib.h>         // Libraries required for reading shaders
#include <stdio.h>
#include <GL/glew.h>        // Defines the prototypes of OpenGL functions and constants required for modern OpenGL programming
#include <GL/freeglut.h>    // Includes functions for:
							// - managing windows and keyboard/mouse events,
							// - drawing graphic primitives such as rectangles, circles, or lines,
							// - creating menus and submenus;
#include "loadShaders.h"    // The file that links the program with shaders
#include "glm/glm.hpp"      // Libraries used for graphic transformations
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL.h"
#include <vector>

float PI = 3.141592;
using namespace std;

// Identifiers for OpenGL objects
GLuint
VaoId,
VboId,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrScaleLocation,
matrTranslLocation,
matrRotlLocation,
texture,
codColLocation,
TextureBufferId;
// Dimensions of the display window
GLfloat
winWidth = 800, winHeight = 600;
// Variables for transformation matrices
glm::mat4
myMatrix, resizeMatrix, matrTransl, matrScale1, matrScale2, matrRot, matrDep, matrTranslCub1, matrTranslCub, myMatrix2;

// Variable determining the color change of pixels in the shader
int codCol, ct = 0;
// Variables for orthographic projection
float xMin = 0.f, xMax = 400.f, yMin = 0.f, yMax = 300.f, xMax1 = 400, yMax1 = -300;
// Variables for movement on the Ox axis and rotation
float i = 20.0, alpha = 0.0, step = 0.1, beta = 0.002, angle = 0.0, pi = 3.1415;
float mouseX = 0.0f;
float mouseY = 0.0f;

bool isBallBlocked = false;
bool fail = false;
bool win = false;

// Variables for the position and speed of the ball
glm::vec2 ballPosition(150.0f, 150.0f);
glm::vec2 ballVelocity(2.0f, 3.5f); // Speed on both axes


struct Brick {
	glm::mat4 positionMatrix;
	bool isHit = false;
	GLfloat xmax, xmin, ymax, ymin;
};

std::vector<Brick> bricks;

void LoadTexture(const char* photoPath)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Unwrapping the image horizontally/vertically based on texture parameters;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Function to display the transformation matrix
void DisplayMatrix()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			cout << myMatrix[i][j] << "  ";
		cout << endl;
	};
	cout << "\n";
};


void UpdateBallPosition()
{
	if (isBallBlocked) {
		// The ball is already blocked, do nothing
		return;
	}

	//Updating the ball position
	ballPosition.x += ballVelocity.x;
	ballPosition.y += ballVelocity.y;

	//Checking the collision with the walls
	if (ballPosition.x + 26.25f * ballVelocity.x < xMin || ballPosition.x - 45.f  > xMax) {
		// If it touches the left or right limits, we reverse x
		ballVelocity.x = -ballVelocity.x;
	}
	if (ballPosition.y - 45.f > yMax) {
		// If it touches the upper limit, we reverse y
		ballVelocity.y = -ballVelocity.y;
	}

	if (ballPosition.y + 12 * ballVelocity.y <= yMin) {

		//the ball is out of the screen and the game is over
		isBallBlocked = true;
		fail = true;
		ballPosition.x = -300;
		ballPosition.y = 50;
		yMax = -20;
		yMin = yMax1;

		resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);


	}

	//Checking the collision with the mouse controlled square
	float squareLeft = mouseX + 10.0f;
	float squareRight = mouseX + 90.0f;
	float squareTop = mouseY + 50.0f;
	float squareBottom = mouseY;

	if (ct % 2 == 1)
	{
		squareRight += 30.0f;
	}

	if (ballPosition.x > squareLeft + 30.0f && ballPosition.x < squareRight &&
		ballPosition.y <= squareTop && ballPosition.y >= squareBottom) {
		// if the ball touches the square, we reverse the directions
		ballVelocity.x = -ballVelocity.x;
		ballVelocity.y = -ballVelocity.y;
	}

	else if (ballPosition.x > squareLeft && ballPosition.x < squareRight - 30.0f &&
		ballPosition.y <= squareTop && ballPosition.y >= squareBottom) {
		// if the ball touches the square, we reverse the directions	
		ballVelocity.x = ballVelocity.x;
		ballVelocity.y = -ballVelocity.y;
	}

	//Checking the collision with the bricks

	for (i = 0; i < bricks.size(); i++)
	{

		if (ballPosition.x >= bricks[i].xmin + 45.0f && ballPosition.x <= bricks[i].xmax + 45.0f &&
			ballPosition.y <= bricks[i].ymax + 45.0f && ballPosition.y >= bricks[i].ymin + 45.0f && bricks[i].isHit == false)
		{
			bricks[i].isHit = true;
			ct++;
			ballVelocity.y = -ballVelocity.y;
		}
	}

	if (ct == bricks.size() && win == false)
	{
		xMax = -300;
		win = true;
		isBallBlocked = true;

		resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	}


}


void Update(int value)
{
	UpdateBallPosition(); // Update ball position

	// Redraw the window
	glutPostRedisplay();

	// Call the function at a certain interval
	glutTimerFunc(20, Update, 0);
}

void TrackMouse(int x, int y)
{
	// Calculate the X coordinate of the cursor
	mouseX = (x / winWidth) * (xMax - xMin) + xMin;
}



// Create and compile shader objects;
// They must be in the same directory as the current project;
// Vertex shader - affects the scene's geometry;
// Fragment shader - affects the color of pixels;
void CreateShaders(void)
{
	ProgramId = LoadShaders("Shader.vert", "Shader.frag");
	glUseProgram(ProgramId);
}

// Initialize a Vertex Buffer Object (VBO) for transferring data to the graphics card memory (to shaders);
// It stores data about vertices (coordinates, colors, indices, texturing, etc.);
void CreateVBO(void)
{
	// Coordinates of the vertices;
	GLfloat Vertices[] = {
		// Vertices for axes;
			-450.0f, 0.0f, 0.0f, 1.0f,
			450.0f,  0.0f, 0.0f, 1.0f,
			0.0f, -300.0f, 0.0f, 1.0f,
			0.0f, 300.0f, 0.0f, 1.0f,

			// Vertices for background

			0.0f,  0.0f, 0.0f, 1.0f,
			0.0f,  400.0f, 0.0f, 1.0f,
			400.0f,  400.0f, 0.0f, 1.0f,
			400.0f,  0.0f, 0.0f, 1.0f,


			// Vertices for game over
			0.0f, -300.0f, 0.0f, 1.0f,
			0.0f, -20.0f, 0.0f, 1.0f,
			400.0f, -20.0f, 0.0f, 1.0f,
			440.0f, -300.0f, 0.0f, 1.0f,


			// Vertices for game over emoji

			180.0f, -130.0f, 0.0f, 1.0f,
			220.0f, -130.0f, 0.0f, 1.0f,
			160.0f, -180.0f, 0.0f, 1.0f,
			180.0f, -160.0f, 0.0f, 1.0f,
			220.0f, -160.0f, 0.0f, 1.0f,
			240.0f, -180.0f, 0.0f, 1.0f,

			// Vertices for win
			-400.0f, 0.0f, 0.0f, 1.0f,
			-400.0f, 400.0f, 0.0f, 1.0f,
			0.0f, 400.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f,

			// Vertices for win emoji

			-130.0f, 180.0f, 0.0f, 1.0f,
			-170.0f, 180.0f, 0.0f, 1.0f,
			-110.0f, 160.0f, 0.0f, 1.0f,
			-130.0f, 130.0f, 0.0f, 1.0f,
			-170.0f, 130.0f, 0.0f, 1.0f,
			-190.0f, 160.0f, 0.0f, 1.0f,

			// Vertices for brick 
			-10.0f,  -10.0f, 0.0f, 1.0f,
			10.0f, -10.0f, 0.0f, 1.0f,
			10.0f,  10.0f, 0.0f, 1.0f,
			-10.0f,  10.0f, 0.0f, 1.0f,

			// Vertices for ball
			5 * cos(0 * 2 * pi / 16), 5 * sin(0 * 2 * pi / 16), 0.0f, 1.0f,	//	0
			5 * cos(1 * 2 * pi / 16), 5 * sin(1 * 2 * pi / 16), 0.0f, 1.0f,	//	1
			5 * cos(2 * 2 * pi / 16), 5 * sin(2 * 2 * pi / 16), 0.0f, 1.0f,	//	2
			5 * cos(3 * 2 * pi / 16), 5 * sin(3 * 2 * pi / 16), 0.0f, 1.0f,	//	3
			5 * cos(4 * 2 * pi / 16), 5 * sin(4 * 2 * pi / 16), 0.0f, 1.0f,	//	4
			5 * cos(5 * 2 * pi / 16), 5 * sin(5 * 2 * pi / 16), 0.0f, 1.0f,	//	5
			5 * cos(6 * 2 * pi / 16), 5 * sin(6 * 2 * pi / 16), 0.0f, 1.0f,	//	6
			5 * cos(7 * 2 * pi / 16), 5 * sin(7 * 2 * pi / 16), 0.0f, 1.0f,	//	7
			5 * cos(8 * 2 * pi / 16), 5 * sin(8 * 2 * pi / 16), 0.0f, 1.0f,	//	8
			5 * cos(9 * 2 * pi / 16), 5 * sin(9 * 2 * pi / 16), 0.0f, 1.0f,	//	9
			5 * cos(10 * 2 * pi / 16), 5 * sin(10 * 2 * pi / 16), 0.0f, 1.0f,	//	10
			5 * cos(11 * 2 * pi / 16), 5 * sin(11 * 2 * pi / 16), 0.0f, 1.0f,	//	11
			5 * cos(12 * 2 * pi / 16), 5 * sin(12 * 2 * pi / 16), 0.0f, 1.0f,	//	12
			5 * cos(13 * 2 * pi / 16), 5 * sin(13 * 2 * pi / 16), 0.0f, 1.0f,	//	13
			5 * cos(14 * 2 * pi / 16), 5 * sin(14 * 2 * pi / 16), 0.0f, 1.0f,	//	14
			5 * cos(15 * 2 * pi / 16), 5 * sin(15 * 2 * pi / 16), 0.0f, 1.0f,	//	15



			// Vertices for mouse controlled paddle
			0.0f,  0.0f, 0.0f, 1.0f,
			200.0f, 0.0f, 0.0f, 1.0f,
			200.0f,  100.0f, 0.0f, 1.0f,
			0.0f,  100.0f, 0.0f, 1.0f,



	};

	// Colors of the axes;
	GLfloat Colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
	};


	GLfloat Textures[] = {
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	// Transmitting data through buffers;

	// Creating/binding a Vertex Array Object (VAO) - useful when using multiple VBOs;
	glGenVertexArrays(1, &VaoId); // Generation of VAO and indexing it to the variable VaoId;
	glBindVertexArray(VaoId);

	// Creating a buffer for VERTICES;
	glGenBuffers(1, &VboId); // Generation of the buffer and indexing it to the variable VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId); // Setting the buffer type - vertex attributes;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // Points are "copied" into the current buffer;
	// Associating the attribute (0 = coordinates) for the shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Creating a buffer for COLORS;
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// Associating the attribute (1 = color) for the shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &TextureBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, TextureBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Textures), Textures, GL_STATIC_DRAW);
	// Associating the attribute (2 = color) for the shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
}


// Delete shader objects after running;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

// Delete VBO objects after running;
void DestroyVBO(void)
{
	// Release attributes from shaders (position, color, texturing, etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);

	// Deleting buffers for vertices, colors;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &TextureBufferId);
	glDeleteBuffers(1, &VboId);

	// Release VAO objects;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

// Function to release allocated resources;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}


// Setting up the parameters necessary for the visualization window;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);      // Background color of the screen;
	CreateVBO();                               // Transfer rendering data to the buffer used by shaders;
	CreateShaders();                           // Initializing shaders;
	// Instantiating uniform variables to "communicate" with shaders;
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	glutPassiveMotionFunc(TrackMouse);
	glutTimerFunc(20, Update, 0);
}

int nrRenderings = 1;
// Function for drawing graphics on the screen;

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);            // Clear the OpenGL screen to draw the new content;

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);                            // "Bring" the scene to the "standard square" [-1,1]x[-1,1];
	// Translation matrix (along the X axis);

	// Drawing the axes;

	// Resizing matrix (for "fixed" elements - axes);
	myMatrix = resizeMatrix;
	// Color;
	codCol = 0;
	// Transmitting uniform variables for TRANSFORMATION MATRIX and COLOR to shaders;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	// The drawing function takes 3 arguments:
	// - arg1 = type of the drawn primitive,
	// - arg2 = index of the first point to be drawn from the buffer,
	// - arg3 = number of consecutive points to be drawn;

	// Draw the axes;
	glDrawArrays(GL_LINES, 0, 4);


	/*LoadTexture("c.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);*/

	//draw background

	codCol = 6;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4);



	/*LoadTexture("d.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);*/

	//draw game over background

	//codCol= 4;
	codCol = 5;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 8, 4);

	//draw game over emoji
	glLineWidth(10);
	glPointSize(10);
	codCol = 0;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POINTS, 12, 2);

	glDrawArrays(GL_LINES, 14, 2);
	glDrawArrays(GL_LINES, 15, 2);
	glDrawArrays(GL_LINES, 16, 2);

	/*LoadTexture("d.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);*/


	//draw win background

	//codCol = 4;
	codCol = 1;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 18, 4);

	// win emoji
	glLineWidth(10);
	glPointSize(10);
	codCol = 0;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POINTS, 22, 2);

	glDrawArrays(GL_LINES, 24, 2);
	glDrawArrays(GL_LINES, 25, 2);
	glDrawArrays(GL_LINES, 26, 2);


	//draw bricks

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	codCol = 2;
	matrTranslCub = glm::translate(glm::vec3(25.0f, 0.0f, 0.0f));
	//translation matrix to move the bricks down
	matrTranslCub1 = glm::translate(matrTranslCub, glm::vec3(0.0f, -25.0f, 0.0f));

	glUniform1i(codColLocation, codCol);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(90.0f, 200.0f, 0.0f));
	myMatrix2 = resizeMatrix * model;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix2[0][0]);

	GLfloat angle = PI / 16;
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));

	//glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void*)(0));

	GLfloat xmaxinit = 10.0f + 90.0f, xmininit = -10.0f + 90.0f, ymaxinit = 10.0f + 200.0f, ymininit = -10.0f + 200.0f;
	if (nrRenderings == 1)
		for (int row = 0; row < 4; row++)
		{

			for (int col = 0; col < 10; col++)
			{
				if (col != 0)
				{
					myMatrix2 = myMatrix2 * matrTranslCub;
				}
				//add values to the bricks vector
				Brick brick;
				brick.positionMatrix = myMatrix2;
				brick.xmax = xmaxinit;
				brick.xmin = xmininit;
				brick.ymax = ymaxinit;
				brick.ymin = ymininit;
				xmaxinit += 25.0f;
				xmininit += 25.0f;
				bricks.push_back(brick);

				glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix2[0][0]);
				glDrawArrays(GL_TRIANGLE_FAN, 28, 4);

			}
			//translate the bricks to the next row
			myMatrix2 = myMatrix2 * glm::translate(glm::vec3(-250.0f, 0.0f, 0.0f));//translate with (col+1)*15
			myMatrix2 = myMatrix2 * matrTranslCub1;
			ymaxinit -= 25.0f;
			ymininit -= 25.0f;
			xmaxinit -= 250.0f;
			xmininit -= 250.0f;
		}
	nrRenderings++;
	int ok = 1;
	if (nrRenderings > 2 and fail == false)
	{
		ok = 0;
		for (int i = 0; i < bricks.size(); i++)
		{
			if (i % 2 == 1)
			{
				if (bricks[i].isHit == false)
				{
					bricks[i].positionMatrix = bricks[i].positionMatrix * matrRot;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &bricks[i].positionMatrix[0][0]);
					glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
					ok = 1;
				}
			}
			else
			{
				if (bricks[i].isHit == false)
				{
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &bricks[i].positionMatrix[0][0]);
					glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
					ok = 1;
				}
			}

		}
	}
	if (ok == 0)
	{
		resizeMatrix = glm::ortho(-400, 0, 0, 300);
	}
	int c = bricks.size();

	// Drawing primitives;
	 // The glDrawElements function takes 4 arguments:
	 // - arg1 = drawing mode;
	 // - arg2 = number of vertices;
	 // - arg3 = type of index data;
	 // - arg4 = pointer to indices (EBO): the starting position of the indices;

	 // Draw the ball;

	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(ballPosition.x, ballPosition.y, 0.0f));
	myMatrix = resizeMatrix * matrTransl * glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -50.0f, 0.0f));


	codCol = 3;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 16);

	//LoadTexture("lidl.png");
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	//draw the paddle

	if (ct % 2 == 0)
	{
		matrScale2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.3, 0.2, 0.0));		//Scaling the paddle

		matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(mouseX, mouseY, 0.0f));
		myMatrix = resizeMatrix * matrTransl * matrScale2 * glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -50.0f, 0.0f));

		codCol = 1;

		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glUniform1i(codColLocation, codCol);
		glDrawArrays(GL_POLYGON, 48, 4);
	}
	else
	{
		matrScale2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.2, 0.0));		//	scale the paddle

		matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(mouseX, mouseY, 0.0f));
		myMatrix = resizeMatrix * matrTransl * matrScale2 * glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -50.0f, 0.0f));

		codCol = 1;

		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glUniform1i(codColLocation, codCol);
		glDrawArrays(GL_POLYGON, 48, 4);
	}



	glutSwapBuffers();	// Replace the drawn image in the window with the rendered one;
	glFlush();  // Ensure the execution of all previously called OpenGL commands;
}

// Entry point into the program, OpenGL routine is executed;
int main(int argc, char* argv[])
{
	// Initialize GLUT and the OpenGL context, configure the window and display mode;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);      // Use 2 buffers (one for display and one for rendering => smooth animations) and RGB colors;
	glutInitWindowPosition(100, 100);                 // Initial position of the window;
	glutInitWindowSize(winWidth, winHeight);          // Window size;
	glutCreateWindow("Ball break game");              // Create the viewing window, specifying its name;

	// Initialize GLEW and check for the availability of modern OpenGL extensions on the host system;
	// Must be initialized before drawing;

	glewInit();

	Initialize();                                   // Set up the necessary parameters for the viewing window;

	glutDisplayFunc(RenderFunction);                // Draw the scene in the window;
	glutMotionFunc(TrackMouse);                      // Activate mouse tracking;

	glutCloseFunc(Cleanup);                          // Release resources allocated by the program;

	// The main loop for processing GLUT events (functions starting with glut: glutInit, etc.) is started;
	// Handles events and draws the OpenGL window until the user closes it;

	glutMainLoop();

	return 0;
}
