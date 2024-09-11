#pragma once

#include "glm/glm.hpp"
#include <PhysicsDispatch.h>
#include <imgui.h>

constexpr uint32_t maxNeighbours = 64;

PhysicsProperties particle[maxNeighbours];

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;

using namespace glm;

struct Vector {
    float x;
    vec3 y;
    float z;
};
struct GradVector {
    vec3 x;
    mat3 y;
    vec3 z;
};

float b = 0.f;
float gamma = 1.4f;

vec4 Qube(uint32_t idx, uint32_t maxParticles) {
    float meshSize = pow(maxParticles, 1 / 3.);
    return vec4(
        mod(floor(idx / meshSize), meshSize),
        mod(float(idx), meshSize),
        floor(idx / (meshSize * meshSize)),
        0.f);
}

void FindNeighbours(uint32_t index, uint32_t MaxParticles) {
    vec3 x_i = vec3(particle[index].position.x, particle[index].position.y, particle[index].position.z);
    uint32_t neighbourCount = 0;
    for (uint32_t j = 0; j < MaxParticles; j++) {
        vec3 x_j = vec3(particle[j].position.x, particle[j].position.y, particle[j].position.z);
        if (distance(x_i, x_j) <= 2 * KernelRadius && (index != j)) {
            particle[index].neighbours[neighbourCount] = j;
            neighbourCount++;
        }
    }
}

PhysicsProperties* Init() {
    for (int i = 0; i < maxNeighbours; i++)
    {
        particle[i].position = Qube(i, maxNeighbours);
        for (uint32_t j = 0; j < MaxNeighbours; j++) {
            particle[i].neighbours[j] = 0xffffffff;
        }
    }
    for (int i = 0; i < maxNeighbours; i++)
    {
        FindNeighbours(i, maxNeighbours);
    }
    return particle;
}

mat3 GetRotationZMatrix(float alpha_z) {
    return mat3(cos(alpha_z), -sin(alpha_z), 0,
        sin(alpha_z), cos(alpha_z), 0,
        0, 0, 1);
}
mat3 GetRotationYMatrix(float alpha_y) {
    return mat3(cos(alpha_y), 0, sin(alpha_y),
        0, 1, 0,
        -sin(alpha_y), 0, cos(alpha_y));
}

float CalculateKernelWeight(vec3 x) {
    const float b = 0.6;
    return (pow(E, -(x.x + x.y + x.z) / (2 * b * b * KernelRadius * KernelRadius)) * KernelRadius) / pow(2 * PI * b, 3 / 2);
}

vec3 CalculateFrameVelocity(uint32_t index_i, uint32_t index_j) {
    vec3 v_i = vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z);
    vec3 v_j = vec3(particle[index_j].velocity.x, particle[index_j].velocity.y, particle[index_j].velocity.z);
    vec3 x_i = vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z);
    vec3 x_j = vec3(particle[index_j].position.x, particle[index_j].position.y, particle[index_j].position.z);
    vec3 x_ij = vec3((x_i.x + x_j.x) / 2., (x_i.y + x_j.y) / 2., (x_i.z + x_j.z) / 2.);

    return v_i + (v_j - v_i) * vec3(
        ((x_ij - x_i) * (x_j - x_i)).x / pow(distance(x_j, x_i), 2),
        ((x_ij - x_i) * (x_j - x_i)).y / pow(distance(x_j, x_i), 2),
        ((x_ij - x_i) * (x_j - x_i)).z / pow(distance(x_j, x_i), 2));
}

float CalculateOmega(uint32_t index_x) {

    float omega = 0;
    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        omega += CalculateKernelWeight(vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) -
                                       vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z));
    }
    return omega;
}

float CalculatePsi(uint32_t index_i, uint32_t index_x) {
    return CalculateKernelWeight(vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) - 
                                 vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z)) / CalculateOmega(index_x);
}

vec3 CalculateNearPsi(uint32_t index_i, uint32_t index_x) {

    mat3 E = mat3(0);
    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        E += outerProduct(vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z) - 
                               vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z),
                               vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z) - 
                               vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z)) *
            CalculatePsi(neighbour_index, index_x);
    }
    mat3 B = inverse(E);
    vec3 nearPsi_j = B * (vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z) - vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z)) * CalculatePsi(index_i, index_x);
    return nearPsi_j;
}

