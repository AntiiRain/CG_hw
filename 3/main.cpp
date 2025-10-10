#include <iostream>
#include <cmath> // For std::abs

// A simple structure to hold 2D coordinates
struct Point2D {
  float x, y;
};

/**
 * @brief Calculates the area of a 2D triangle given its three vertices.
 * * This function implements the formula based on the determinant of the
 * coordinates, as specified in the assignment.
 * Area = 0.5 * | (x0(y1-y2) + x1(y2-y0) + x2(y0-y1)) |
 * * @param p0 The first vertex of the triangle.
 * @param p1 The second vertex of the triangle.
 * @param p2 The third vertex of the triangle.
 * @return The area of the triangle.
 */
float calculateTriangleArea(Point2D p0, Point2D p1, Point2D p2) {
  // Calculate the value of the determinant:
  // | x0  x1  x2 |
  // | y0  y1  y2 |
  // | 1   1   1  |
  // This is equivalent to x0(y1 - y2) + x1(y2 - y0) + x2(y0 - y1)
  float determinant = p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y);

  // The area is half the absolute value of the determinant.
  return 0.5f * std::abs(determinant);
}

int main() {
  // Define the three vertices of a triangle
  Point2D v0 = {0.0f, 0.0f};
  Point2D v1 = {5.0f, 0.0f};
  Point2D v2 = {0.0f, 2.0f};

  // Calculate the area
  float area = calculateTriangleArea(v0, v1, v2);

  // Print the result
  std::cout << "The coordinates of the vertices are:" << std::endl;
  std::cout << "p0: (" << v0.x << ", " << v0.y << ")" << std::endl;
  std::cout << "p1: (" << v1.x << ", " << v1.y << ")" << std::endl;
  std::cout << "p2: (" << v2.x << ", " << v2.y << ")" << std::endl;
  std::cout << "The area of the triangle is: " << area << std::endl; // Expected: 0.5 * base * height = 0.5 * 5 * 2 = 5

  return 0;
}