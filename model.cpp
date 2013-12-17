#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtx/transform.hpp>

#include "model.h"
#include "gl_context.h"
#include "camera.h"

struct model_data {
   model_data(gl_context const& ctx): _context(ctx) { }
   ~model_data() {
      glDeleteBuffers(1, &_vertexbuffer);
      glDeleteBuffers(1, &_normalbuffer);
      glDeleteBuffers(1, &_light_vertexbuffer);
      _context.remove_shader(_lighting_shader_id);
      _context.remove_shader(_wire_shader_id);
   }
   gl_context const& _context;
   GLuint _vertexbuffer;
   GLuint _light_vertexbuffer;
   GLuint _normalbuffer;
   GLuint _lighting_shader_id;
   GLuint _wire_shader_id;
};

model::model(gl_context const& context, std::vector<glm::vec3> const& vertices,
      std::vector<glm::vec3> const& normals, glm::mat4 const& model_matrix):
   _context(context),
   _lighting_type(lighting_type::PHONG),
   _coloring_type(coloring_type::PER_FRAGMENT),
   _specular_reflectance(1.0, 0.0, 0.0),
   _diffuse_reflectance(0.0, 0.6, 0.0),
   _ambient_reflectance(0.0, 0.1, 0.1),
   _specular_power(3),
   _light_position(0.0, 5.0, 0.0),
   _light_color(1.0, 1.0, 1.0),
   _light_half_distance(2),
   _vertex_count(vertices.size()),
   _model_matrix(model_matrix),
   _data(new model_data(_context)) {

   glGenBuffers(1, &_data->_vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, _vertex_count * sizeof(glm::vec3), &vertices[0],
         GL_STATIC_DRAW);

   glGenBuffers(1, &_data->_normalbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_normalbuffer);
   glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0],
         GL_STATIC_DRAW);

   std::vector<glm::vec3> sphere = generate_sphere();
   _light_vertex_count = sphere.size();
   glGenBuffers(1, &_data->_light_vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_light_vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(glm::vec3), &sphere[0],
         GL_STATIC_DRAW);

   _data->_lighting_shader_id =
       _context.add_shader("lighting.vert", "lighting.frag");
   _data->_wire_shader_id = _context.add_shader("wirecolor.vert", "wirecolor.frag");
}

void model::draw(glm::mat4 const& view, glm::mat4 const& projection) {
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_vertexbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_normalbuffer);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   _context.use_shader(_data->_lighting_shader_id);
   glUniform1i(glGetUniformLocation(_data->_lighting_shader_id, "lighting"),
         (int)_lighting_type);
   glUniform1i(glGetUniformLocation(_data->_lighting_shader_id, "coloring"),
         (int)_coloring_type);
   glUniform3fv(glGetUniformLocation(_data->_lighting_shader_id, "specular_reflectance"),
         1, &_specular_reflectance[0]);
   glUniform3fv(glGetUniformLocation(_data->_lighting_shader_id, "diffuse_reflectance"),
         1, &_diffuse_reflectance[0]);
   glUniform3fv(glGetUniformLocation(_data->_lighting_shader_id, "ambient_reflectance"),
         1, &_ambient_reflectance[0]);
   glUniform1f(glGetUniformLocation(_data->_lighting_shader_id, "specular_power"),
         _specular_power);
   glUniform3fv(glGetUniformLocation(_data->_lighting_shader_id, "light_position"),
         1, &_light_position[0]);
   glUniform3fv(glGetUniformLocation(_data->_lighting_shader_id, "light_color"),
         1, &_light_color[0]);
   glUniform1f(glGetUniformLocation(_data->_lighting_shader_id, "light_half_distance"),
         _light_half_distance);
   glUniformMatrix4fv(glGetUniformLocation(_data->_lighting_shader_id, "model_matrix"),
         1, GL_FALSE, &_model_matrix[0][0]);
   glUniformMatrix4fv(glGetUniformLocation(_data->_lighting_shader_id, "view_matrix"),
         1, GL_FALSE, &view[0][0]);
   glUniformMatrix4fv(glGetUniformLocation(_data->_lighting_shader_id, "projection_matrix"),
         1, GL_FALSE, &projection[0][0]);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glPolygonOffset(1, 1);
   glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
   glDisable(GL_POLYGON_OFFSET_FILL);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_light_vertexbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   _context.use_shader(_data->_wire_shader_id);
   glm::vec3 scale(_light_half_distance);
   glm::mat4 mvp = projection * view * glm::translate(_light_position) * glm::scale(scale);
   glUniformMatrix4fv(glGetUniformLocation(_data->_wire_shader_id, "MVP"),
         1, GL_FALSE, &mvp[0][0]);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDrawArrays(GL_TRIANGLES, 0, _light_vertex_count);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   glDisableVertexAttribArray(0);
}
