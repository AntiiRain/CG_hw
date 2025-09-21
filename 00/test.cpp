#include <iostream>
#include <string>
#include <glm/glm.hpp>

// 使用 glm 的类型别名
using glm::vec3;
using glm::dot;
using glm::normalize; // 用于向量归一化

// 定义球体结构体 (确保与您的主程序一致)
struct Sphere {
  vec3 center;
  float radius;
  vec3 color;
};

// --- 在这里粘贴上面那个修正并改进后的 interaction 函数 ---
float interaction(const Sphere& S, const vec3& ray_origin, const vec3& ray_direction) {
  vec3 oc = ray_origin - S.center;
  float a = dot(ray_direction, ray_direction);
  float b = 2.0f * dot(oc, ray_direction);
  float c = dot(oc, oc) - S.radius * S.radius;

  float discriminant = b * b - 4 * a * c;

  // 如果判别式小于0，说明没有实数解，即不相交
  if (discriminant < 0) {
    return -1.0f;
  } else {
    // 求解一元二次方程
    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);

    // 我们需要的是在相机前方(t>0)且最近的交点
    if (t0 > 0.001f && t0 < t1) { // t0 通常是较小的那个解
      return t0;
    }
    if (t1 > 0.001f) {
      return t1;
    }

    // 如果两个交点都在相机背后，也算作未击中
    return -1.0f;
  }
}

// ---------------------------------------------------------

// 一个辅助函数，用于运行单个测试并打印结果
void run_test(const std::string& test_name, const Sphere& s, const vec3& origin, const vec3& dir, bool should_hit) {
  std::cout << "--- " << test_name << " ---" << std::endl;
  // 确保方向向量是单位向量，这对于求交公式很重要
  float t = interaction(s, origin, normalize(dir));
  bool hit = (t > 0.0f);

  if (hit) {
    std::cout << "结果: 击中! 交点距离 t = " << t << std::endl;
  } else {
    std::cout << "结果: 未击中" << std::endl;
  }

  if (hit == should_hit) {
    std::cout << "状态: 通过 (PASS)" << std::endl;
  } else {
    std::cout << "状态: 失败 (FAIL)" << std::endl;
  }
  std::cout << std::endl;
}

int main() {
  // 1. 创建一个用于测试的球体
  //    位置在(0,0,-5)，半径为1
  Sphere test_sphere;
  test_sphere.center = vec3(0, 0, -5);
  test_sphere.radius = 1.0f;
  std::cout << "测试球体: 中心(0,0,-5), 半径1.0\n" << std::endl;

  // 2. 运行一系列测试用例
  //    用例1: 从原点(0,0,0)发射光线，直击球心。预期：击中
  run_test("直击中心", test_sphere, vec3(0, 0, 0), vec3(0, 0, -1), true);

  //    用例2: 从很远的地方发射光线，明显未击中。预期：未击中
  run_test("明显未击中", test_sphere, vec3(5, 5, 0), vec3(0, 0, -1), false);

  //    用例3: 擦边球，光线与球体相切。预期：击中
  run_test("擦边击中", test_sphere, vec3(1, 0, 0), vec3(0, 0, -1), true);

  //    用例4: 球在光线起点背后。预期：未击中（因为t值为负）
  run_test("球在背后", test_sphere, vec3(0, 0, 0), vec3(0, 0, 1), false);

  //    用例5: 光线起点在球体内。预期：击中（会得到一个小的正数t值）
  run_test("起点在球内", test_sphere, vec3(0, 0, -5), vec3(0, 0, -1), true);

  return 0;
}