#include <cstdlib>
#include "glCanvas.h"
#include "argparser.h"
#include "camera.h"

#if _WIN32
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

#include "geometry.h"
#include "utils.h"

// ========================================================
// static variables of GLCanvas class

ArgParser* GLCanvas::args = NULL;
Camera* GLCanvas::camera = NULL;
Geometry* GLCanvas::geometry = NULL;
BoundingBox GLCanvas::bbox;
GLFWwindow* GLCanvas::window = NULL;

// mouse position
int GLCanvas::mouseX = 0;
int GLCanvas::mouseY = 0;
// which mouse button
bool GLCanvas::leftMousePressed = false;
bool GLCanvas::middleMousePressed = false;
bool GLCanvas::rightMousePressed = false;
// current state of modifier keys
bool GLCanvas::shiftKeyPressed = false;
bool GLCanvas::controlKeyPressed = false;
bool GLCanvas::altKeyPressed = false;
bool GLCanvas::superKeyPressed = false;

GLuint GLCanvas::render_VAO;

GLuint GLCanvas::ViewMatrixID;
GLuint GLCanvas::ModelMatrixID;
GLuint GLCanvas::MatrixID;
GLuint GLCanvas::programID;
GLuint GLCanvas::ColorID;

GLuint GLCanvas::targetBuffer;
GLuint GLCanvas::targetDepthBuffer;

GLuint GLCanvas::albedoTargetTexture;
GLuint GLCanvas::normalTargetTexture;
GLuint GLCanvas::positionTargetTexture;

GLuint GLCanvas::screenQuadData;
GLuint GLCanvas::screenQuadVAO;
GLuint GLCanvas::screenQuadShaderProgram;
GLuint GLCanvas::screenQuadTexSize;

GLuint GLCanvas::screenQuadAlbedo;
GLuint GLCanvas::screenQuadNormal;
GLuint GLCanvas::screenQuadPosition;

GLuint GLCanvas::lightingProgram;
GLuint GLCanvas::lightQuadAlbedo;
GLuint GLCanvas::lightQuadNormal;
GLuint GLCanvas::lightQuadPosition;
GLuint GLCanvas::lightQuadTexSize;
GLuint GLCanvas::lightQuadMVP;
GLuint GLCanvas::lightQuadM;

GLuint GLCanvas::lightRadius;
GLuint GLCanvas::lightColorin;

GLuint GLCanvas::lightBuffer;
GLuint GLCanvas::lightTargetTexture;
GLuint GLCanvas::lightDepthBuffer;


// ========================================================
// Initialize all appropriate OpenGL variables, set
// callback functions, and start the main event loop.
// This function will not return but can be terminated
// by calling 'exit(0)'
// ========================================================

