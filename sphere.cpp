#include <iostream>

#include "model.h"

std::ostream& operator<<(std::ostream& ost, glm::vec3 const& v) {
   return ost << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

std::ostream& operator<<(std::ostream& ost, glm::vec2 const& v) {
   return ost << "(" << v[0] << ", " << v[1] << ")";
}

struct xyzuv {
   xyzuv(glm::vec3 v): xyz(v) {
      uv[0] = (1 + std::atan2(v[0], v[2]) / M_PI) / 2.0;
      uv[1] = 1 - std::acos(v[1]) / M_PI;
      //if(uv[0] > 1 || uv[0] < 0 || uv[1] > 1 || uv[1] < 0) std::cerr << v << ": " << uv << std::endl;
   }
   glm::vec3 xyz;
   glm::vec2 uv;
};

std::ostream& operator<<(std::ostream& ost, xyzuv const& v) {
   return ost << v.xyz << "<->" << v.uv;
}

// This isn't a sum operator.
xyzuv operator+(xyzuv const& a, xyzuv const& b) {
   return xyzuv(glm::normalize(a.xyz + b.xyz));
}

struct triangle {
   triangle(xyzuv a, xyzuv b, xyzuv c): A(a), B(b), C(c) {
      auto dist = [](xyzuv a, xyzuv b, float d) { return std::abs(a.uv[0] + d - b.uv[0]); };
      float a2b = dist(a, b, 0);
      float a2c = dist(a, c, 0);
      float b2c = dist(b, c, 0);
      if(dist(a, b, 1) < a2b && dist(a, c, 1) < a2c) {
         A.uv[0] += 1;
      } else if(dist(a, b, -1) < a2b && dist(a, c, -1) < a2c) {
         A.uv[0] -= 1;
      } else if(dist(b, a, 1) < a2b && dist(b, c, 1) < b2c) {
         B.uv[0] += 1;
      } else if(dist(b, a, -1) < a2b && dist(b, c, -1) < b2c) {
         B.uv[0] -= 1;
      } else if(dist(c, a, 1) < a2c && dist(c, b, 1) < b2c) {
         C.uv[0] += 1;
      } else if(dist(c, a, -1) < a2c && dist(c, b, -1) < b2c) {
         C.uv[0] -= 1;
      }
   }
   xyzuv A;
   xyzuv B;
   xyzuv C;
};

std::ostream& operator<<(std::ostream& ost, triangle const& t) {
   return ost << "triangle { " << t.A << ", " << t.B << ", " << t.C << " }";
}

std::vector<triangle> divide_triangle(triangle const& t) {
   xyzuv f = t.A + t.B;
   xyzuv g = t.B + t.C;
   xyzuv h = t.A + t.C;
   std::vector<triangle> res;
   res.push_back(triangle(t.A, f, h));
   res.push_back(triangle(f, g, h));
   res.push_back(triangle(g, t.C, h));
   res.push_back(triangle(f, t.B, g));
   return res;
}

std::vector<triangle> iteration(std::vector<triangle> const& ts) {
   std::vector<triangle> res;
   for(auto t: ts) {
      auto trs = divide_triangle(t);
      res.insert(res.end(), trs.begin(), trs.end());
   }
   return res;
}

std::vector<triangle> iteration0() {
   float s32 = std::sqrt(3) / 2;
   xyzuv A(glm::vec3(0,0,1));
   xyzuv B(glm::vec3(s32,0,-0.5));
   xyzuv C(glm::vec3(-s32,0,-0.5));
   xyzuv D(glm::vec3(0,1,0));
   xyzuv E(glm::vec3(0,-1,0));
   std::vector<triangle> res;
   res.push_back(triangle(A, B, D));
   res.push_back(triangle(B, C, D));
   res.push_back(triangle(C, A, D));
   res.push_back(triangle(B, A, E));
   res.push_back(triangle(C, B, E));
   res.push_back(triangle(A, C, E));
   return res;
}

void convert_triangles(std::vector<triangle> const& ts,
      std::vector<glm::vec3>& xyzs, std::vector<glm::vec2>& uvs) {
   for(auto t: ts) {
      //std::cerr << t << std::endl;
      xyzs.push_back(t.A.xyz);
      xyzs.push_back(t.B.xyz);
      xyzs.push_back(t.C.xyz);
      uvs.push_back(t.A.uv);
      uvs.push_back(t.B.uv);
      uvs.push_back(t.C.uv);
   }
}

std::vector<glm::vec3> generate_sphere() {
   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> uvs;
   auto ts = iteration0();
   for(int i = 0; i != 3; ++i) ts = iteration(ts);
   convert_triangles(ts, vertices, uvs);
   return vertices;
}

//model create_sphere_model(gl_context const& ctx, send_mvp_function const& f) {
   // TODO: do
   //std::vector<glm::vec3> vertices;
   //std::vector<glm::vec2> uvs;
   //auto ts = iteration0();
   //for(int i = 0; i != 5; ++i) ts = iteration(ts);
   //convert_triangles(ts, vertices, uvs);
   //fipImage img;
   //if(!img.load("earth_texture_grid.bmp"))
      //throw std::runtime_error("Can't find earth_texture_grid.bmp texture");
   //return model(ctx, f, vertices, uvs, img.getWidth(), img.getHeight(), img.accessPixels());
//}