GradVector CalculateGradW(uint32_t index_x, Vector Wp) {
    GradVector gradWp = GradVector(vec3(0), mat3(0), vec3(0));

    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        Vector Wp_temp_j = Vector(particle[neighbour_index].VolumeDensityPressureMass.y,
            vec3(particle[neighbour_index].velocity.x, particle[neighbour_index].velocity.y, particle[neighbour_index].velocity.z) - CalculateFrameVelocity(index_x, neighbour_index),
            particle[neighbour_index].VolumeDensityPressureMass.z);
        vec3 nearPsi_i = CalculateNearPsi(neighbour_index, index_x);
        gradWp.x += (Wp_temp_j.x - Wp.x) * nearPsi_i;
        gradWp.y += outerProduct((Wp_temp_j.y - Wp.y), nearPsi_i);
        gradWp.z += (Wp_temp_j.z - Wp.z) * nearPsi_i;
        //for(uint32_t dim = 0; dim < 3; dim ++){
        //	gradW_i[dim].x += (Wp_temp_j.x - Wp_i.x) * nearPsi_i[dim];
        //	gradW_i[dim].y += (Wp_temp_j.y - Wp_i.y) * nearPsi_i[dim];
        //	gradW_i[dim].z += (Wp_temp_j.z - Wp_i.z) * nearPsi_i[dim];
        //}
    }
    return gradWp;
}

Vector CalculateTimeDerivativeOfW(uint32_t index_x, Vector Wp) {
    GradVector gradWp = CalculateGradW(index_x, Wp);
    Vector dW_dt = Vector(0, vec3(0), 0);
    const vec3 v = vec3(particle[index_x].velocity.x, particle[index_x].velocity.y, particle[index_x].velocity.z);
    const float density = particle[index_x].VolumeDensityPressureMass.y;
    const float pressure = particle[index_x].VolumeDensityPressureMass.z;
    const vec3 Wp_y_trace = vec3(gradWp.y[0][0], gradWp.y[1][1], gradWp.y[2][2]);
    dW_dt.x = -(dot(v, gradWp.x) + dot(vec3(density), Wp_y_trace));
    dW_dt.y = -(v * gradWp.y + vec3(1. / density) * gradWp.z);
    dW_dt.z = -(dot(vec3(gamma * pressure), Wp_y_trace) + dot(v, gradWp.z));

    return dW_dt;
}

Vector PrepareRiemmanProblemSide(uint32_t index_x, vec3 v_frame, vec3 x_mean_x, vec3* dir_versor) {
    const float dt = 1.f / ImGui::GetIO().Framerate;
    const mat3 I = mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 1);

    Vector W_x = Vector(particle[index_x].VolumeDensityPressureMass.y,
        vec3(particle[index_x].velocity.x, particle[index_x].velocity.y, particle[index_x].velocity.z),
        particle[index_x].VolumeDensityPressureMass.z);

    Vector Wp_x = Vector(W_x.x, W_x.y - v_frame, W_x.z);
    GradVector gradW_x = CalculateGradW(index_x, Wp_x);

    Vector gradCrossW_x = Vector(dot(gradW_x.x, x_mean_x),
        gradW_x.y * x_mean_x,
        dot(gradW_x.z, x_mean_x));
    Vector dW_dt_x = CalculateTimeDerivativeOfW(index_x, Wp_x);


    Vector Wpp_x = Vector(0, vec3(0), 0);
    Wpp_x.x = Wp_x.x + gradCrossW_x.x + dW_dt_x.x * dt / 2.;
    Wpp_x.y = Wp_x.y + gradCrossW_x.y + vec3(dW_dt_x.y.x * dt / 2., dW_dt_x.y.y * dt / 2., dW_dt_x.y.z * dt / 2.);
    Wpp_x.z = Wp_x.z + gradCrossW_x.z + dW_dt_x.z * dt / 2.;

    *dir_versor = length(Wpp_x.y) > 0 ? normalize(Wpp_x.y) : Wpp_x.y;

    //vec3 Wpp_y_norm = length(Wpp_x.y) > 0 ? normalize(Wpp_x.y) : vec3(0);
    //vec3 x_versor = vec3(1, 0, 0);
    //vec3 rot_x = cross(Wpp_y_norm, x_versor);
    //rot_x = length(rot_x) > 0 ? normalize(rot_x) : vec3(0);
    //float cos_x = dot(Wpp_y_norm, x_versor) / (length(Wpp_y_norm) * length(x_versor));
    //const mat3 rot_helper_x = mat3(0, -rot_x.z, rot_x.y,
    //    rot_x.z, 0, -rot_x.x,
    //    -rot_x.y, rot_x.x, 0);
    //const auto rot_helper_squared = rot_helper_x * rot_helper_x;
    //mat3 rotMtx_x = I + sqrt(1 - cos_x * cos_x) * rot_helper_x + (1 - cos_x) * rot_helper_squared;
    Vector Wppp_x = Vector(Wpp_x.x, vec3(length(Wpp_x.y), 0, 0), Wpp_x.z);

    return Wppp_x;
}