void GLCanvas::initialize(ArgParser *_args) {

  args = _args;
  geometry = new Geometry(args);
  geometry->Load();
  bbox.Set(geometry->getBoundingBox());

  glfwSetErrorCallback(error_callback);

  // Initialize GLFW
  if( !glfwInit() ) {
    std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
    exit(1);
  }
  
  // We will ask it to specifically open an OpenGL 3.2 context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFW window
  window = glfwCreateWindow(args->width,args->height, "OpenGL viewer", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Failed to open GLFW window" << std::endl;
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  HandleGLError("in glcanvas first");

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
    glfwTerminate();
    exit(1);
  }

  // there seems to be a "GL_INVALID_ENUM" error in glewInit that is a
  // know issue, but can safely be ignored
  HandleGLError("after glewInit()",true);

  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "OpenGL Version: " << (char*)glGetString(GL_VERSION) << '\n';
  std::cout << "-------------------------------------------------------" << std::endl;

  // Initialize callback functions
  glfwSetCursorPosCallback(GLCanvas::window,GLCanvas::mousemotionCB);
  glfwSetMouseButtonCallback(GLCanvas::window,GLCanvas::mousebuttonCB);
  glfwSetKeyCallback(GLCanvas::window,GLCanvas::keyboardCB);

  programID = LoadShaders( args->path+"/"+args->shader_filename+".vs",
                           args->path+"/"+args->shader_filename+".fs");
	printf("PROGRAMID: %i\n",programID);

  GLCanvas::initializeVBOs();
  GLCanvas::setupVBOs();

  // ===========================
  // initial placement of camera 
  // look at an object scaled & positioned to just fit in the box (-1,-1,-1)->(1,1,1)
  glm::vec3 camera_position = glm::vec3(1,3,8);
  glm::vec3 point_of_interest = glm::vec3(0,0,0);
  glm::vec3 up = glm::vec3(0,1,0);
  float angle = 20.0;
  camera = new PerspectiveCamera(camera_position, point_of_interest, up, angle);
  camera->glPlaceCamera(); 

  /** Initialize the target texture to be rendered to. */
  /*side = side effect*/

	printf("Enter create target\n");

	/* INFORMATION PASS TEXTURES */
	CreateBufferTarget( &targetBuffer, &targetDepthBuffer);
	/* create the buffer for albedo */
  CreateAndBindTextureTarget( GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, targetBuffer, &albedoTargetTexture);
	/* create the buffer for normals */
  CreateAndBindTextureTarget( GL_RGB16F, GL_RGB, GL_FLOAT, targetBuffer, &normalTargetTexture);
	/* Create the buffer for the positions */
  CreateAndBindTextureTarget( GL_RGB32F, GL_RGB, GL_FLOAT, targetBuffer, &positionTargetTexture);

	/* LIGHTING PASS TEXTURES */
	CreateBufferTarget( &lightBuffer, &lightDepthBuffer);
	/* Create the buffer to accrete light */
  CreateAndBindTextureTarget( GL_RGB32F, GL_RGB, GL_FLOAT, lightBuffer, &lightTargetTexture);

	printf("Out of create target\n");
		
	/* Inform opengl that there are a bunch of buffers to be rendered to.*/
  // framebuffer config
  glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, albedoTargetTexture, 0);
  glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normalTargetTexture, 0);
  glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, positionTargetTexture, 0);
  GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, DrawBuffers);

  /** Initialize the stupid quad in world space to render the synthesized texture directly onto, because apparently a framebuffer blit is just
    TOOOOOO hard... */
  {
    glGenVertexArrays(1, &screenQuadVAO);
    glBindVertexArray(screenQuadVAO);
    static const GLfloat screen_quad_vert_worldlocs[] = 
    {
    	-1.0f, -1.0f, 0.0f,
    	1.0f, -1.0f, 0.0f,
    	-1.0f,  1.0f, 0.0f,
    	-1.0f,  1.0f, 0.0f,
    	1.0f, -1.0f, 0.0f,
    	1.0f,  1.0f, 0.0f 
    };

    glGenBuffers(1, &screenQuadData);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadData);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_vert_worldlocs), screen_quad_vert_worldlocs, GL_STATIC_DRAW); 

    // Create and compile our GLSL program from the shaders
    screenQuadShaderProgram = LoadShaders( args->path+"/"+"pass"+".vs",
                           		            args->path+"/"+"pass"+".fs");
    screenQuadAlbedo = glGetUniformLocation(screenQuadShaderProgram, "albedo");
    screenQuadNormal = glGetUniformLocation(screenQuadShaderProgram, "normal");
    screenQuadPosition = glGetUniformLocation(screenQuadShaderProgram, "inposition");
    screenQuadTexSize = glGetUniformLocation(screenQuadShaderProgram, "texSize");
		printf("screenQuadProgram: %i\n", screenQuadShaderProgram);

		lightingProgram = LoadShaders( args->path+"/"+"lighting"+".vs",
																	args->path+"/"+"lighting"+".fs");
  	lightQuadAlbedo = glGetUniformLocation(lightingProgram, "albedo");
  	lightQuadNormal = glGetUniformLocation(lightingProgram, "normal");
  	lightQuadPosition = glGetUniformLocation(lightingProgram, "inposition");

  	lightQuadTexSize = glGetUniformLocation(lightingProgram, "texSize");

		lightQuadMVP = glGetUniformLocation(lightingProgram, "MVP");
		lightQuadM = glGetUniformLocation(lightingProgram, "M");

		lightRadius = glGetUniformLocation(lightingProgram, "radius");
		lightColorin = glGetUniformLocation(lightingProgram, "colorin");
		printf("lightProgram: %i\n", lightingProgram);
  }

  HandleGLError("finished glcanvas initialize");
}

