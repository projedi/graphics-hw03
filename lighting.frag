#version 330 core

// 0 - PHONG, 1 - BLINN_PHONG
uniform int lighting;
// 0 - per facet, 1 - per vertex, 2 - per fragment
uniform int coloring;
uniform vec3 specular_reflectance;
uniform vec3 diffuse_reflectance;
uniform vec3 ambient_reflectance;
uniform float specular_power;
uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_half_distance;
uniform mat4 view_matrix;

in vec3 pos_interpolated;
in vec3 norm_interpolated;
flat in vec3 color_flat;
in vec3 color_interpolated;

out vec3 color;

vec3 get_color(int lighting, vec3 spec_refl, vec3 diff_refl, vec3 amb_refl,
      float spec_power, vec3 light_pos, vec3 light_color, float light_half_dist,
      vec3 pos, vec3 norm) {
   vec3 lpos = vec3(view_matrix * vec4(light_position, 1));
   vec3 dir_to_light = normalize(lpos - pos);
   // Quadratic.
   float power =
      max(1 - dot(lpos - pos, lpos - pos) / 2 / light_half_dist / light_half_dist, 0);
   vec3 amb_int = light_color * amb_refl;
   float diff_coef = max(dot(dir_to_light, norm), 0);
   vec3 diff_int = light_color * diff_refl * diff_coef * power;
   float spec_coef = 0;
   if(lighting == 0) {
      spec_coef = max(dot(reflect(-dir_to_light, norm), normalize(-pos)), 0);
   } else if(lighting == 1) {
      spec_coef = max(dot(normalize(normalize(-pos) + dir_to_light), norm), 0);
   }
   vec3 spec_int = light_color * spec_refl * pow(spec_coef, spec_power) * power;
   return amb_int + diff_int + spec_int;
}

void main() {
   if(coloring == 0) color = color_flat;
   else if(coloring == 1) color = color_interpolated;
   else color =
      get_color(lighting, specular_reflectance, diffuse_reflectance,
         ambient_reflectance, specular_power, light_position, light_color,
         light_half_distance, pos_interpolated, normalize(norm_interpolated));
}
