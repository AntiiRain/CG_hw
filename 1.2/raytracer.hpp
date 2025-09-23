#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <limits>

// Use the glm namespace for convenience
using namespace glm;

// Structure to define a ray
struct Ray {
  vec3 origin;
  vec3 direction;
};

// Structure for material properties
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
  float reflectivity;
};

// Structure for a sphere object
struct Sphere {
  vec3 center;
  float radius;
  Material material;
};

// Function to calculate the intersection of a ray with a sphere
// Returns the distance 't' to the intersection point, or -1.0 if no intersection.
float interaction(const Sphere& S, const vec3& ray_origin, const vec3& ray_direction) {
  vec3 oc = ray_origin - S.center;
  float a = dot(ray_direction, ray_direction);
  float b = 2.0f * dot(oc, ray_direction);
  float c = dot(oc, oc) - S.radius * S.radius;
  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0f;
  } else {
    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
    if (t0 > 0.001f) return t0; // Return the closest valid intersection

    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
    if (t1 > 0.001f) return t1;

    return -1.0f;
  }
}

// The core recursive ray casting function
vec3 castRay(const Ray& ray, const std::vector<Sphere>& scene_objects, const vec3& light_direction, const vec3& light_color, const vec3& ambient_light, int depth) {
  // Base case 1: Recursion depth limit reached
  if (depth <= 0) {
    return vec3(0.0f, 0.0f, 0.0f);
  }

  float closest_t = std::numeric_limits<float>::max();
  const Sphere* hit_sphere = nullptr;

  // Find the closest object intersection
  for (const auto& sphere : scene_objects) {
    float t = interaction(sphere, ray.origin, ray.direction);
    if (t > 0.0f && t < closest_t) {
      closest_t = t;
      hit_sphere = &sphere;
    }
  }

  // Base case 2: No object was hit
  if (hit_sphere == nullptr) {
    return vec3(0.1f, 0.15f, 0.2f); // Return background color
  }

  // An object was hit, calculate shading
  vec3 hit_point = ray.origin + closest_t * ray.direction;
  vec3 normal = normalize(hit_point - hit_sphere->center);
  vec3 view_dir = -ray.direction;

  // Phong Illumination Model
  vec3 ambient = hit_sphere->material.ambient * ambient_light;
  float diff_factor = std::max(dot(normal, -light_direction), 0.0f);
  vec3 diffuse = hit_sphere->material.diffuse * diff_factor * light_color;
  vec3 reflect_dir_light = reflect(light_direction, normal);
  float spec_factor = std::pow(std::max(dot(view_dir, reflect_dir_light), 0.0f), hit_sphere->material.shininess);
  vec3 specular = hit_sphere->material.specular * spec_factor * light_color;
  vec3 local_color = ambient + diffuse + specular;

  // Handle reflections
  float reflect_amount = hit_sphere->material.reflectivity;
  if (reflect_amount > 0) {
    vec3 reflect_dir_view = reflect(ray.direction, normal);
    // Create the reflected ray, offsetting the origin slightly to avoid self-intersection
    Ray reflected_ray = {hit_point + normal * 0.001f, reflect_dir_view};

    // Recursive call to get the color from the reflection
    vec3 reflected_color = castRay(reflected_ray, scene_objects, light_direction, light_color, ambient_light, depth - 1);

    // Blend local and reflected colors
    return local_color * (1.0f - reflect_amount) + reflected_color * reflect_amount;
  } else {
    // Not a reflective surface, return only the local color
    return local_color;
  }
}


#endif // RAYTRACER_HPP