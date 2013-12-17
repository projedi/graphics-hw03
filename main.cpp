#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "camera.h"
#include "controller.h"
#include "gl_context.h"
#include "model.h"

void setup_tweak_bar(gl_context const& context, model& model) {
   TwEnumVal lvals[] = { { lighting_type::PHONG, "Phong" },
      { lighting_type::BLINN_PHONG, "Blinn-Phong" } };
   TwType ltype = TwDefineEnum("lighting_type", lvals, 2);
   TwAddVarCB(context.tweak_bar(), "Lighting type", ltype,
         [](void const* val, void* m) {
            ((struct model*)m)->lighting() = *(lighting_type*)val;
         }, [](void* val, void* m) {
            *(lighting_type*)val = ((struct model*)m)->lighting();
         }, &model, "");

   TwEnumVal cvals[] = { { coloring_type::PER_FACET, "Per facet" },
      { coloring_type::PER_VERTEX, "Per vertex" },
      { coloring_type::PER_FRAGMENT, "Per fragment" } };
   TwType ctype = TwDefineEnum("coloring_type", cvals, 3);
   TwAddVarCB(context.tweak_bar(), "Coloring type", ctype,
         [](void const* val, void* m) {
            ((struct model*)m)->coloring() = *(coloring_type*)val;
         }, [](void* val, void* m) {
            *(coloring_type*)val = ((struct model*)m)->coloring();
         }, &model, "");

   TwAddVarCB(context.tweak_bar(), "Specular reflectance", TW_TYPE_COLOR3F,
         [](void const* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            m->specular_reflectance()[0] = val[0];
            m->specular_reflectance()[1] = val[1];
            m->specular_reflectance()[2] = val[2];
         }, [](void* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            val[0] = m->specular_reflectance()[0];
            val[1] = m->specular_reflectance()[1];
            val[2] = m->specular_reflectance()[2];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Diffuse reflectance", TW_TYPE_COLOR3F,
         [](void const* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            m->diffuse_reflectance()[0] = val[0];
            m->diffuse_reflectance()[1] = val[1];
            m->diffuse_reflectance()[2] = val[2];
         }, [](void* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            val[0] = m->diffuse_reflectance()[0];
            val[1] = m->diffuse_reflectance()[1];
            val[2] = m->diffuse_reflectance()[2];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Ambient reflectance", TW_TYPE_COLOR3F,
         [](void const* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            m->ambient_reflectance()[0] = val[0];
            m->ambient_reflectance()[1] = val[1];
            m->ambient_reflectance()[2] = val[2];
         }, [](void* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            val[0] = m->ambient_reflectance()[0];
            val[1] = m->ambient_reflectance()[1];
            val[2] = m->ambient_reflectance()[2];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Specular power", TW_TYPE_FLOAT,
         [](void const* val, void* m) {
            ((struct model*)m)->specular_power() = *(float*)val;
         }, [](void* val, void* m) {
            *(float*)val = ((struct model*)m)->specular_power();
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Light color", TW_TYPE_COLOR3F,
         [](void const* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            m->light_color()[0] = val[0];
            m->light_color()[1] = val[1];
            m->light_color()[2] = val[2];
         }, [](void* v, void* o) {
            struct model* m = (struct model*)o;
            float* val = (float*)v;
            val[0] = m->light_color()[0];
            val[1] = m->light_color()[1];
            val[2] = m->light_color()[2];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Light half distance", TW_TYPE_FLOAT,
         [](void const* val, void* m) {
            ((struct model*)m)->light_half_distance() = *(float*)val;
         }, [](void* val, void* m) {
            *(float*)val = ((struct model*)m)->light_half_distance();
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Light position X", TW_TYPE_FLOAT,
         [](void const* val, void* m) {
            ((struct model*)m)->light_position()[0] = *(float*)val;
         }, [](void* val, void* m) {
            *(float*)val = ((struct model*)m)->light_position()[0];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Light position Y", TW_TYPE_FLOAT,
         [](void const* val, void* m) {
            ((struct model*)m)->light_position()[1] = *(float*)val;
         }, [](void* val, void* m) {
            *(float*)val = ((struct model*)m)->light_position()[1];
         }, &model, "");
   TwAddVarCB(context.tweak_bar(), "Light position Z", TW_TYPE_FLOAT,
         [](void const* val, void* m) {
            ((struct model*)m)->light_position()[2] = *(float*)val;
         }, [](void* val, void* m) {
            *(float*)val = ((struct model*)m)->light_position()[2];
         }, &model, "");
}

void setup_controller(gl_context& context, camera& main_camera, model& m) {
   // Since controller is a singleton anyway it will do.
   setup_tweak_bar(context, m);
   static bool _rotation_mode = false;
   static double _mouse_x = 0;
   static double _mouse_y = 0;
   controller::instance()->mouse_button([&](int button, int action, int) {
      if(button == GLFW_MOUSE_BUTTON_LEFT) {
         _rotation_mode = (action == GLFW_PRESS);
         if(_rotation_mode) {
            glfwGetCursorPos(context.main_window(), &_mouse_x, &_mouse_y);
         }
      }
   });
   controller::instance()->scroll([&](double xoffset, double yoffset) {
      if(yoffset < 0) main_camera.zoom_out();
      else if(yoffset > 0) main_camera.zoom_in();
   });
   controller::instance()->key([&](int key, int, int action, int) {
      if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
         main_camera.zoom_in();
      else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
         main_camera.zoom_out();
   });
   controller::instance()->cursor_pos([&](double xpos, double ypos) {
      if(!_rotation_mode) return;
      double dx = xpos - _mouse_x;
      double dy = ypos - _mouse_y;
      float up_angle = -dx;
      float right_angle = dy;
      main_camera.set_angles(up_angle, right_angle);
      _mouse_x = xpos;
      _mouse_y = ypos;
   });
}

int main() {
   gl_context context("HW03", 800, 600);
   camera main_camera(context.main_window(), 0.1, 1000);
   model m = load_obj_model(context, "bunny_n.obj");
   controller::init(context.main_window(), context.tweak_bar());

   setup_controller(context, main_camera, m);

   glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);

   context.main_loop([&]() {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      main_camera.update_matrices();
      m.draw(main_camera.view(), main_camera.projection());
   });

   controller::deinit();
   return 0;
}
