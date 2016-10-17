/*
Title: Improved Particle System
File Name: main.cpp
Copyright � 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "../header/shader.h"
#include "../header/texture.h"
#include "../header/particleSystem.h"
#include "../header/fpsController.h"

glm::vec2 viewportDimensions = glm::vec2(800, 600);
glm::vec2 mousePosition;
ParticleSystem* particleSystem;


// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    viewportDimensions = glm::vec2(width, height);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}

// Change settings when keys are pressed
void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
    {
        particleSystem->m_acceleration.y += .25;
    }
    if (key == GLFW_KEY_F && action == GLFW_RELEASE)
    {
        particleSystem->m_acceleration.y -= .25;
    }
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
        particleSystem->m_particleSize += 50;
    }
    if (key == GLFW_KEY_G && action == GLFW_RELEASE)
    {
        particleSystem->m_particleSize -= 50;
    }
    if (key == GLFW_KEY_Y && action == GLFW_RELEASE)
    {
        particleSystem->m_startVelocityMin += .1f;
    }
    if (key == GLFW_KEY_H && action == GLFW_RELEASE)
    {
        particleSystem->m_startVelocityMin -= .1f;
    }
    if (key == GLFW_KEY_U && action == GLFW_RELEASE)
    {
        particleSystem->m_startVelocityRange += .1f;
    }
    if (key == GLFW_KEY_J && action == GLFW_RELEASE)
    {
        particleSystem->m_startVelocityRange -= .1f;
    }
    if (key == GLFW_KEY_I && action == GLFW_RELEASE)
    {
        particleSystem->m_spawnFrequency *= .5f;
    }
    if (key == GLFW_KEY_K && action == GLFW_RELEASE)
    {
        particleSystem->m_spawnFrequency *= 2.f;
    }
}

int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "Particles!", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback and mouse movement callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetKeyCallback(window, keyPressCallback);

	// Initializes the glew library
	glewInit();


    Texture* texture = new Texture("../assets/particle.png");
    texture->IncRefCount();

    // Initialize the particle system class with a bunch of parameters:
    particleSystem = new ParticleSystem(texture);
    particleSystem->m_position = glm::vec3(0, 0, -.5);
    particleSystem->m_spawnFrequency = .01;
    particleSystem->m_lifeTime = 2.0f;
    particleSystem->m_startVelocityMin = .1f;
    particleSystem->m_startVelocityRange = .1f;
    particleSystem->m_rotationRange = glm::vec2(0, 6.28);
    particleSystem->m_angularVelocityRange = glm::vec2(-1.f, 2.f);
    particleSystem->m_acceleration = glm::vec3(0, 0, 0);
    particleSystem->m_particleSize = glm::vec2(100, 100);
    particleSystem->m_color = glm::vec4(.9f, .3f, .1f, 1);


    std::cout << "Controls:" << std::endl;
    std::cout << "Use the mouse to look around, and wasd to move." << std::endl;
    std::cout << "R and F control gravity." << std::endl;
    std::cout << "T and G control particle size." << std::endl;
    std::cout << "Y and H control base particle velocity." << std::endl;
    std::cout << "U and J control particle velocity variance." << std::endl;
    std::cout << "I and K control particle spawn rate." << std::endl;
    std::cout << "Press escape to exit the demo." << std::endl;

    // Make a first person controller for the camera.
    FPSController controller = FPSController();


    float frames = 0;
    float secCounter = 0;

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Exit when escape is pressed.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        // Calculate delta time and frame rate
        float dt = glfwGetTime();
        frames++;
        secCounter += dt;
        if (secCounter > 1.f)
        {
            std::string title = "FPS: " + std::to_string(frames);
            glfwSetWindowTitle(window, title.c_str());
            secCounter = 0;
            frames = 0;
        }
        glfwSetTime(0);

        // Update the player controller.
        controller.Update(window, viewportDimensions, mousePosition, dt);

        // Update the particle simulation
        particleSystem->Update(dt);

        //////////////////////////
        // Start Drawing       //
        ////////////////////////

        // Calculate view-projection matrix.
        glm::mat4 viewMatrix = controller.GetTransform().GetInverseMatrix();
        glm::mat4 perspectiveProjection = glm::perspective(.75f, viewportDimensions.x / viewportDimensions.y, .1f, 100.f);
        glm::mat4 viewProjection = perspectiveProjection * viewMatrix;
        
        // The view projection matrix will be used in the vertex shader to move the particle.
        particleSystem->GetMaterial()->SetMatrix("cameraView", viewProjection);
        // The viewport dimensions are needed in the geometry shader to make a correctly sized quad.
        particleSystem->GetMaterial()->SetVec2("viewport", viewportDimensions);

		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 0.0);

        // Tell Particle System to draw.
        particleSystem->Draw();






		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}
    delete particleSystem;

    texture->DecRefCount();

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