void GLCanvas::CreateBufferTarget( GLuint *targetBuffer, GLuint *targetDepthBuffer)
{
	int width = args->width;
	int height = args->height;

	// framebuffer
	glGenFramebuffers(1, targetBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *targetBuffer);

	if (targetDepthBuffer)
	{
		// Depth buffer
		glGenRenderbuffers(1, targetDepthBuffer);
		glBindRenderbuffer( GL_RENDERBUFFER, *targetDepthBuffer);
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *targetDepthBuffer);
	}

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		printf("Failed to complete the framebuffer that will be used for rendering to a target texture.\n");
		exit(1);
	}
	else 
	{
		printf("Successfully initialized and bound framebuffer.\n");
	}
}


void GLCanvas::CreateAndBindTextureTarget(GLuint internalFormat, GLuint format, GLuint pixelData, GLuint frameBuffer, GLuint *renderTarget)
{
	glBindFramebuffer(GL_FRAMEBUFFER, targetBuffer);

	int width = args->width;
	int height = args->height;

	// target texture
	glGenTextures(1, renderTarget);
	glBindTexture(GL_TEXTURE_2D, *renderTarget);

	// target texture config
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelData, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void GLCanvas::drawPost()
{
	glClearColor(0.2f,0.2f,0.6f,1);

	glUseProgram(screenQuadShaderProgram);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnableVertexAttribArray(0);
	glUniform1i(screenQuadAlbedo, 0);
	glUniform1i(screenQuadNormal, 1);
	glUniform1i(screenQuadPosition, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedoTargetTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTargetTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, positionTargetTexture);

	glBindBuffer(GL_ARRAY_BUFFER, screenQuadData);
	glUniform2f(screenQuadTexSize, args->width, args->height);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
}


void GLCanvas::animate(){
#if _WIN32    
  static int last_tick_count;
  static int last_fps_count;
  static int frames = -1;
  if (frames == -1) {    
    last_fps_count = last_tick_count = GetTickCount();
    frames = 0;
  }
  if (args->animate) {
    frames++;
    int this_tick_count = GetTickCount();
    // difference in milliseconds
    double diff = 0.001*(this_tick_count-last_tick_count);
    last_tick_count = this_tick_count;
    args->timer += diff;
    double diff_fps_time = 0.001*(this_tick_count-last_fps_count);
    if (diff_fps_time > 1.00) {      
      float fps = frames / float(diff_fps_time);
      std::cout << "fps: " << fps << std::endl;
      frames = 0;
      last_fps_count = this_tick_count;
    }
    //geometry->setupVBOs();
  } else {
    last_tick_count = last_fps_count = GetTickCount();
  }
#else
  static timeval last_time;
  static timeval last_fps_time;
  static int frames = -1;
  if (frames == -1) {
    gettimeofday(&last_time,NULL);
    last_fps_time = last_time;
    frames = 0;
  }
  if (args->animate) {
    frames++;
    timeval this_time;
    gettimeofday(&this_time,NULL);
    // compute the difference from last time
    double diff = this_time.tv_sec - last_time.tv_sec + 
      0.000001 * (this_time.tv_usec - last_time.tv_usec);
    double diff_fps_time = this_time.tv_sec - last_fps_time.tv_sec + 
      0.000001 * (this_time.tv_usec - last_fps_time.tv_usec);
    last_time = this_time;
    // print out stats on the FPS occasionally
    if (diff_fps_time > 1.00) {      
      float fps = frames / float(diff_fps_time);
      std::cout << "fps: " << fps << std::endl;
      frames = 0;
      last_fps_time = this_time;
    }
    args->timer += diff;
    //geometry->setupVBOs();
  } else {
    gettimeofday(&last_time, NULL);
    last_fps_time = last_time;
    frames = 0;
    // if we aren't animating the light source, avoid busy-waiting!
    //usleep (100);
  }
#endif
  
}


void GLCanvas::initializeVBOs(){
  HandleGLError("enter initilizeVBOs()");
  glGenVertexArrays(1, &render_VAO);
  glBindVertexArray(render_VAO);
  GLCanvas::MatrixID = glGetUniformLocation(GLCanvas::programID, "MVP");
  GLCanvas::ViewMatrixID = glGetUniformLocation(GLCanvas::programID, "V");
  GLCanvas::ModelMatrixID = glGetUniformLocation(GLCanvas::programID, "M");
	GLCanvas::ColorID = glGetUniformLocation(GLCanvas::programID, "colorin");

  geometry->initializeVBOs();
  HandleGLError("leaving initilizeVBOs()");
}


void GLCanvas::setupVBOs(){
  HandleGLError("enter GLCanvas::setupVBOs()");
  assert (geometry != NULL);
  geometry->setupVBOs();
  HandleGLError("leaving GLCanvas::setupVBOs()");
}


void GLCanvas::drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix){
  HandleGLError("enter GlCanvas::drawVBOs()");

  glUniformMatrix4fv(GLCanvas::ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

  glm::vec3 cameraPosition = camera->camera_position;

  geometry->drawVBOs(ProjectionMatrix, ViewMatrix);
  HandleGLError("leaving GlCanvas::drawVBOs()");
}


void GLCanvas::cleanupVBOs(){
  bbox.cleanupVBOs();
  geometry->cleanupVBOs();
}


// ========================================================
// Callback function for mouse click or release
// ========================================================

void GLCanvas::mousebuttonCB(GLFWwindow *window, int which_button, int action, int mods) {
  // store the current state of the mouse buttons
  if (which_button == GLFW_MOUSE_BUTTON_1) {
    if (action == GLFW_PRESS) {
      leftMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      leftMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_2) {
    if (action == GLFW_PRESS) {
      rightMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      rightMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_3) {
    if (action == GLFW_PRESS) {
      middleMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      middleMousePressed = false;
    }
  }
}	

// ========================================================
// Callback function for mouse drag
// ========================================================

void GLCanvas::mousemotionCB(GLFWwindow *window, double x, double y) {

  // camera controls that work well for a 3 button mouse
  if (!shiftKeyPressed && !controlKeyPressed && !altKeyPressed) {
    if (leftMousePressed) {
      camera->rotateCamera(mouseX-x,mouseY-y);
    } else if (middleMousePressed)  {
      camera->truckCamera(mouseX-x, y-mouseY);
    } else if (rightMousePressed) {
      camera->dollyCamera(mouseY-y);
    }
  }

  if (leftMousePressed || middleMousePressed || rightMousePressed) {
    if (shiftKeyPressed) {
      camera->zoomCamera(mouseY-y);
    }
    // allow reasonable control for a non-3 button mouse
    if (controlKeyPressed) {
      camera->truckCamera(mouseX-x, y-mouseY);    
    }
    if (altKeyPressed) {
      camera->dollyCamera(y-mouseY);    
    }
  }
  mouseX = x;
  mouseY = y;
}

// ========================================================
// Callback function for keyboard events
// ========================================================

void GLCanvas::keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // store the modifier keys
  shiftKeyPressed = (GLFW_MOD_SHIFT & mods);
  controlKeyPressed = (GLFW_MOD_CONTROL & mods);
  altKeyPressed = (GLFW_MOD_ALT & mods);
  superKeyPressed = (GLFW_MOD_SUPER & mods);
  // non modifier key actions

  if (key == GLFW_KEY_ESCAPE || key == 'q' || key == 'Q') {
    glfwSetWindowShouldClose(GLCanvas::window, GL_TRUE);
  }

  // other normal ascii keys...
  if ( (action == GLFW_PRESS || action == GLFW_REPEAT) && key < 256) {
    switch (key) {
    case 'b': case 'B':
      args->bounding_box = !args->bounding_box;
      geometry->setupVBOs();
      break;
    case 'g': case 'G':
      args->geometry = !args->geometry;
      geometry->setupVBOs();
      break;
    case 'a': case 'A':
      args->animate = !args->animate;
      break;
    case ' ':
      args->timer += 0.1;
      geometry->setupVBOs();
      break;
    case 'n': case 'N':
      args->gouraud_normals = !args->gouraud_normals;
      geometry->setupVBOs();
      break;
    case 'x': case 'X':
      args->whichshader = (args->whichshader+1)%4;
      std::cout << "CURRENT SHADER: " << args->whichshader;
      if (args->whichshader == 0) { std::cout << " none" << std::endl; }
      if (args->whichshader == 1) { std::cout << " checkerboard" << std::endl; }
      if (args->whichshader == 2) { std::cout << " orange" << std::endl; }
      if (args->whichshader == 3) { std::cout << " wood" << std::endl; }
      geometry->setupVBOs();
      break;
    case 'l' : case 'L':
      //LoadCompileLinkShaders();
      geometry->setupVBOs();
      break;
    case 'q':  case 'Q':
      // quit
      glfwSetWindowShouldClose(GLCanvas::window, GL_TRUE);
      break;
    default:
      std::cout << "UNKNOWN KEYBOARD INPUT  '" << (char)key << "'" << std::endl;
    }
    setupVBOs();
  }
}


// ========================================================
// Load the vertex & fragment shaders
// ========================================================

GLuint LoadShaders(const std::string &vertex_file_path,const std::string &fragment_file_path){

  std::cout << "load shaders" << std::endl;

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
  if (VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  
  GLint Result = GL_FALSE;
 
  // Compile Vertex Shader
  std::cout << "Compiling shader : " << vertex_file_path << std::endl;
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
 
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(VertexShaderID, 1024, &log_length, message);
    std::cout << "VERTEX ERROR " << message << std::endl;
    exit(1);
  }

  // Compile Fragment Shader
  std::cout << "Compiling shader : " << fragment_file_path << std::endl;
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
 
  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(FragmentShaderID, 1024, &log_length, message);
    std::cout << "FRAGMENT ERROR " << message << std::endl;
    exit(1);
  }

  // Link the program
  std::cout << "Linking shader program" << std::endl;
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);
 
  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(ProgramID, 1024, &log_length, message);
    std::cout << "SHADER PROGRAM ERROR " << message << std::endl;
    exit(1);
  }

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

// ========================================================
// Functions related to error handling
// ========================================================

void GLCanvas::error_callback(int error, const char* description) {
  std::cerr << "ERROR CALLBACK: " << description << std::endl;
}

std::string WhichGLError(GLenum &error) {
  switch (error) {
  case GL_NO_ERROR:
    return "NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";
  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";
  default:
    return "OTHER GL ERROR";
  }
}

int HandleGLError(const std::string &message, bool ignore) {
  GLenum error;
  int i = 0;
  while ((error = glGetError()) != GL_NO_ERROR) {
    if (!ignore) {
      if (message != "") {
	std::cerr << "[" << message << "] ";
      }
      std::cerr << "GL ERROR(" << i << ") " << WhichGLError(error) << std::endl;
    }
    i++;
  }
  if (i == 0) return 1;
  return 0;
}

// ========================================================
// ========================================================
