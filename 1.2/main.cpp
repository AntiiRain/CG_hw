// Based on templates from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <limits>
#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

//make .. -DCMAKE_TOOLCHAIN_FILE=C:\Users\lennard\.vcpkg-clion\vcpkg/scripts/buildsystems/vcpkg.cmake
//struct Sphere {
//  glm::vec3 center;   // 中心位置
//  float radius;       // 半径
//  glm::vec3 color;    // 颜色 (例如 R,G,B值范围在0.0到1.0)
//
//};
using namespace glm;
struct Material {
  glm::vec3 ambient;      // 环境光系数 (通常是物体颜色)
  glm::vec3 diffuse;      // 漫反射系数 (通常是物体颜色)
  glm::vec3 specular;     // 镜面反射系数 (通常是白色)
  float shininess;      // 高光指数
  float reflectivity;
};

struct Sphere {
  glm::vec3 center;
  float radius;
  Material material;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float interaction(const Sphere& S, const glm::vec3& ray_origin, const glm::vec3& ray_direction);
glm::vec3 castRay(
    const Ray& ray,
    const std::vector<Sphere>& scene_objects,
    const glm::vec3& light_direction,
    const glm::vec3& light_color,
    const glm::vec3& ambient_light,
    int depth // 递归深度
);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "layout (location = 2) in vec2 aTexCoord;\n"
                                 "out vec3 ourColor;\n"
                                 "out vec2 TexCoord;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos, 1.0);\n"
                                 "ourColor = aColor;\n"
                                 "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 ourColor;\n"
                                   "in vec2 TexCoord;\n"
                                   "uniform sampler2D texture1;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = texture(texture1, TexCoord);\n"
                                   "}\n\0";






int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  Sphere  test;

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // // GLEW: load all OpenGL function pointers
  glewInit();

  // build and compile the shaders
  // ------------------------------------
  // vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);




  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);


  // load and create a texture
  // -------------------------
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Create the image (RGB Array) to be displayed
  const int width  = 256; // keep it in powers of 2!
  const int height = 256; // keep it in powers of 2!
  unsigned char image[width*height*3];




  // 1. 定义场景光照
  glm::vec3 light_direction = glm::normalize(glm::vec3(0.5f, -1.0f, -1.0f)); // 平行光方向(从右上指向左下)
  glm::vec3 light_color(1.0f, 1.0f, 1.0f);     // 光源颜色(白色)
  glm::vec3 ambient_light(0.2f, 0.2f, 0.2f);   // 环境光强度(较暗的白光)


  // eye position
  glm::vec3 eye_position(0.0f, 0.0f, 100.0f); // 放在Z轴正方向，朝Z负方向看

// fov
  float viewport_height = 50.0f;
  float viewport_width = 50.0f;
  float focal_length = 100.0f; // 眼睛到视口的距离


//  std::vector<Sphere> scene_objects;
//// 第一个球：红色，高光较弱
//  Sphere sphere1;
//  sphere1.center = glm::vec3(-30.0f, 0.0f, -30.0f);
//  sphere1.radius = 25.0f;
//  sphere1.material.ambient = glm::vec3(0.8f, 0.1f, 0.1f);
//  sphere1.material.diffuse = glm::vec3(0.8f, 0.1f, 0.1f);
//  sphere1.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
//  sphere1.material.shininess = 32.0f;
//  scene_objects.push_back(sphere1);
//
//// 第二个球：蓝色，高光更强更集中
//  Sphere sphere2;
//  sphere2.center = glm::vec3(30.0f, -10.0f, -40.0f);
//  sphere2.radius = 20.0f;
//  sphere2.material.ambient = glm::vec3(0.1f, 0.1f, 0.9f);
//  sphere2.material.diffuse = glm::vec3(0.1f, 0.1f, 0.9f);
//  sphere2.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
//
//  sphere2.material.shininess = 128.0f;
//  scene_objects.push_back(sphere2);


  std::vector<Sphere> scene_objects;
  const int MAX_DEPTH = 5; // 定义最大反射次数

// 第一个球：红色，部分反射
  Sphere sphere1;
  sphere1.center = glm::vec3(-30.0f, 0.0f, -30.0f);
  sphere1.radius = 25.0f;
  sphere1.material.ambient = glm::vec3(0.8f, 0.1f, 0.1f);
  sphere1.material.diffuse = glm::vec3(0.8f, 0.1f, 0.1f);
  sphere1.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  sphere1.material.shininess = 32.0f;
  sphere1.material.reflectivity = 0.3f; // 30% 反射
  scene_objects.push_back(sphere1);

// 第二个球：蓝色，不反射
  Sphere sphere2;
  sphere2.center = glm::vec3(30.0f, -5.0f, -40.0f);
  sphere2.radius = 20.0f;
  sphere2.material.ambient = glm::vec3(0.1f, 0.1f, 0.9f);
  sphere2.material.diffuse = glm::vec3(0.1f, 0.1f, 0.9f);
  sphere2.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  sphere2.material.shininess = 128.0f;
  sphere2.material.reflectivity = 0.0f; // 0% 反射
  scene_objects.push_back(sphere2);

