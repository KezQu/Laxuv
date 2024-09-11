#version 460

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const mat3 I = mat3(1, 0, 0,
    0, 1, 0,
    0, 0, 1);

const uint KernelRadius = 1;
const uint MaxNeighbours = KernelRadius * KernelRadius * 4 * 4 * 2;

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

uniform float dt;
uniform float b = 0.f;
uniform float gamma = 0.6f;

struct PhysicsProperties{
	vec4 force;
	vec4 velocity;
	vec4 position;
	vec4 VolumeDensityPressureMass;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	PhysicsProperties particle[];
};

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

vec3 CalculateFrameVelocity(uint index_i, uint index_j) {
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

void FindNeighbours(uint index, uint MaxParticles) {
    vec3 x_i = vec3(particle[index].position.x, particle[index].position.y, particle[index].position.z);
    uint neighbourCount = 0;
    for (uint j = 0; j < MaxParticles; j++) {
        vec3 x_j = vec3(particle[j].position.x, particle[j].position.y, particle[j].position.z);
        if (distance(x_i, x_j) <= 2 * KernelRadius && (index != j)) {
            particle[index].neighbours[neighbourCount] = j;
            neighbourCount++;
        }
    }
}

float CalculateOmega(uint index_x) {

    float omega = 0;
    for (uint j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint neighbour_index = particle[index_x].neighbours[j];
        omega += CalculateKernelWeight(vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) -
                                       vec3(particle[neighbour_index].position.x, particle[neighbour_index].position.y, particle[neighbour_index].position.z));
    }
    return omega;
}

float CalculatePsi(uint index_i, uint index_x) {
    return CalculateKernelWeight(vec3(particle[index_x].position.x, particle[index_x].position.y, particle[index_x].position.z) - 
                                 vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z)) / CalculateOmega(index_x);
}

