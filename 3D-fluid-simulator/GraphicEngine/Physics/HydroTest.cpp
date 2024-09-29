#include "HydroTest.h"

#include "glm/glm.hpp"

using namespace glm;

void HydroTest::FindNeighbours(uint32_t index, uint32_t MaxParticles)
{
  const vec3 x_i = vec3(particle[index].position.x, particle[index].position.y,
                        particle[index].position.z);
  for (int j = 0; j < Essentials::MaxNeighbours; j++)
  {
    particle[index].neighbours[j] = MaxValueNeighbour;
  }
  uint32_t neighbourCount = 0;
  for (uint32_t j = 0;
       j < MaxParticles && neighbourCount < Essentials::MaxNeighbours; j++)
  {
    const vec3 x_j = vec3(particle[j].position.x, particle[j].position.y,
                          particle[j].position.z);
    if (distance(x_i, x_j) <= 2 * Essentials::KernelRadius && (index != j))
    {
      particle[index].neighbours[neighbourCount] = uint16_t(j);
      neighbourCount++;
    }
  }
}

mat3 HydroTest::GetRotationZMatrix(float alpha_z)
{
  return mat3(cos(alpha_z), -sin(alpha_z), 0, sin(alpha_z), cos(alpha_z), 0, 0,
              0, 1);
}
mat3 HydroTest::GetRotationYMatrix(float alpha_y)
{
  return mat3(cos(alpha_y), 0, sin(alpha_y), 0, 1, 0, -sin(alpha_y), 0,
              cos(alpha_y));
}

float HydroTest::CalculateKernelWeight(vec3 x)
{
  const float b = 0.4f;
  const float abs_x = length(x);
  return pow(E, -pow(abs_x / Essentials::KernelRadius, 2. / 3.) / (2 * b * b)) /
         pow(2 * PI * b * b, 1. / 2.);
}

vec3 HydroTest::CalculateFrameVelocity(uint32_t index_i, uint32_t index_j)
{
  const vec3 v_i =
      vec3(particle[index_i].velocity.x, particle[index_i].velocity.y,
           particle[index_i].velocity.z);
  const vec3 v_j =
      vec3(particle[index_j].velocity.x, particle[index_j].velocity.y,
           particle[index_j].velocity.z);
  const vec3 x_i =
      vec3(particle[index_i].position.x, particle[index_i].position.y,
           particle[index_i].position.z);
  const vec3 x_j =
      vec3(particle[index_j].position.x, particle[index_j].position.y,
           particle[index_j].position.z);
  const vec3 x_ij =
      vec3((x_i.x + x_j.x) / 2., (x_i.y + x_j.y) / 2., (x_i.z + x_j.z) / 2.);

  return v_i +
         (v_j - v_i) *
             vec3(((x_ij - x_i) * (x_j - x_i)).x / pow(distance(x_j, x_i), 2),
                  ((x_ij - x_i) * (x_j - x_i)).y / pow(distance(x_j, x_i), 2),
                  ((x_ij - x_i) * (x_j - x_i)).z / pow(distance(x_j, x_i), 2));
}

float HydroTest::CalculateOmega(uint32_t index_x)
{
  float omega = CalculateKernelWeight(
      vec3(particle[index_x].position.x, particle[index_x].position.y,
           particle[index_x].position.z) -
      vec3(particle[index_x].position.x, particle[index_x].position.y,
           particle[index_x].position.z));
  for (uint32_t j = 0; j < Essentials::MaxNeighbours &&
                       particle[index_x].neighbours[j] != MaxValueNeighbour;
       j++)
  {
    const uint32_t neighbour_index = particle[index_x].neighbours[j];
    omega += CalculateKernelWeight(vec3(particle[index_x].position.x,
                                        particle[index_x].position.y,
                                        particle[index_x].position.z) -
                                   vec3(particle[neighbour_index].position.x,
                                        particle[neighbour_index].position.y,
                                        particle[neighbour_index].position.z));
  }
  return omega;
}

float HydroTest::CalculatePsi(uint32_t index_i, uint32_t index_x)
{
  return CalculateKernelWeight(
             vec3(particle[index_x].position.x, particle[index_x].position.y,
                  particle[index_x].position.z) -
             vec3(particle[index_i].position.x, particle[index_i].position.y,
                  particle[index_i].position.z)) /
         CalculateOmega(index_x);
}