// 第三个物体：一个大的球体作为镜面地板
  Sphere floor_sphere;
  floor_sphere.center = glm::vec3(0.0f, 500.0f, -50.0f);
  floor_sphere.radius = 475.0f;
  floor_sphere.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  floor_sphere.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
  floor_sphere.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
  floor_sphere.material.shininess = 64.0f;
  floor_sphere.material.reflectivity = 0.8f; // 80% 反射，非常像镜子
  scene_objects.push_back(floor_sphere);


// ====================================================================
// =================== 更新后的主渲染循环 ===============================
// ====================================================================
  for(int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {

      // --- 1. 透视相机光线生成 ---
      float u = (float(j) - width / 2.0f) * (viewport_width / width);
      float v = (float(height / 2.0f) - i) * (viewport_height / height);
      vec3 ray_origin = eye_position;
      vec3 viewport_pixel_pos(u, v, eye_position.z - focal_length);
      vec3 ray_direction = glm::normalize(viewport_pixel_pos - ray_origin);

      Ray initial_ray = {ray_origin, ray_direction};

      // --- 2. 调用 castRay ---
      // 渲染逻辑的核心现在都封装在 castRay 函数里了
      vec3 pixel_color = castRay(initial_ray, scene_objects, light_direction, light_color, ambient_light, MAX_DEPTH);

      // --- 3. 写入图像缓存 ---
      pixel_color = glm::clamp(pixel_color, 0.0f, 1.0f);
      int idx = (i * width + j) * 3;
      image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
      image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
      image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
    }
  }


//  for(int i = 0; i < height; i++) {
//    for (int j = 0; j < width; j++) {
//
//      // --- 1. 透视相机光线生成 (已完成) ---
//      float u = (float(j) - width / 2.0f) * (viewport_width / width);
//      float v = (float(height / 2.0f) - i) * (viewport_height / height);
//
//      glm::vec3 ray_o = eye_position;
//      glm::vec3 viewport_pixel_pos(u, v, eye_position.z - focal_length);
//      glm::vec3 ray_d = glm::normalize(viewport_pixel_pos - ray_o);
//
//      // --- 2. 光线与场景求交 (已完成) ---
//      float closest_t = std::numeric_limits<float>::max();
//      const Sphere* hit_sphere = nullptr; // 用于记录击中了哪个球体
//
//      for (const auto& sphere : scene_objects) {
//        float t = interaction(sphere, ray_o, ray_d);
//        if (t > 0.001f && t < closest_t) {
//          closest_t = t;
//          hit_sphere = &sphere;
//        }
//      }
//
//      // --- 3. 着色 (Shading) ---
//      glm::vec3 pixel_color;
//      if (hit_sphere != nullptr) {
//        // ================================================================
//        // TODO: 在这里实现你的 Phong Shading 计算
//        // ================================================================
//        //
//        // **你需要用到的变量:**
//        // - ray_o:           光线起点
//        // - ray_d:           光线方向
//        // - closest_t:       光线到最近交点的距离
//        // - hit_sphere:      指向被击中的球体的指针 (用 hit_sphere->... 来访问其属性)
//        // - eye_position:    相机/眼睛的位置
//        // - light_direction: 光源方向
//        // - light_color:     光源颜色
//        // - ambient_light:   环境光强度
//
//        // **实现步骤:**
//
//        // a. intersect
//        // 公式: P = ray_o + closest_t * ray_d;
//        // 公式: N = normalize(P - hit_sphere->center);
//        glm::vec3 hit_point = ray_o + closest_t * ray_d;
//        glm::vec3 normal = glm::normalize(hit_point - hit_sphere->center);
//        // b.  View direction
//        //opposite direction whit ray_d
//        glm::vec3 view_dir = -ray_d;
//        // c. Ambient
//        glm::vec3 ambient =  hit_sphere->material.ambient * ambient_light; //phong simulation
//
//        // d.  Lambertian (Diffuse)
//        float diff_factor = std::max(glm::dot(normal, -light_direction), 0.0f);
//        glm::vec3 diffuse = hit_sphere->material.diffuse * diff_factor * light_color;
//        // e. 计算镜面反射 (Specular)
//        glm::vec3 reflect_dir = glm::reflect(light_direction, normal);
//        float spec_factor = std::pow(std::max(glm::dot(view_dir, reflect_dir), 0.0f), hit_sphere->material.shininess);
//        glm::vec3 specular = hit_sphere->material.specular * spec_factor * light_color;
//
//
//        pixel_color = ambient + diffuse + specular;
//
//
//      } else {
//        // 如果光线什么都没击中，显示背景色 (已完成)
//        pixel_color = glm::vec3(0.1f, 0.15f, 0.2f);
//      }
//
//      // --- 4. 写入图像缓存 (已完成) ---
//      // 防止颜色值超出范围
//      pixel_color = glm::clamp(pixel_color, 0.0f, 1.0f);
//
//      int idx = (i * width + j) * 3;
//      image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
//      image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
//      image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
//    }
//  }
//

