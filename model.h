#include <functional>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct gl_context;

// I embedded light description in here too.

enum lighting_type { PHONG, BLINN_PHONG };
enum coloring_type { PER_FACET, PER_VERTEX, PER_FRAGMENT };

struct model_data;

struct model {
   model(gl_context const&, std::vector<glm::vec3> const& vertices,
         std::vector<glm::vec3> const& normals, glm::mat4 const& model_matrix);
   void draw(glm::mat4 const& view, glm::mat4 const& projection);
   lighting_type& lighting() { return _lighting_type; }
   coloring_type& coloring() { return _coloring_type; }
   glm::vec3& specular_reflectance() { return _specular_reflectance; }
   glm::vec3& diffuse_reflectance() { return _diffuse_reflectance; }
   glm::vec3& ambient_reflectance() { return _ambient_reflectance; }
   float& specular_power() { return _specular_power; }
   glm::vec3& light_position() { return _light_position; }
   glm::vec3& light_color() { return _light_color; }
   // Distance on which light has half the intensity.
   float& light_half_distance() { return _light_half_distance; }
private:
   gl_context const& _context;
   lighting_type _lighting_type;
   coloring_type _coloring_type;
   glm::vec3 _specular_reflectance;
   glm::vec3 _diffuse_reflectance;
   glm::vec3 _ambient_reflectance;
   float _specular_power;
   glm::vec3 _light_position;
   glm::vec3 _light_color;
   float _light_half_distance;
   GLsizei _vertex_count;
   glm::mat4 _model_matrix;
   std::shared_ptr<model_data> _data;
   GLsizei _light_vertex_count;
};

model load_obj_model(gl_context const&, std::string const& filename);
std::vector<glm::vec3> generate_sphere();
