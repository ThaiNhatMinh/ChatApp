#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <memory>
// OpenGL Include
// GLEW
#define GLEW_STATIC
#include "include\gl\glew.h"

// GLFW
#include "include\GLFW\glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "include\GLFW\glfw3native.h"
// TinyXML 2
#include "include\tinyxml2.h"


// libary
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "tinyxml2.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Ws2_32.lib")

// Math
#include "include\glm\mat4x4.hpp"
#include "include\glm\vec3.hpp"
#include "include\glm\vec4.hpp"
#include "include\glm\vec2.hpp"

#include "include\glm\gtc\quaternion.hpp"
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtc\type_ptr.hpp"
#include "include\glm\gtc\constants.hpp"

#include "include\glm\gtx\quaternion.hpp"
#include "include\glm\gtx\compatibility.hpp"
#include "include\glm\gtx\euler_angles.hpp"
#include "include\glm\geometric.hpp"
#include "include\glm\detail\func_matrix.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;

// share class
#include "Net.h"
#include "Texture.h"
#include "OpenGLRenderer.h"
#include "Shader.h"
#include "imguiMesh.h"
#include "SystemUI.h"
#include "Windows.h"
#include "Application.h"
#include "Buffer.h"
#include "Socket.h"
#include "Log.h"