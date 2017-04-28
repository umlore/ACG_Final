#ifndef _GL_CANVAS_H_
#define _GL_CANVAS_H_

#define _USE_MATH_DEFINES 
#include <cmath>

// Graphics Library Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>

#include "boundingbox.h"

class ArgParser;
class Camera;

class Geometry;

// ====================================================================
// NOTE:  All the methods and variables of this class are static
// ====================================================================

class GLCanvas {

public:

  // various static variables
  static ArgParser *args;
  /* TODO (Andrew): We are going to have more than one geometry, and a bunch of lights. That should 
     probably be reflected here. */
  static Geometry *geometry;

  static BoundingBox bbox;
  static Camera* camera;
  static GLFWwindow* window;

  static GLuint ViewMatrixID;
  static GLuint ModelMatrixID;
	static GLuint ColorID;
  static GLuint MatrixID;
  static GLuint programID;

  static GLuint targetBuffer;
  static GLuint targetDepthBuffer;

	static GLuint albedoTargetTexture;
  static GLuint normalTargetTexture;
  static GLuint positionTargetTexture;

  static GLuint screenQuadData;
  static GLuint screenQuadVAO;
  static GLuint screenQuadShaderProgram;
  static GLuint screenQuadAlbedo;
  static GLuint screenQuadNormal;
	static GLuint screenQuadPosition;
  static GLuint screenQuadTexSize;

	static GLuint lightingProgram;
	static GLuint lightQuadAlbedo;
	static GLuint lightQuadNormal;
	static GLuint lightQuadPosition;
	static GLuint lightQuadTexSize;

	static GLuint lightBuffer;
	static GLuint lightDepthBuffer;
	static GLuint lightTargetTexture;
	static GLuint lightQuadMVP;
	static GLuint lightQuadM;

	static GLuint lightRadius;
	static GLuint lightColorin;
	static GLuint lightCamera;
	static GLuint lightCenter;

  // mouse position
  static int mouseX;
  static int mouseY;
  // which mouse button
  static bool leftMousePressed;
  static bool middleMousePressed;
  static bool rightMousePressed;
  // current state of modifier keys
  static bool shiftKeyPressed;
  static bool controlKeyPressed;
  static bool altKeyPressed;
  static bool superKeyPressed;

  static GLuint render_VAO;
  static GLuint lights_VAO;

  static void initialize(ArgParser *_args);
  static void initializeVBOs();
  static void setupVBOs();
  static void drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix);
  static void cleanupVBOs();
	static void drawPost();

	/* TEXTURE OUPUT AND FRAMEBUFFER STUFF*/
	static void CreateBufferTarget( GLuint *targetBuffer, GLuint *targetDepthBuffer);
	static void CreateAndBindTextureTarget(GLuint internalFormat, GLuint format, GLuint pixelData, GLuint frameBuffer, GLuint *renderTarget);

  static void animate();

  // Callback functions for mouse and keyboard events
  static void mousebuttonCB(GLFWwindow *window, int which_button, int action, int mods);
  static void mousemotionCB(GLFWwindow *window, double x, double y);
  static void keyboardCB(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void error_callback(int error, const char* description);
};

// ====================================================================

// helper functions
GLuint LoadShaders(const std::string &vertex_file_path,const std::string &fragment_file_path);
std::string WhichGLError(GLenum &error);
int HandleGLError(const std::string &message = "", bool ignore = false);

#endif
