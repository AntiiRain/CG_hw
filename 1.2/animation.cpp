#include <iostream>
#include <cstdio> // For sprintf
#include "raytracer.hpp"

// Include stb_image_write for saving PNG files
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
  // Image settings
  const int width = 400; // Smaller resolution for faster rendering
  const int height = 400;
  unsigned char image[width * height * 3];

  // Scene Setup
  vec3 light_direction = normalize(vec3(0.5f, -1.0f, -1.0f));
  vec3 light_color(1.0f, 1.0f, 1.0f);
  vec3 ambient_light(0.2f, 0.2f, 0.2f);
  float viewport_height = 50.0f;
  float viewport_width = 50.0f;
  const int MAX_DEPTH = 5;

  // Define scene objects
  std::vector<Sphere> scene_objects;
  scene_objects.push_back({vec3(-30.0f, 5.0f, -30.0f), 25.0f, {vec3(0.8f, 0.1f, 0.1f), vec3(0.8f, 0.1f, 0.1f), vec3(1.0f), 32.0f, 0.3f}});
  scene_objects.push_back({vec3(30.0f, -5.0f, -40.0f), 20.0f, {vec3(0.1f, 0.1f, 0.9f), vec3(0.1f, 0.1f, 0.9f), vec3(1.0f), 128.0f, 0.0f}});
  scene_objects.push_back({vec3(0.0f, -500.0f, -50.0f), 475.0f, {vec3(0.2f), vec3(0.5f), vec3(1.0f), 64.0f, 0.8f}});

  // Animation Loop
  int num_frames = 120; // 4 seconds at 30 fps
  for (int frame = 0; frame < num_frames; ++frame) {
    std::cout << "Rendering frame " << (frame + 1) << "/" << num_frames << "...\n";

    // Update scene for the current frame
    float angle = 2.0f * 3.14159f * static_cast<float>(frame) / static_cast<float>(num_frames);
    vec3 eye_position(sin(angle) * 120.0f, 40.0f, cos(angle) * 120.0f);
    scene_objects[0].center.y = 5.0f + 20.0f * sin(angle * 2.0f); // Red sphere bounces

    // Render the frame
    for(int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        // Robust camera ray generation
        vec3 target = vec3(0.0f, 0.0f, 0.0f);
        vec3 cam_forward = normalize(target - eye_position);
        vec3 cam_right = normalize(cross(vec3(0,1,0), cam_forward));
        vec3 cam_up = cross(cam_forward, cam_right);
        float u = (float(j) - width / 2.0f);
        float v = (float(height / 2.0f) - i);
        vec3 ray_direction = normalize(cam_forward * 100.0f + cam_right * u + cam_up * v);

        Ray initial_ray = {eye_position, ray_direction};
        vec3 pixel_color = castRay(initial_ray, scene_objects, light_direction, light_color, ambient_light, MAX_DEPTH);

        pixel_color = clamp(pixel_color, 0.0f, 1.0f);
        int idx = (i * width + j) * 3;
        image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
        image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
        image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
      }
    }

    // Save the frame to a PNG file
    char filename[128];
    sprintf(filename, "output/frame_%03d.png", frame);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filename, width, height, 3, image, width * 3);
  }

  std::cout << "Animation frames rendered successfully into the 'output' directory.\n";
  return 0;
}