vec3 HydroTest::CalculateNearPsi(const uint32_t index_i, const uint32_t index_x)
{
  mat3 E_x = mat3(0);
  for (uint32_t j = 0; j < Essentials::MaxNeighbours &&
                       particle[index_x].neighbours[j] != MaxValueNeighbour;
       j++)
  {
    const uint32_t neighbour_index = particle[index_x].neighbours[j];
    const vec3 neighbour_x_diff =
        vec3(particle[neighbour_index].position.x,
             particle[neighbour_index].position.y,
             particle[neighbour_index].position.z) -
        vec3(particle[index_x].position.x, particle[index_x].position.y,
             particle[index_x].position.z);
    E_x += outerProduct(neighbour_x_diff, neighbour_x_diff) *
           CalculatePsi(neighbour_index, index_x);
  }
  const mat3 B_x = inverse(E_x);
  const vec3 nearPsi_j =
      B_x *
      (vec3(particle[index_i].position.x, particle[index_i].position.y,
            particle[index_i].position.z) -
       vec3(particle[index_x].position.x, particle[index_x].position.y,
            particle[index_x].position.z)) *
      CalculatePsi(index_i, index_x);
  return nearPsi_j;
}

mat3 HydroTest::CalculateGradW(const uint32_t index_x, const Vector Wp)
{
  mat3 gradWp = mat3(0);

  for (uint32_t j = 0; j < Essentials::MaxNeighbours &&
                       particle[index_x].neighbours[j] != MaxValueNeighbour;
       j++)
  {
    const uint32_t neighbour_index = particle[index_x].neighbours[j];
    const vec3 nearPsi_i = CalculateNearPsi(neighbour_index, index_x);
    const Vector Wp_temp_j =
        Vector(particle[neighbour_index].VolumeDensityPressureMass.y,
               vec3(particle[neighbour_index].velocity.x,
                    particle[neighbour_index].velocity.y,
                    particle[neighbour_index].velocity.z) -
                   CalculateFrameVelocity(index_x, neighbour_index),
               particle[neighbour_index].VolumeDensityPressureMass.z);

    gradWp[0] += (Wp_temp_j.x - Wp.x) * nearPsi_i;
    gradWp[1] += (Wp_temp_j.y - Wp.y) * nearPsi_i;
    gradWp[2] += (Wp_temp_j.z - Wp.z) * nearPsi_i;
  }
  return gradWp;
}

Vector HydroTest::CalculateTimeDerivativeOfW(const uint32_t index_x,
                                             const Vector Wp)
{
  mat3 gradWp = CalculateGradW(index_x, Wp);
  Vector dW_dt = Vector(0, vec3(0), 0);
  const vec3 v =
      vec3(particle[index_x].velocity.x, particle[index_x].velocity.y,
           particle[index_x].velocity.z);
  const float density = particle[index_x].VolumeDensityPressureMass.y;
  const float pressure = particle[index_x].VolumeDensityPressureMass.z;

  dW_dt.x = -(dot(v, gradWp[0]) + dot(vec3(density), gradWp[1]));
  dW_dt.y = -(v * gradWp[1] + gradWp[2] / density);
  dW_dt.z = -(dot(vec3(gamma * density), gradWp[1]) + dot(v, gradWp[2]));

  return dW_dt;
}

Vector HydroTest::PrepareRiemmanProblemSide(uint32_t index_x, vec3 v_frame,
                                            vec3 x_mean_x, vec3* dir_vector)
{
  Vector W_x =
      Vector(particle[index_x].VolumeDensityPressureMass.y,
             vec3(particle[index_x].velocity.x, particle[index_x].velocity.y,
                  particle[index_x].velocity.z),
             particle[index_x].VolumeDensityPressureMass.z);

  Vector Wp_x = Vector(W_x.x, W_x.y - v_frame, W_x.z);
  mat3 gradW_x = CalculateGradW(index_x, Wp_x);

  Vector gradCrossW_x = Vector(dot(gradW_x[0], x_mean_x), gradW_x[1] * x_mean_x,
                               dot(gradW_x[2], x_mean_x));

  Vector dW_dt_x = CalculateTimeDerivativeOfW(index_x, Wp_x);

  Vector Wpp_x = Vector(0, vec3(0), 0);
  Wpp_x.x = Wp_x.x + gradCrossW_x.x + dW_dt_x.x * dt / 2.f;
  Wpp_x.y = Wp_x.y + gradCrossW_x.y + dW_dt_x.y * dt / 2.f;
  Wpp_x.z = Wp_x.z + gradCrossW_x.z + dW_dt_x.z * dt / 2.f;

  *dir_vector = Wpp_x.y;
  Vector Wppp_x = Vector(Wpp_x.x, vec3(length(Wpp_x.y), 0, 0), Wpp_x.z);

  return Wppp_x;
}

