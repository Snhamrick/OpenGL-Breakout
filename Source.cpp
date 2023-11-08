#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;

void processInput(GLFWwindow* window);
int getRandomDirection();

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	BRICKTYPE brick_type;
	ONOFF onoff;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.003;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left
	int count;																										//Variable for circle color change
	int brickCount;																									//Variable to count brick hits

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
		count = 0;
		brickCount = 0;
	}


	void CheckCollision(Brick* brk)
	{

		if (brk->brick_type == REFLECTIVE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				direction = GetNewDirection(direction);																										//Get new direction for circle
				x = x;
				y = y;
				brickCount += 1;																				
				if (brickCount % 3 == 0) {																		//Cyan
					brk->red = 0;
					brk->green = 1;
					brk->blue = 0.8;
				}
				else {																							//Red
					brk->red = 1; 
					brk->green = 0;
					brk->blue = 0;
				}
			}
		}
		else if (brk->brick_type == DESTRUCTABLE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				brickCount += 1;																															//Update the brick has been hit
				if (brickCount > 0) {																														//Change color if first hit (white)
					brk->red = 1;
					brk->green = 1;
					brk->blue = 1;
				}
				if (brickCount > 1) {																														//Turn brick off after second hit
					brk->onoff = OFF;
				}
			}
		}
	}
		
	bool CheckCircles(Circle circle) {																														//Function to check for circle collisions
		if ((x > circle.x - circle.radius && x <= circle.x + circle.radius) && (y > circle.y - circle.radius && y <= circle.y + circle.radius)) {
			count += 1;
			return true;																																	//returns true if collision
		}
		return false;																																		//returns false no collision
	}



	int GetNewDirection(int currentDirection) {																												//Function to find new direction

		/*Directions opposite from current*/
		if (currentDirection == 1) {																														
			return 3;
		}
		else if (currentDirection == 2) {
			return 4;
		}
		else if (currentDirection == 3) {
			return 1;
		}
		else if (currentDirection == 4) {
			return 2;
		}
		else if (currentDirection == 5) {
			return 8; 
		}
		else if (currentDirection == 6) {
			return 7;
		}
		else if (currentDirection == 7) {
			return 6;
		}
		else if (currentDirection == 8) {
			return 5;
		}

	}

	void ChangeColor() {																															//Function to change circle color
		if (count % 2 == 1) {																					//purple
			red = 1;
			green = 0;
			blue == 0.8;
		}
		else {																									//white
			red = 1;
			green = 1;
			blue = 1;
		}
	}



	void MoveOneStep()
	{
		if (direction == 1 || direction == 5 || direction == 6)  // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = GetNewDirection(direction);
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)  // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = GetNewDirection(direction);
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)  // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = GetNewDirection(direction);
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)  // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = GetNewDirection(direction);
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world;


int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "Random World of Circles", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Top Row
	Brick brick(REFLECTIVE, 0, 0.6, 0.2, 1, 0, 0);
	Brick brick2(DESTRUCTABLE, -0.2, 0.6, 0.2, 0, 0, 1);
	Brick brick3(DESTRUCTABLE, 0.2, 0.6, 0.2, 0, 0, 1);
	Brick brick4(REFLECTIVE, -0.4, 0.6, 0.2, 1, 0, 0);
	Brick brick5(REFLECTIVE, 0.4, 0.6, 0.2, 1, 0, 0);
	//Left Side
	Brick brick6(DESTRUCTABLE, -0.4, 0.4, 0.2, 0, 0, 1);
	Brick brick7(REFLECTIVE, -0.4, 0.2, 0.2, 1, 0, 0);
	Brick brick15(DESTRUCTABLE, -0.4, 0.0, 0.2, 0, 0, 1);
	Brick brick16(REFLECTIVE, -0.4, -0.2, 0.2, 1, 0, 0);
	Brick brick19(DESTRUCTABLE, -0.4, -0.4, 0.2, 0, 0, 1);
	//Bottom Row
	Brick brick10(REFLECTIVE, 0, -0.6, 0.2, 1, 0, 0);
	Brick brick11(DESTRUCTABLE, -0.2, -0.6, 0.2, 0, 0, 1);
	Brick brick12(DESTRUCTABLE, 0.2, -0.6, 0.2, 0, 0, 1);
	Brick brick13(REFLECTIVE, -0.4, -0.6, 0.2, 1, 0, 0);
	Brick brick14(REFLECTIVE, 0.4, -0.6, 0.2, 1, 0, 0);
	//RightSide
	Brick brick17(DESTRUCTABLE, 0.4, 0.4, 0.2, 0, 0, 1);
	Brick brick18(REFLECTIVE, 0.4, 0.2, 0.2, 1, 0, 0);
	Brick brick8(DESTRUCTABLE, 0.4, 0.0, 0.2, 0, 0, 1);
	Brick brick9(REFLECTIVE, 0.4, -0.2, 0.2, 1, 0, 0);
	Brick brick20(DESTRUCTABLE, 0.4, -0.4, 0.2, 0, 0, 1);
	

	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		int counter = 0;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);
		
		//Movement
		for (int i = 0; i < world.size(); i++)  {
			for (int j = 0; j < world.size(); j++) {
				if (world[i].CheckCircles(world[j]) == true) {
					world[i].ChangeColor();
					world[j].ChangeColor();
				}
			}
			world[i].CheckCollision(&brick);
			world[i].CheckCollision(&brick2);
			world[i].CheckCollision(&brick3);
			world[i].CheckCollision(&brick4);
			world[i].CheckCollision(&brick5);
			world[i].CheckCollision(&brick6);
			world[i].CheckCollision(&brick7);
			world[i].CheckCollision(&brick8);
			world[i].CheckCollision(&brick9);
			world[i].CheckCollision(&brick10);
			world[i].CheckCollision(&brick11);
			world[i].CheckCollision(&brick12);
			world[i].CheckCollision(&brick13);
			world[i].CheckCollision(&brick14);
			world[i].CheckCollision(&brick15);
			world[i].CheckCollision(&brick16);
			world[i].CheckCollision(&brick17);
			world[i].CheckCollision(&brick18);
			world[i].CheckCollision(&brick19);
			world[i].CheckCollision(&brick20);
			world[i].MoveOneStep();
			world[i].DrawCircle();

		}

		brick.drawBrick();
		brick2.drawBrick();
		brick3.drawBrick();
		brick4.drawBrick();
		brick5.drawBrick();
		brick6.drawBrick();
		brick7.drawBrick();
		brick8.drawBrick(); 
		brick9.drawBrick();
		brick10.drawBrick();
		brick11.drawBrick();
		brick12.drawBrick();
		brick13.drawBrick();
		brick14.drawBrick();
		brick15.drawBrick();
		brick16.drawBrick();
		brick17.drawBrick();
		brick18.drawBrick();
		brick19.drawBrick();
		brick20.drawBrick();
		counter = counter + 1;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{


		Circle A(-0.2, -1, 0.2, getRandomDirection(), 0.05, 1, 1, 1);															//Circles generate from each side of window when pressed
		Circle B(0.2, 1, 0.2, getRandomDirection(), 0.05, 1, 1, 1);
		Circle C(-1, 0.1, 0.2, getRandomDirection(), 0.05, 1, 1, 1);
		Circle D(1, -0.1, 0.2, getRandomDirection(), 0.05, 1, 1, 1);
		Circle E(0, 0, 0.2, getRandomDirection(), 0.05, 1, 1, 1);
		world.push_back(A);
		world.push_back(B);
		world.push_back(C);
		world.push_back(D);
		
	}
}

int getRandomDirection()
{
	return (rand() % 8) + 1;
}