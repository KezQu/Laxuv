#pragma once

#include "glm/glm.hpp"
#include <PhysicsDispatch.h>
#include <imgui.h>

constexpr uint32_t maxNeighbours = 64;

PhysicsProperties particle[maxNeighbours];

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const glm::mat3 I = glm::mat3(1, 0, 0,
    0, 1, 0,
    0, 0, 1);


struct Vector {
    float x;
    glm::vec3 y;
    float z;
};
struct GradVector {
    glm::vec3 x;
    glm::mat3 y;
    glm::vec3 z;
};

float b = 0.f;
float gamma = 1.4f;

glm::vec4 Qube(uint32_t idx, uint32_t maxParticles) {
    float meshSize = glm::pow(maxParticles, 1 / 3.);
    return glm::vec4(
        glm::mod(glm::floor(idx / meshSize), meshSize),
        glm::mod(float(idx), meshSize),
        glm::floor(idx / (meshSize * meshSize)),
        0.f);
}

void FindNeighbours(uint32_t index, uint32_t MaxParticles) {
    glm::vec3 x_i = glm::vec3(particle[index].position.x, particle[index].position.y, particle[index].position.z);
    uint32_t neighbourCount = 0;
    for (uint32_t j = 0; j < MaxParticles; j++) {
        glm::vec3 x_j = glm::vec3(particle[j].position.x, particle[j].position.y, particle[j].position.z);
        if (glm::distance(x_i, x_j) <= 2 * KernelRadius && (index != j)) {
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

glm::mat3 GetRotationZMatrix(float alpha_z) {
    return glm::mat3(cos(alpha_z), -sin(alpha_z), 0,
        sin(alpha_z), cos(alpha_z), 0,
        0, 0, 1);
}
glm::mat3 GetRotationYMatrix(float alpha_y) {
    return glm::mat3(cos(alpha_y), 0, sin(alpha_y),
        0, 1, 0,
        -sin(alpha_y), 0, cos(alpha_y));
}

float CalculateKernelWeight(glm::vec3 x) {
    const float b = 0.6;
    return (pow(E, -(x.x + x.y + x.z) / (2 * b * b * KernelRadius * KernelRadius)) * KernelRadius) / pow(2 * PI * b, 3 / 2);
}

glm::vec3 CalculateFrameVelocity(uint32_t index_i, uint32_t index_j) {
    glm::vec3 v_i = glm::vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z);
    glm::vec3 v_j = glm::vec3(particle[index_j].velocity.x, particle[index_j].velocity.y, particle[index_j].velocity.z);
    glm::vec3 x_i = glm::vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z);
    glm::vec3 x_j = glm::vec3(particle[index_j].position.x, particle[index_j].position.y, particle[index_j].position.z);
    glm::vec3 x_ij = glm::vec3((x_i.x + x_j.x) / 2., (x_i.y + x_j.y) / 2., (x_i.z + x_j.z) / 2.);

    return v_i + (v_j - v_i) * glm::vec3(
        ((x_ij - x_i) * (x_j - x_i)).x / pow(glm::distance(x_j, x_i), 2),
        ((x_ij - x_i) * (x_j - x_i)).y / pow(glm::distance(x_j, x_i), 2),
        ((x_ij - x_i) * (x_j - x_i)).z / pow(glm::distance(x_j, x_i), 2));
}

float CalculateOmega(uint32_t index_x) {

    float omega = 0;
    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        omega += CalculateKernelWeight(glm::vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) -
                                       glm::vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z));
    }
    return omega;
}

float CalculatePsi(uint32_t index_i, uint32_t index_x) {
    return CalculateKernelWeight(glm::vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) - 
                                 glm::vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z)) / CalculateOmega(index_x);
}

glm::vec3 CalculateNearPsi(uint32_t index_i, uint32_t index_x) {

    glm::mat3 E = glm::mat3(0);
    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        E += glm::outerProduct(glm::vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z) - 
                               glm::vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z),
                               glm::vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z) - 
                               glm::vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z)) *
            CalculatePsi(neighbour_index, index_x);
    }
    glm::mat3 B = glm::inverse(E);
    glm::vec3 nearPsi_j = B * (glm::vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z) - glm::vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z)) * CalculatePsi(index_i, index_x);
    return nearPsi_j;
}

