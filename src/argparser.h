#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include <glm/glm.hpp>
#include <random>

// ================================================================================
// ================================================================================

inline void separatePathAndFile(const std::string &input, std::string &path, std::string &file) {
  // we need to separate the filename from the path
  // (we assume the vertex & fragment shaders are in the same directory)
  // first, locate the last '/' in the filename
  size_t last = std::string::npos;  
  while (1) {
    int next = input.find('/',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    next = input.find('\\',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    break;
  }
  if (last == std::string::npos) {
    // if there is no directory in the filename
    file = input;
    path = ".";
  } else {
    // separate filename & path
    file = input.substr(last+1,input.size()-last-1);
    path = input.substr(0,last);
  }
}

// ======================================================================
// Class to collect all the high-level rendering parameters controlled
// by the command line or the keyboard input
// ======================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();
    // parse the command line arguments
    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]) == std::string("-input") || 
          std::string(argv[i]) == std::string("-i")) {
        i++; assert (i < argc); 
        separatePathAndFile(argv[i],path,input_file);
      } else if (std::string(argv[i]) == std::string("-shader")) {
        shader_filename = std::string(argv[i]);
      } else if (std::string(argv[i]) == std::string("-size")) {
        i++; assert (i < argc); 
        width = height = atoi(argv[i]);
      } else {
	std::cout << "ERROR: unknown command line argument " 
		  << i << ": '" << argv[i] << "'" << std::endl;
	exit(1);
      }
    }
  }

  double rand() {
#if 1
    // random seed
    static std::random_device rd;    
    static std::mt19937 engine(rd());
#else
    // deterministic randomness
    static std::mt19937 engine(37);
#endif
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine);
  }

  void DefaultValues() {
    // BASIC RENDERING PARAMETERS
    input_file = "";
    path = "";
    shader_filename = "hw4_shader";
    width = 300;
    height = 300;
    geometry = true;
    bounding_box = false;
    gouraud_normals = false;
    lightbox = true;
    whichshader = 0;
    timer = 0.0;
    animate = false;
  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)

  // BASIC RENDERING PARAMETERS
  std::string input_file;
  std::string path;
  std::string shader_filename;
  int width;
  int height;
  bool geometry;
  bool bounding_box;
  bool lightbox;
  bool gouraud_normals;
  int whichshader;
  float timer;
  bool animate;
};

#endif