Flux HydroTest::CalculateReimannProblem(uint32_t index_i, uint32_t index_j)
{
  /////////////////////////////////////PREPARATION//////////////////////////////////////////
  vec3 x_i = vec3(particle[index_i].position.x, particle[index_i].position.y,
                  particle[index_i].position.z);
  vec3 x_j = vec3(particle[index_j].position.x, particle[index_j].position.y,
                  particle[index_j].position.z);
  vec3 x_ij = vec3((x_i + x_j).x / 2., (x_i + x_j).y / 2., (x_i + x_j).z / 2.);
  vec3 v_frameij = CalculateFrameVelocity(index_i, index_j);

  vec3 x_ij_i = x_ij - x_i;
  vec3 x_ij_j = x_ij - x_j;

  vec3 dir_vector_r = vec3(0);
  Vector Wppp_r =
      PrepareRiemmanProblemSide(index_i, v_frameij, x_ij_i, &dir_vector_r);

  vec3 dir_vector_l = vec3(0);
  Vector Wppp_l =
      PrepareRiemmanProblemSide(index_j, v_frameij, x_ij_j, &dir_vector_l);
  /////////////////////////////////////HLLC
  ///SOLVER//////////////////////////////////////////
  float ro_r = Wppp_r.x;
  float u_r = Wppp_r.y.x;
  float p_r = Wppp_r.z;
  float ro_l = Wppp_l.x;
  float u_l = Wppp_l.y.x;
  float p_l = Wppp_l.z;

  float b = 0.f;

  float E_r = ro_r * u_r * u_r / 2. +
              p_r * (1 - b * ro_r) / (gamma - 1.);  // for internal energy;
  float E_l = ro_l * u_l * u_l / 2. + p_l * (1 - b * ro_l) / (gamma - 1.);
  vec3 U_r = vec3(ro_r, ro_r * u_r, E_r);
  vec3 U_l = vec3(ro_l, ro_l * u_l, E_l);
  vec3 F_r = vec3(ro_r * u_r, ro_r * u_r * u_r + p_r, u_r * (E_r + p_r));
  vec3 F_l = vec3(ro_l * u_l, ro_l * u_l * u_l + p_l, u_l * (E_l + p_l));

  float a_mean = 1500.;
  float ro_mean = (ro_l + ro_r) / 2.;

  float p_hash = (p_l + p_r) / 2. - (ro_mean * a_mean * (u_r - u_l)) / 2.;
  float u_hash = (u_l + u_r) / 2. - (p_r - p_l) / (2. * ro_mean * a_mean);

  float q_l = p_hash / p_l <= 1
                  ? 1
                  : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_l - 1));
  float q_r = p_hash / p_r <= 1
                  ? 1
                  : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_r - 1));

  float S_l = u_l - a_mean * q_l;
  float S_r = u_r + a_mean * q_r;
  float S_m = u_hash;

  vec3 Q = S_l * U_l - F_l;
  vec3 R = S_r * U_r - F_r;

  float ro_l_hash = Q.x / (S_l - S_m);
  float ro_r_hash = R.x / (S_r - S_m);

  float p_l_hash = S_m * Q.x - Q.y;
  float p_r_hash = S_m * R.x - R.y;

  vec3 U_l_hash =
      vec3(ro_l_hash, ro_l_hash * u_hash, (Q.z + S_m * p_l_hash) / (S_l - S_m));
  vec3 U_r_hash =
      vec3(ro_r_hash, ro_r_hash * u_hash, (R.z + S_m * p_r_hash) / (S_r - S_m));

  vec3 F_l_hash = vec3(0);
  vec3 F_r_hash = vec3(0);
  if (S_l <= 0 && S_r >= 0)
  {
    F_r_hash = S_r * U_r_hash - R;
    F_l_hash = S_l * U_l_hash - Q;
  }
  else if (S_l <= S_m && S_m <= S_r && S_r <= 0)
  {
    F_l_hash = S_l * U_l_hash - Q;
    F_r_hash = (S_m * S_r * (U_r - U_l) + S_m * S_r * F_l / S_l - S_m * F_r +
                S_r * (1 - S_m / S_l) * F_l_hash) /
               (S_r - S_m);
  }
  else if (S_l >= 0 && S_m >= S_l && S_r >= S_m)
  {
    F_r_hash = S_r * U_r_hash - R;
    F_l_hash = S_l * U_l_hash - Q;
    F_l_hash = (S_m * S_l * (U_r - U_l) + S_m * S_l * F_r / S_r - S_m * F_l +
                S_l * (1 - S_m / S_r) * F_r_hash) /
               (S_m - S_l);
  }

  float CourantNumbers[5] = {-1, dt * S_l / length(x_i - x_j),
                             dt * S_m / length(x_i - x_j),
                             dt * S_r / length(x_i - x_j), 1};
  vec3 F_i2_k[5] = {vec3(0), F_l, F_l_hash, F_r_hash, F_r};
  vec3 F_i2 = vec3(0);
  for (uint32_t k = 1; k < 5; k++)
  {
    F_i2 += vec3((CourantNumbers[k] - CourantNumbers[k - 1]) / 2.) * F_i2_k[k];
  }
  /////////////////////////////////////FLUX IJ
  ///DEBOOST//////////////////////////////////////////
  const vec3 x_versor = vec3(1, 0, 0);
  const vec3 dir_vector_m = dir_vector_r + dir_vector_l;
  Flux Fp_ij = Flux(vec3(0), mat3(0), vec3(0));
  if (length(dir_vector_m) > 1e-6f)
  {
    const vec3 dir_versor_r = normalize(dir_vector_r);
    const vec3 dir_versor_l = normalize(dir_vector_l);

    const vec3 dir_versor_m = normalize(dir_vector_m);
    vec3 rot_m = cross(dir_versor_m, x_versor);
    rot_m = length(rot_m) > 0 ? normalize(rot_m) : rot_m;
    const float cos_angle_m = dot(dir_versor_m, x_versor);
    const mat3 rot_helper_m =
        mat3(0, -rot_m.z, rot_m.y, rot_m.z, 0, -rot_m.x, -rot_m.y, rot_m.x, 0);
    const mat3 rotMtx_m = I +
                          sqrt(1 - cos_angle_m * cos_angle_m) * rot_helper_m +
                          (1 - cos_angle_m) * rot_helper_m * rot_helper_m;
    vec3 test_m = dir_vector_m * rotMtx_m;
    Fp_ij = Flux(
        dir_versor_m * F_i2.x,
        outerProduct(dir_versor_m, (vec3(F_i2.y, 0, 0) * inverse(rotMtx_m))),
        dir_versor_m * F_i2.z);
  }
  Flux F_ij = Flux(Fp_ij.x, Fp_ij.y + outerProduct(v_frameij, Fp_ij.x),
                   Fp_ij.z + float(pow(length(v_frameij), 2)) * Fp_ij.x / 2.f +
                       v_frameij * Fp_ij.y);

  return F_ij;
}