vec3 CalculateNearPsi(uint index_i, uint index_x) {

    mat3 E = mat3(0);
    for (uint j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint neighbour_index = particle[index_x].neighbours[j];
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

GradVector CalculateGradW(uint index_x, Vector Wp) {
    GradVector gradWp = GradVector(vec3(0), mat3(0), vec3(0));

    for (uint j = 0; j < MaxNeighbours && particle[index_x].neighbours[j] != 0xffffffff; j++) {
        uint neighbour_index = particle[index_x].neighbours[j];
        Vector Wp_temp_j = Vector(particle[neighbour_index].VolumeDensityPressureMass.y,
            vec3(particle[neighbour_index].velocity.x, particle[neighbour_index].velocity.y, particle[neighbour_index].velocity.z) - CalculateFrameVelocity(index_x, neighbour_index),
            particle[neighbour_index].VolumeDensityPressureMass.z);
        vec3 nearPsi_i = CalculateNearPsi(neighbour_index, index_x);
        gradWp.x += (Wp_temp_j.x - Wp.x) * nearPsi_i;
        gradWp.y += outerProduct((Wp_temp_j.y - Wp.y), nearPsi_i);
        gradWp.z += (Wp_temp_j.z - Wp.z) * nearPsi_i;
        //for(uint dim = 0; dim < 3; dim ++){
        //	gradW_i[dim].x += (Wp_temp_j.x - Wp_i.x) * nearPsi_i[dim];
        //	gradW_i[dim].y += (Wp_temp_j.y - Wp_i.y) * nearPsi_i[dim];
        //	gradW_i[dim].z += (Wp_temp_j.z - Wp_i.z) * nearPsi_i[dim];
        //}
    }
    return gradWp;
}

Vector CalculateTimeDerivativeOfW(uint index_x, Vector Wp) {
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

Vector CalculateReimannProblem(uint index_i, uint index_j) {
    /////////////////////////////////////PREPARATION//////////////////////////////////////////
    vec3 x_i = vec3(particle[index_i].position.x, particle[index_i].position.y, particle[index_i].position.z);
    vec3 x_j = vec3(particle[index_j].position.x, particle[index_j].position.y, particle[index_j].position.z);
    vec3 x_ij = vec3((x_i + x_j).x / 2., (x_i + x_j).y / 2., (x_i + x_j).z / 2.);
    vec3 v_frameij = CalculateFrameVelocity(index_i, index_j);

    Vector W_i = Vector(particle[index_i].VolumeDensityPressureMass.y,
        vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
        particle[index_i].VolumeDensityPressureMass.z);
    Vector W_j = Vector(particle[index_j].VolumeDensityPressureMass.y,
        vec3(particle[index_j].velocity.x, particle[index_j].velocity.y, particle[index_j].velocity.z),
        particle[index_j].VolumeDensityPressureMass.z);

    Vector Wp_i = Vector(W_i.x, W_i.y - v_frameij, W_i.z);
    Vector Wp_j = Vector(W_j.x, W_j.y - v_frameij, W_j.z);

    GradVector gradW_i = CalculateGradW(index_i, Wp_i);
    GradVector gradW_j = CalculateGradW(index_j, Wp_j);

    vec3 x_ij_i = x_ij - x_i;

    Vector gradCrossW_i = Vector(dot(gradW_i.x, x_ij_i),
        gradW_i.y * x_ij_i,
        dot(gradW_i.z, x_ij_i));

    vec3 x_ij_j = x_ij - x_j;
    Vector gradCrossW_j = Vector(dot(gradW_j.x, x_ij_j),
        gradW_j.y * x_ij_j,
        dot(gradW_j.z, x_ij_j));

    Vector dW_dt_i = CalculateTimeDerivativeOfW(index_i, Wp_i);
    Vector dW_dt_j = CalculateTimeDerivativeOfW(index_j, Wp_j);

    Vector Wpp_l = Vector(0, vec3(0), 0);
    Wpp_l.x = Wp_j.x + gradCrossW_j.x + dW_dt_j.x * dt / 2.;
    Wpp_l.y = Wp_j.y + gradCrossW_j.y + vec3(dW_dt_j.y.x * dt / 2., dW_dt_j.y.y * dt / 2., dW_dt_j.y.z * dt / 2.);
    Wpp_l.z = Wp_j.z + gradCrossW_j.z + dW_dt_j.z * dt / 2.;

    Vector Wpp_r = Vector(0, vec3(0), 0);
    Wpp_r.x = Wp_i.x + gradCrossW_i.x + dW_dt_i.x * dt / 2.;
    Wpp_r.y = Wp_i.y + gradCrossW_i.y + vec3(dW_dt_i.y.x * dt / 2., dW_dt_i.y.y * dt / 2., dW_dt_i.y.z * dt / 2.);
    Wpp_r.z = Wp_i.z + gradCrossW_i.z + dW_dt_i.z * dt / 2.;
    
    vec3 direction_versor = length(Wpp_r.y) > 0 ? normalize(Wpp_r.y) : Wpp_r.y;
 
    float alpha_zr = atan(Wpp_r.y.y, Wpp_r.y.x);
    float alpha_zl = atan(Wpp_l.y.y, Wpp_l.y.x);
    Vector Wppp_r = Vector(Wpp_r.x, GetRotationZMatrix(alpha_zr) * Wpp_r.y, Wpp_r.z);
    Vector Wppp_l = Vector(Wpp_l.x, GetRotationZMatrix(alpha_zr) * Wpp_l.y, Wpp_l.z);
    float alpha_yr = atan(Wppp_r.y.z, sqrt(Wppp_r.y.x * Wppp_r.y.x + Wppp_r.y.y * Wppp_r.y.y));
    float alpha_yl = atan(Wppp_l.y.z, sqrt(Wppp_l.y.x * Wppp_l.y.x + Wppp_l.y.y * Wppp_l.y.y));
    Wppp_r.y = GetRotationYMatrix(alpha_yr) * Wppp_r.y;
    Wppp_l.y = GetRotationYMatrix(alpha_yr) * Wppp_l.y;

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
    for (uint k = 1; k < 5; k++) {
        F_i2 += vec3((CourantNumbers[k] - CourantNumbers[k - 1]) / 2.) * F_i2_k[k];
    }
    /////////////////////////////////////FLUX IJ DEBOOST//////////////////////////////////////////
    Vector Fp_ij = Vector(F_i2.x, direction_versor * (inverse(GetRotationZMatrix(alpha_zr)) * (inverse(GetRotationYMatrix(alpha_yr)) * glm::vec3(F_i2.y, 0, 0))), F_i2.z);
    Vector F_ij = Vector(Fp_ij.x, Fp_ij.y + v_frameij * Fp_ij.x, length(v_frameij) * Fp_ij.x / 2. + dot(v_frameij, Fp_ij.y));

    return F_ij;
}

Vector GenerateHydrodynamics(uint index_i) {
    Vector Q_n = Vector(particle[index_i].VolumeDensityPressureMass.y,
        particle[index_i].VolumeDensityPressureMass.y * vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
        particle[index_i].VolumeDensityPressureMass.y * dot(vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z),
            vec3(particle[index_i].velocity.x, particle[index_i].velocity.y, particle[index_i].velocity.z)) / 2);
    float V_i = particle[index_i].VolumeDensityPressureMass.x;

    Vector Aij_Fij = Vector(0, vec3(0), 0);
    for (int j = 0; j < MaxNeighbours && particle[index_i].neighbours[j] != 0xffffffff; j++) {
        uint neighbour_index = particle[index_i].neighbours[j];
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