Vector CalculateReimannProblem(uint32_t index_i, uint32_t index_j) {
    const float dt = 1.f / ImGui::GetIO().Framerate;
    const mat3 I = mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 1);
    /////////////////////////////////////PREPARATION//////////////////////////////////////////
    vec3 x_i = vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z);
    vec3 x_j = vec3(particle[index_j].position.x, particle[index_j].position.y, particle[index_j].position.z);
    vec3 x_ij = vec3((x_i + x_j).x / 2., (x_i + x_j).y / 2., (x_i + x_j).z / 2.);
    vec3 v_frameij = CalculateFrameVelocity(index_i, index_j);

    vec3 x_ij_i = x_ij - x_i;
    vec3 x_ij_j = x_ij - x_j;

    vec3 dir_versor_r = vec3(0);
    Vector Wppp_r = PrepareRiemmanProblemSide(index_i, v_frameij, x_ij_i, &dir_versor_r);

    vec3 dir_versor_l = vec3(0);
    Vector Wppp_l = PrepareRiemmanProblemSide(index_j, v_frameij, x_ij_j, &dir_versor_l);
    /////////////////////////////////////HLLC SOLVER//////////////////////////////////////////
    float ro_r = Wppp_r.x;
    float u_r = Wppp_r.y.x;
    float p_r = Wppp_r.z;
    float ro_l = Wppp_l.x;
    float u_l = Wppp_l.y.x;
    float p_l = Wppp_l.z;

    float E_r = ro_r * u_r * u_r / 2. + p_r * (1 - b * ro_r) / (gamma - 1.); // for internal energy;
    float E_l = ro_l * u_l * u_l / 2. + p_l * (1 - b * ro_l) / (gamma - 1.);
    vec3 U_r = vec3(ro_r, ro_r * u_r, E_r);
    vec3 U_l = vec3(ro_l, ro_l * u_l, E_l);
    vec3 F_r = vec3(ro_r * u_r, ro_r * u_r * u_r + p_r, u_r * (E_r + p_r));
    vec3 F_l = vec3(ro_l * u_l, ro_l * u_l * u_l + p_l, u_l * (E_l + p_l));


    float a_mean = 1500.;
    float ro_mean = (ro_l + ro_r) / 2.;

    float p_hash = (p_l + p_r) / 2. - (u_r - u_l) / 2. * ro_mean * a_mean;
    float u_hash = (u_l + u_r) / 2. - (p_r - p_l) / (2. * ro_mean * a_mean);

    float q_l = p_hash / p_l <= 1 ? 1 : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_l - 1));
    float q_r = p_hash / p_r <= 1 ? 1 : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_r - 1));

    float S_l = u_l - a_mean * q_l;
    float S_r = u_r + a_mean * q_r;
    float S_m = u_hash;


    vec3 Q = S_l * U_l - F_l;
    vec3 R = S_r * U_r - F_r;

    float ro_l_hash = Q.x / (S_l - S_m);
    float ro_r_hash = R.x / (S_r - S_m);

    float p_l_hash = S_m * Q.x - Q.y;
    float p_r_hash = S_m * R.x - R.y;

    vec3 U_l_hash = vec3(ro_l_hash, ro_l_hash * u_hash, (Q.z + S_m * p_l_hash) / (S_l - S_m));
    vec3 U_r_hash = vec3(ro_r_hash, ro_r_hash * u_hash, (R.z + S_m * p_r_hash) / (S_r - S_m));

    vec3 F_l_hash;
    vec3 F_r_hash;
    if (S_l <= 0 && S_r >= 0) {
        F_r_hash = S_r * U_r_hash - R;
        F_l_hash = S_l * U_l_hash - Q;
    }
    else if (S_l <= S_m && S_m <= S_r && S_r <= 0) {
        F_l_hash = S_l * U_l_hash - Q;
        F_r_hash = (S_m * S_r * (U_r - U_l) + S_m * S_r * F_l / S_l - S_m * F_r + S_r * (1 - S_m / S_l) * F_l_hash) / (S_r - S_m);
    }
    else if (S_l >= 0 && S_m >= S_l && S_r >= S_m) {
        F_r_hash = S_r * U_r_hash - R;
        F_l_hash = S_l * U_l_hash - Q;
        F_l_hash = (S_m * S_l * (U_r - U_l) + S_m * S_l * F_r / S_r - S_m * F_l + S_l * (1 - S_m / S_r) * F_r_hash) / (S_m - S_l);
    }

    float CourantNumbers[5] = { -1,
                                dt * S_l / length(x_i - x_j),
                                dt * S_m / length(x_i - x_j),
                                dt * S_r / length(x_i - x_j),
                                1 };
    vec3 F_i2_k[5] = { vec3(0), F_l, F_l_hash, F_r_hash, F_r };
    vec3 F_i2 = vec3(0);
    for (uint32_t k = 1; k < 5; k++) {
        F_i2 += vec3((CourantNumbers[k] - CourantNumbers[k - 1]) / 2.) * F_i2_k[k];
    }
    /////////////////////////////////////FLUX IJ DEBOOST//////////////////////////////////////////
    vec3 x_versor = vec3(1, 0, 0);
    vec3 dir_versor_m = dir_versor_r + dir_versor_l;
    vec3 rot_m = cross(dir_versor_m, x_versor);
    float rot_angle_m = acos(dot(dir_versor_m, x_versor) / (length(dir_versor_m) * length(x_versor)));
    const mat3 rot_helper_m = mat3(0, -rot_m.z, rot_m.y,
                                   rot_m.z, 0, -rot_m.x,
                                   -rot_m.y, rot_m.x, 0);
    mat3 rotMtx_m = I + sin(rot_angle_m) * rot_helper_m + (1- cos(rot_angle_m)) * rot_helper_m * rot_helper_m;
    Vector Fp_ij = Vector(F_i2.x, dir_versor_m * (inverse(rotMtx_m) * vec3(F_i2.y, 0, 0)), F_i2.z);
    Vector F_ij = Vector(Fp_ij.x, Fp_ij.y + v_frameij * Fp_ij.x, length(v_frameij) * Fp_ij.x / 2. + dot(v_frameij, Fp_ij.y));

    return F_ij;
}

