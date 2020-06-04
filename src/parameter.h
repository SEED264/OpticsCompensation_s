#ifndef _OPTICSCOMPENSATION_S_SRC_PARAMETER_H_
#define _OPTICSCOMPENSATION_S_SRC_PARAMETER_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/vec2.hpp>

struct OpticsCompensationParameter {
    OpticsCompensationParameter();
    OpticsCompensationParameter(float amount, bool spool_mode, bool anti_aliasing, 
                                const glm::vec2 &center_pos);

    float CalcFocalDistance();

    float amount;
    bool spool_mode;
    bool anti_aliasing;
    glm::vec2 center_pos;
};

inline OpticsCompensationParameter::
    OpticsCompensationParameter() :
    OpticsCompensationParameter(0, false, false, glm::vec2(0)) {}

inline OpticsCompensationParameter::
    OpticsCompensationParameter(float amount, bool spool_mode, bool anti_aliasing,
                                const glm::vec2 &center_pos) :
    amount(amount),
    spool_mode(spool_mode),
    anti_aliasing(anti_aliasing), 
    center_pos(center_pos) {}

inline float OpticsCompensationParameter::CalcFocalDistance() {
    return static_cast<float>(500.0 / std::tan(0.5 * amount * 3.14159265358979323846));
}

#endif // _OPTICSCOMPENSATION_S_SRC_PARAMETER_H_