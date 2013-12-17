#include "model.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtx/transform.hpp>

glm::vec3 load_obj_file(std::string const& filename, std::vector<glm::vec3> &vertices,
      std::vector<glm::vec2>& uvs, std::vector<glm::vec3> &normals) {
   std::ifstream in(filename);
   if(!in) {
      std::cerr << "Cannot open obj file " << filename << std::endl;
      throw std::logic_error("Can't open file");
   }
   std::vector<glm::vec3> vbuf;
   std::vector<glm::vec3> vnbuf;
   std::vector<glm::vec2> vtbuf;
   std::string line;
   glm::vec3 center;
   while(std::getline(in, line)) {
      if(line.substr(0,2) == "v ") {
         std::stringstream s(line.substr(2));
         glm::vec3 v;
         s >> v.x >> v.y >> v.z;
         center += v;
         vbuf.push_back(v);
      } else if(line.substr(0,3) == "vn ") {
         std::stringstream s(line.substr(3));
         glm::vec3 vn;
         s >> vn.x >> vn.y >> vn.z;
         vnbuf.push_back(vn);
      } else if(line.substr(0,3) == "vt ") {
         std::stringstream s(line.substr(3));
         glm::vec2 vt;
         s >> vt.x >> vt.y;
         vtbuf.push_back(vt);
      } else if(line.substr(0,2) == "f ") {
         // Will parse only f v//vn
         std::stringstream s(line.substr(2));
         int v1,vn1,v2,vn2,v3,vn3;
         char skip;
         s >> v1 >> skip >> skip >> vn1
           >> v2 >> skip >> skip >> vn2
           >> v3 >> skip >> skip >> vn3;
         vertices.push_back(vbuf[v1 - 1]);
         vertices.push_back(vbuf[v2 - 1]);
         vertices.push_back(vbuf[v3 - 1]);
         normals.push_back(vnbuf[vn1 - 1]);
         normals.push_back(vnbuf[vn2 - 1]);
         normals.push_back(vnbuf[vn3 - 1]);
      }
   }
   center /= vbuf.size();
   return center;
}

model load_obj_model(gl_context const& ctx, std::string const& filename) {
   std::vector<glm::vec3> vs;
   std::vector<glm::vec2> uvs;
   std::vector<glm::vec3> ns;
   glm::vec3 center = load_obj_file(filename, vs, uvs, ns);
   glm::vec3 scale(100,100,100);
   return model(ctx, vs, ns, glm::scale(scale) * glm::translate(-center));
}