GradVector CalculateGradW(uint32_t index_x, Vector Wp) {
    GradVector gradWp = GradVector(glm::vec3(0), glm::mat3(0), glm::vec3(0));

    for (uint32_t j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_x].neighbours[j];
        Vector Wp_temp_j = Vector(particle[neighbour_index].VolumeDensityPressureMass.y,
            glm::vec3(particle[neighbour_index].velocity.x, particle[neighbour_index].velocity.y, particle[neighbour_index].velocity.z) - CalculateFrameVelocity(index_x, neighbour_index),
            particle[neighbour_index].VolumeDensityPressureMass.z);
        glm::vec3 nearPsi_i = CalculateNearPsi(neighbour_index, index_x);
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
    Vector dW_dt = Vector(0, glm::vec3(0), 0);
    const glm::vec3 v = glm::vec3(particle[index_x].velocity.x, particle[index_x].velocity.y, particle[index_x].velocity.z);
    const float density = particle[index_x].VolumeDensityPressureMass.y;
    const float pressure = particle[index_x].VolumeDensityPressureMass.z;
    //const float gamma = 0.6;
    const glm::vec3 Wp_y_trace = glm::vec3(gradWp.y[0][0], gradWp.y[1][1], gradWp.y[2][2]);
    dW_dt.x = -(dot(v, gradWp.x) + dot(glm::vec3(density), Wp_y_trace));
    dW_dt.y = -(v * gradWp.y + glm::vec3(1. / density) * gradWp.z);
    dW_dt.z = -(dot(glm::vec3(gamma * pressure), Wp_y_trace) + dot(v, gradWp.z));

    return dW_dt;
}

