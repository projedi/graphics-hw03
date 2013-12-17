#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "controller.h"

controller* controller::INSTANCE;

void mouseButtonCallback(GLFWwindow*, int button, int action, int mods) {
   controller::instance()->mouseButtonCallback(button, action, mods);
}

void scrollCallback(GLFWwindow*, double xoffset, double yoffset) {
   controller::instance()->scrollCallback(xoffset, yoffset);
}

void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
   controller::instance()->keyCallback(key, scancode, action, mods);
}

void cursorPosCallback(GLFWwindow*, double xpos, double ypos) {
   controller::instance()->cursorPosCallback(xpos, ypos);
}

void charCallback(GLFWwindow*, unsigned scancode) {
   controller::instance()->charCallback(scancode);
}

controller::controller(GLFWwindow* window, TwBar* bar):
   _window(window), _tweak_bar(bar) { }

void controller::init(GLFWwindow* window, TwBar* bar) {
   if(INSTANCE) return;
   glfwSetMouseButtonCallback(window, ::mouseButtonCallback);
   glfwSetScrollCallback(window, ::scrollCallback);
   glfwSetKeyCallback(window, ::keyCallback);
   glfwSetCursorPosCallback(window, ::cursorPosCallback);
   glfwSetCharCallback(window, ::charCallback);
   INSTANCE = new controller(window, bar);
}

void controller::deinit() {
   if(!INSTANCE) return;
   glfwSetMouseButtonCallback(INSTANCE->_window, nullptr);
   glfwSetScrollCallback(INSTANCE->_window, nullptr);
   glfwSetKeyCallback(INSTANCE->_window, nullptr);
   glfwSetCursorPosCallback(INSTANCE->_window, nullptr);
   glfwSetCharCallback(INSTANCE->_window, nullptr);
   delete INSTANCE;
}

void controller::mouseButtonCallback(int button, int action, int mods) {
   if(TwEventMouseButtonGLFW(button, action)) return;
   if(_mouse_button_callback)
      _mouse_button_callback(button, action, mods);
}

void controller::scrollCallback(double xoffset, double yoffset) {
   if(TwEventMouseWheelGLFW(yoffset)) return;
   if(_scroll_callback)
      _scroll_callback(xoffset, yoffset);
}

void controller::keyCallback(int key, int scancode, int action, int mods) {
   if(TwEventKeyGLFW(key, action)) return;
   if(_key_callback)
      _key_callback(key, scancode, action, mods);
}

void controller::cursorPosCallback(double xpos, double ypos) {
   if(TwEventMousePosGLFW(int(xpos), int(ypos))) return;
   if(_cursor_pos_callback)
      _cursor_pos_callback(xpos, ypos);
}

void controller::charCallback(unsigned scancode) {
   if(TwEventCharGLFW(scancode, GLFW_PRESS)) return;
}