Vector HydroTest::GenerateHydrodynamics(uint32_t index_i)
{
  vec3 vel_i = vec3(particle[index_i].velocity.x, particle[index_i].velocity.y,
                    particle[index_i].velocity.z);
  Vector U_n =
      Vector(particle[index_i].VolumeDensityPressureMass.y,
             particle[index_i].VolumeDensityPressureMass.y * vel_i,
             particle[index_i].VolumeDensityPressureMass.z / (gamma - 1.) +
                 particle[index_i].VolumeDensityPressureMass.y *
                     pow(length(vel_i), 2) / 2);
  float V_i = particle[index_i].VolumeDensityPressureMass.x;

  Vector Q_n = Vector(U_n.x * V_i, U_n.y * V_i, U_n.z * V_i);

  Vector Aij_Fij = Vector(0, vec3(0), 0);
  for (int j = 0; j < Essentials::MaxNeighbours &&
                  particle[index_i].neighbours[j] != MaxValueNeighbour;
       j++)
  {
    uint32_t neighbour_index = particle[index_i].neighbours[j];
    Flux F_ij = CalculateReimannProblem(index_i, neighbour_index);
    vec3 nearPsi_ji = CalculateNearPsi(neighbour_index, index_i);
    vec3 nearPsi_ij = CalculateNearPsi(index_i, neighbour_index);
    float V_j = particle[neighbour_index].VolumeDensityPressureMass.x;

    Aij_Fij.x += dot(F_ij.x, (V_i * nearPsi_ji - V_j * nearPsi_ij));
    Aij_Fij.y += F_ij.y * (V_i * nearPsi_ji - V_j * nearPsi_ij);
    Aij_Fij.z += dot(F_ij.z, (V_i * nearPsi_ji - V_j * nearPsi_ij));
  }
  Vector Q_n1;
  Q_n1.x = Q_n.x - dt * Aij_Fij.x;
  Q_n1.y = Q_n.y - dt * Aij_Fij.y;
  Q_n1.z = Q_n.z - dt * Aij_Fij.z;

  return Q_n1;
}