Vector CalculateReimannProblem(uint32_t index_i, uint32_t index_j) {
    float dt = 1.f / ImGui::GetIO().Framerate;
    /////////////////////////////////////PREPARATION//////////////////////////////////////////
    glm::vec3 x_i = glm::vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z);
    glm::vec3 x_j = glm::vec3(particle[index_j].position.x, particle[index_j].position.y, particle[index_j].position.z);
    glm::vec3 x_ij = glm::vec3((x_i + x_j).x / 2., (x_i + x_j).y / 2., (x_i + x_j).z / 2.);
    glm::vec3 v_frameij = CalculateFrameVelocity(index_i, index_j);

    Vector W_i = Vector(particle[index_i].VolumeDensityPressureMass.y,
        glm::vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
        particle[index_i].VolumeDensityPressureMass.z);
    Vector W_j = Vector(particle[index_j].VolumeDensityPressureMass.y,
        glm::vec3(particle[index_j].velocity.x, particle[index_j].velocity.y, particle[index_j].velocity.z),
        particle[index_j].VolumeDensityPressureMass.z);

    Vector Wp_i = Vector(W_i.x, W_i.y - v_frameij, W_i.z);
    Vector Wp_j = Vector(W_j.x, W_j.y - v_frameij, W_j.z);

    GradVector gradW_i = CalculateGradW(index_i, Wp_i);
    GradVector gradW_j = CalculateGradW(index_j, Wp_j);

    glm::vec3 x_ij_i = x_ij - x_i;

    Vector gradCrossW_i = Vector(dot(gradW_i.x, x_ij_i),
        gradW_i.y * x_ij_i,
        dot(gradW_i.z, x_ij_i));

    glm::vec3 x_ij_j = x_ij - x_j;
    Vector gradCrossW_j = Vector(dot(gradW_j.x, x_ij_j),
        gradW_j.y * x_ij_j,
        dot(gradW_j.z, x_ij_j));

    Vector dW_dt_i = CalculateTimeDerivativeOfW(index_i, Wp_i);
    Vector dW_dt_j = CalculateTimeDerivativeOfW(index_j, Wp_j);

    Vector Wpp_l = Vector(0, glm::vec3(0), 0);
    Wpp_l.x = Wp_j.x + gradCrossW_j.x + dW_dt_j.x * dt / 2.;
    Wpp_l.y = Wp_j.y + gradCrossW_j.y + glm::vec3(dW_dt_j.y.x * dt / 2., dW_dt_j.y.y * dt / 2., dW_dt_j.y.z * dt / 2.);
    Wpp_l.z = Wp_j.z + gradCrossW_j.z + dW_dt_j.z * dt / 2.;

    Vector Wpp_r = Vector(0, glm::vec3(0), 0);
    Wpp_r.x = Wp_i.x + gradCrossW_i.x + dW_dt_i.x * dt / 2.;
    Wpp_r.y = Wp_i.y + gradCrossW_i.y + glm::vec3(dW_dt_i.y.x * dt / 2., dW_dt_i.y.y * dt / 2., dW_dt_i.y.z * dt / 2.);
    Wpp_r.z = Wp_i.z + gradCrossW_i.z + dW_dt_i.z * dt / 2.;

    float alpha_zr = glm::atan(Wpp_r.y.y, Wpp_r.y.x);
    float alpha_zl = glm::atan(Wpp_l.y.y, Wpp_l.y.x);
    float alpha_yr = glm::atan(Wpp_r.y.z, sqrt(Wpp_r.y.x * Wpp_r.y.x + Wpp_r.y.y * Wpp_r.y.y));
    float alpha_yl = glm::atan(Wpp_l.y.z, sqrt(Wpp_l.y.x * Wpp_l.y.x + Wpp_l.y.y * Wpp_l.y.y));

    glm::vec3 direction_versor = glm::length(Wpp_r.y) > 0 ? glm::normalize(Wpp_r.y) : Wpp_r.y;
    glm::mat3 rotMtx = GetRotationYMatrix(alpha_yr) * GetRotationZMatrix(alpha_zr);
    //glm::mat3 rotMtx_l = GetRotationYMatrix(alpha_yl) * GetRotationZMatrix(alpha_zl);
    Vector Wppp_r = Vector(Wpp_r.x, rotMtx * Wpp_r.y, Wpp_l.z);
    Vector Wppp_l = Vector(Wpp_l.x, rotMtx * Wpp_l.y, Wpp_l.z);

    /////////////////////////////////////HLLC SOLVER//////////////////////////////////////////
    float ro_r = Wppp_r.x;
    float u_r = Wppp_r.y.x;
    float p_r = Wppp_r.z;
    float ro_l = Wppp_l.x;
    float u_l = Wppp_l.y.x;
    float p_l = Wppp_l.z;

    float E_r = ro_r * u_r * u_r / 2. + p_r * (1 - b * ro_r) / (gamma - 1.); // for internal energy;
    float E_l = ro_l * u_l * u_l / 2. + p_l * (1 - b * ro_l) / (gamma - 1.);
    glm::vec3 U_r = glm::vec3(ro_r, ro_r * u_r, E_r);
    glm::vec3 U_l = glm::vec3(ro_l, ro_l * u_l, E_l);
    glm::vec3 F_r = glm::vec3(ro_r * u_r, ro_r * u_r * u_r + p_r, u_r * (E_r + p_r));
    glm::vec3 F_l = glm::vec3(ro_l * u_l, ro_l * u_l * u_l + p_l, u_l * (E_l + p_l));


    float a_mean = 1500.;
    float ro_mean = (ro_l + ro_r) / 2.;

    float p_hash = (p_l + p_r) / 2. - (u_r - u_l) / 2. * ro_mean * a_mean;
    float u_hash = (u_l + u_r) / 2. - (p_r - p_l) / (2. * ro_mean * a_mean);

    float q_l = p_hash / p_l <= 1 ? 1 : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_l - 1));
    float q_r = p_hash / p_r <= 1 ? 1 : sqrt(1 + (gamma + 1) / (2. * gamma) * (p_hash / p_r - 1));

    float S_l = u_l - a_mean * q_l;
    float S_r = u_r + a_mean * q_r;
    float S_m = u_hash;


    glm::vec3 Q = S_l * U_l - F_l;
    glm::vec3 R = S_r * U_r - F_r;

    float ro_l_hash = Q.x / (S_l - S_m);
    float ro_r_hash = R.x / (S_r - S_m);

    float p_l_hash = S_m * Q.x - Q.y;
    float p_r_hash = S_m * R.x - R.y;

    glm::vec3 U_l_hash = glm::vec3(ro_l_hash, ro_l_hash * u_hash, (Q.z + S_m * p_l_hash) / (S_l - S_m));
    glm::vec3 U_r_hash = glm::vec3(ro_r_hash, ro_r_hash * u_hash, (R.z + S_m * p_r_hash) / (S_r - S_m));

    glm::vec3 F_l_hash;
    glm::vec3 F_r_hash;
    if (S_l <= 0 && S_r >= 0) {
        F_r_hash = S_r * U_r_hash - R;
        F_l_hash = S_l * U_l_hash - Q;
    }
    else if (S_l <= 0 && S_m <= 0 && S_r <= 0) {
        F_l_hash = S_l * U_l_hash - Q;
        F_r_hash = (S_m * S_r * (U_r - U_l) + S_m * S_r * F_l / S_l - S_m * F_r + S_r * (1 - S_m / S_l) * F_l_hash) / (S_r - S_m);
    }
    else if (S_l <= 0 && S_m <= 0 && S_r <= 0) {
        F_r_hash = S_r * U_r_hash - R;
        F_l_hash = S_l * U_l_hash - Q;
        F_l_hash = (S_m * S_l * (U_r - U_l) + S_m * S_l * F_r / S_r - S_m * F_l + S_l * (1 - S_m / S_r) * F_r_hash) / (S_m - S_l);
    }


    float CourantNumbers[5] = { -1,
                                dt * S_l / glm::length(x_i - x_j),
                                dt * S_m / glm::length(x_i - x_j),
                                dt * S_r / glm::length(x_i - x_j),
                                1 };
    glm::vec3 F_i2_k[5] = { glm::vec3(0), F_l, F_l_hash, F_r_hash, F_r };
    glm::vec3 F_i2;
    for (uint32_t k = 1; k < 5; k++) {
        F_i2 += glm::vec3((CourantNumbers[k] - CourantNumbers[k - 1]) / 2.) * F_i2_k[k];
    }
    /////////////////////////////////////FLUX IJ DEBOOST//////////////////////////////////////////
    Vector Fp_ij = Vector(F_i2.x, direction_versor * (glm::inverse(rotMtx) * glm::vec3(F_i2.y, 0, 0)), F_i2.z);
    Vector F_ij = Vector(Fp_ij.x, Fp_ij.y + v_frameij * Fp_ij.x, glm::length(v_frameij) * Fp_ij.x / 2. + dot(v_frameij, Fp_ij.y));

    return F_ij;
}

Vector GenerateHydrodynamics(uint32_t index_i) {
	float dt = 1.f / ImGui::GetIO().Framerate;
    Vector Q_n = Vector(particle[index_i].VolumeDensityPressureMass.y,
        particle[index_i].VolumeDensityPressureMass.y * glm::vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
        particle[index_i].VolumeDensityPressureMass.y * glm::dot(glm::vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
            glm::vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z)) / 2);
    float V_i = particle[index_i].VolumeDensityPressureMass.x;

    Vector Aij_Fij = Vector(0, glm::vec3(0), 0);
    for (int j = 0; j < MaxNeighbours && particle[index_i].neighbours[j] != 0xffffffff; j++) {
        uint32_t neighbour_index = particle[index_i].neighbours[j];
        Vector F_ij = CalculateReimannProblem(index_i, neighbour_index);
        glm::vec3 nearPsi_ji = CalculateNearPsi(neighbour_index, index_i);
        glm::vec3 nearPsi_ij = CalculateNearPsi(index_i, neighbour_index);
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