//  for(int i = 0; i < height; i++)
//  {
//    for (int j = 0; j < width; j++)
//    {
//      // --- 将像素坐标(j,i)映射到[-1, 1]的标准化平面 ---
//      float world_x = 4*(j - width / 2.0f) / (width / 2.0f);
//      float world_y = 4*-(i - height / 2.0f) / (height / 2.0f);
//
//      // 使用标准化坐标生成光线起点
//      vec3 ray_o = camera_viewpoint + vec3(world_x, world_y, 0);
//      // ---------------------------------------------------
//
//      // (后面的相交和颜色设置逻辑保持不变)
//      vec3 pixel_color(0.1f, 0.1f, 0.1f);
//      float closest_t = std::numeric_limits<float>::max();
//
//      float t1 = interaction(sphere1, ray_o, camera_direction);
//      if (t1 > 0.0f && t1 < closest_t) {
//        closest_t = t1;
//        pixel_color = sphere1.color;
//      }
//
//      // 之后您可以取消对 sphere2 的注释来看到两个球体
//       float t2 = interaction(sphere2, ray_o, camera_direction);
//       if (t2 > 0.0f && t2 < closest_t) {
//           closest_t = t2;
//           pixel_color = sphere2.color;
//       }
//
//      int idx = (i * width + j) * 3;
//      image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
//      image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
//      image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
//    }
//  }

  unsigned char *data = &image[0];
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }


  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind Texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // render container
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}



float interaction(const Sphere& S, const glm::vec3& ray_origin, const glm::vec3& ray_direction) {
  glm::vec3 oc = ray_origin - S.center;
  float a = dot(ray_direction, ray_direction);
  float b = 2.0f * dot(oc, ray_direction);
  float c = dot(oc, oc) - S.radius * S.radius;

  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0f;
  } else {
    // 求解一元二次方程
    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
    if (t0 > 0.001f && t0 < t1) {
      return t0;
    }
    if (t1 > 0.001f) {
      return t1;
    }
    return -1.0f;
  }

}



glm::vec3 castRay(
    const Ray& ray,
    const std::vector<Sphere>& scene_objects,
    const glm::vec3& light_direction,
    const glm::vec3& light_color,
    const glm::vec3& ambient_light,
    int depth // 递归深度
) {
  // 1. 递归深度检查：如果反射次数过多，则停止，返回黑色
  if (depth <= 0) {
    return glm::vec3(0.0f, 0.0f, 0.0f);
  }

  // 2. 寻找最近的交点 (逻辑不变)
  float closest_t = std::numeric_limits<float>::max();
  const Sphere* hit_sphere = nullptr;

  for (const auto& sphere : scene_objects) {
    float t = interaction(sphere, ray.origin, ray.direction);
    if (t > 0.001f && t < closest_t) {
      closest_t = t;
      hit_sphere = &sphere;
    }
  }

  // 3. 判断并计算颜色
  if (hit_sphere != nullptr) {
    // 如果光线击中了物体

    // a. 计算局部光照颜色 (Phong Shading)
    glm::vec3 hit_point = ray.origin + closest_t * ray.direction;
    glm::vec3 normal = glm::normalize(hit_point - hit_sphere->center);
    glm::vec3 view_dir = -ray.direction; // 直接对光线方向取反

    glm::vec3 ambient = hit_sphere->material.ambient * ambient_light;
    float diff_factor = std::max(glm::dot(normal, -light_direction), 0.0f);
    glm::vec3 diffuse = hit_sphere->material.diffuse * diff_factor * light_color;
    glm::vec3 reflect_dir_light = glm::reflect(light_direction, normal);
    float spec_factor = std::pow(std::max(glm::dot(view_dir, reflect_dir_light), 0.0f), hit_sphere->material.shininess);
    glm::vec3 specular = hit_sphere->material.specular * spec_factor * light_color;

    glm::vec3 local_color = ambient + diffuse + specular;

    // b. 如果是反射材质，计算反射颜色并混合
    float reflect_amount = hit_sphere->material.reflectivity;
    if (reflect_amount > 0) {
      glm::vec3 reflect_dir_view = glm::reflect(ray.direction, normal);
      // 创建新的反射光线，起点稍微离开表面以防自相交
      Ray reflected_ray = {hit_point + normal * 0.001f, reflect_dir_view};

      // **递归调用**
      glm::vec3 reflected_color = castRay(reflected_ray, scene_objects, light_direction, light_color, ambient_light, depth - 1);

      // c. 将局部颜色和反射颜色按反射度混合
      return local_color * (1.0f - reflect_amount) + reflected_color * reflect_amount;
    } else {
      // 如果不是反射材质，直接返回局部颜色
      return local_color;
    }
  } else {
    // 如果没有击中任何物体，返回背景色
    return glm::vec3(0.1f, 0.15f, 0.2f);
  }
}
