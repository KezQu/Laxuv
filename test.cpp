#include <cmath>
#include <iostream>
struct vec3
{
  float x, y, z;
};

float p_i;
float p_j;
float m_j;
float ro_i;
float ro_j;
float density;
double ro_hash;
double d_ro;
float factor_x;
float kernel_a;
float influenceKernel;
float dt;
vec3 v_i;
vec3 v_j;
vec3 x_ij;

vec3 operator-(vec3 a, vec3 b)
{
  return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 operator-=(vec3& a, vec3 b)
{
  a = a - b;
  return a;
}

vec3 operator*(float fac, vec3 a)
{
  return vec3{a.x * fac, a.y * fac, a.z * fac};
}

float dot(vec3 a, vec3 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length(vec3 a)
{
  return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 normalize(vec3 a)
{
  float len = length(a);
  return vec3{a.x / len, a.y / len, a.z / len};
}

vec3 CalculateGradKernelWeight(vec3 points_dist)
{
  const float abs_dist = length(points_dist);
  const vec3 versor_ij = normalize(points_dist);
  const auto value = -(kernel_a / 2.) * (kernel_a - 1) *
                     pow(influenceKernel - abs_dist, kernel_a - 2) /
                     pow(influenceKernel, kernel_a) * versor_ij;
  // std::cout << "Grad: " << value.x << " " << value.y << " " << value.z
  //           << std::endl;
  return value;
}

void SolveDensityError()
{
  const float density0 = density;
  factor_x = pow(ro_i, 2) /
             (pow(length(m_j * CalculateGradKernelWeight(x_ij)), 2) +
              pow(length(m_j * CalculateGradKernelWeight(x_ij)), 2) + 1e-6);

  ro_hash = ro_i + dt * double(m_j * dot((v_i - v_j),
                                         CalculateGradKernelWeight(x_ij)));
  for (int i = 0; i < 2; i++)
  {
    p_j = p_i = (ro_hash - density0) * factor_x / pow(dt, 2);

    v_i -= dt * (m_j * (p_i / pow(ro_i, 2) + p_j / pow(ro_j, 2)) *
                 CalculateGradKernelWeight(x_ij));
    ro_hash = ro_i + dt * double(m_j * dot((v_i - v_j),
                                           CalculateGradKernelWeight(x_ij)));
  }
  d_ro = abs(ro_hash - density0) / density0;
}

int main(int argc, char const* argv[])
{
  p_i = {};
  p_j = {};
  density = ro_j = ro_i = std::atoi(argv[1]);
  m_j = argc > 2 ? std::atoi(argv[2]) : (4. / 3.) * M_PI * pow(1e-1, 3) * ro_i;
  // m_j = ro_i;
  ro_hash = {};
  d_ro = {};
  factor_x = {};
  kernel_a = {4};
  influenceKernel = {2};
  dt = {1e-3};
  v_i = {0, -1, 0};
  v_j = {0, 0, 0};
  // x_ij = {0, 1.0, 0};
  x_ij = {0, 1.948359, 0};

  SolveDensityError();
  printf("p_i: %.20f \n", p_i);
  printf("p_j: %.20f \n", p_j);
  printf("m_j: %.20f \n", m_j);
  printf("ro_i: %.20f \n", ro_i);
  printf("ro_j: %.20f \n", ro_j);
  printf("density: %.20f \n", density);
  printf("ro_hash: %.20f \n", ro_hash);
  printf("d_ro: %.20f \n", d_ro);
  printf("factor_x: %.20f \n", factor_x);
  printf("kernel_a: %.20f \n", kernel_a);
  printf("influenceKernel: %.20f \n", influenceKernel);
  printf("dt: %.20f \n", dt);
  printf("v_i: %.20f,%.20f,%.20f \n", v_i.x, v_i.y, v_i.z);
  printf("v_j: %.20f,%.20f,%.20f \n", v_j.x, v_j.y, v_j.z);
  printf("x_ij: %.20f,%.20f,%.20f \n", x_ij.x, x_ij.y, x_ij.z);
}