Vector GenerateHydrodynamics(uint32_t index_i) {
	float dt = 1.f / ImGui::GetIO().Framerate;
    Vector Q_n = Vector(particle[index_i].VolumeDensityPressureMass.y,
        particle[index_i].VolumeDensityPressureMass.y * vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
        particle[index_i].VolumeDensityPressureMass.y * dot(vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
            vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z)) / 2);
    float V_i = particle[index_i].VolumeDensityPressureMass.x;

    Vector Aij_Fij = Vector(0, vec3(0), 0);
    for (int j = 0; j < MaxNeighbours && particle[index_i].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_i].neighbours[j];
        Vector F_ij = CalculateReimannProblem(index_i, neighbour_index);
        vec3 nearPsi_ji = CalculateNearPsi(neighbour_index, index_i);
        vec3 nearPsi_ij = CalculateNearPsi(index_i, neighbour_index);
        float V_j = particle[neighbour_index].VolumeDensityPressureMass.x;

        Aij_Fij.x += F_ij.x * (V_i * nearPsi_ji.x - V_j * nearPsi_ij.x);
        Aij_Fij.y += F_ij.y * (V_i * nearPsi_ji.y - V_j * nearPsi_ij.y);
        Aij_Fij.z += F_ij.z * (V_i * nearPsi_ji.z - V_j * nearPsi_ij.z);
    }
    Vector Q_n1;
    Q_n1.x = Q_n.x - dt * Aij_Fij.x;
    Q_n1.y = Q_n.y - dt * Aij_Fij.y;
    Q_n1.z = Q_n.z - dt * Aij_Fij.z;

    return Q_n1;
}