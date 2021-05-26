#include "math.h"
#include "math_functions.hh"

#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define RAD2DEG(_rad) ((_rad) * (180.0 / M_PI))
#define DEG2RAD(_deg) ((_deg) * (M_PI / 180.0))
#define CLAMP(a, b, c) \
  { \
    if ((a) < (b)) { \
      (a) = (b); \
    } \
    else if ((a) > (c)) { \
      (a) = (c); \
    } \
  } \
  (void)0

float compatible_signf(float f)
{
  if (f > 0.0f) {
    return 1.0f;
  }
  if (f < 0.0f) {
    return -1.0f;
  }
  else {
    return 0.0f;
  }
}

float min_ff(float a, float b)
{
  return (a < b) ? a : b;
}

float max_ff(float a, float b)
{
  return (a > b) ? a : b;
}

float safe_divide(float a, float b)
{
  return (b != 0.0f) ? a / b : 0.0f;
}

float safe_powf(float base, float exponent)
{
  if (base < 0.0f && exponent != (int)exponent) {
    return 0.0f;
  }
  return powf(base, exponent);
}

float safe_logf(float a, float base)
{
  if (a <= 0.0f || base <= 0.0f) {
    return 0.0f;
  }
  return safe_divide(logf(a), logf(base));
}

float fractf(float a)
{
  return a - floorf(a);
}

float safe_asinf(float a)
{
  CLAMP(a, -1.0f, 1.0f);
  return asinf(a);
}

float safe_acosf(float a)
{
  CLAMP(a, -1.0f, 1.0f);
  return acosf(a);
}

float safe_sqrtf(float a)
{
  return sqrtf(MAX2(a, 0.0f));
}

float safe_inverse_sqrtf(float a)
{
  return (a > 0.0f) ? 1.0f / sqrtf(a) : 0.0f;
}

float pingpongf(float value, float scale)
{
  if (scale == 0.0f) {
    return 0.0f;
  }
  return fabsf(fractf((value - scale) / (scale * 2.0f)) * scale * 2.0f - scale);
}

float smoothminf(float a, float b, float c)
{
  if (c != 0.0f) {
    float h = max_ff(c - fabsf(a - b), 0.0f) / c;
    return min_ff(a, b) - h * h * h * c * (1.0f / 6.0f);
  }
  else {
    return min_ff(a, b);
  }
}

float wrapf(float value, float max, float min)
{
  float range = max - min;
  return (range != 0.0f) ? value - (range * floorf((value - min) / range)) : min;
}

template<typename Callback>
inline bool try_dispatch_float_math_fl_to_fl(const int operation, Callback &&callback)
{
  const FloatMathOperationInfo *info = get_float_math_operation_info(operation);
  if (info == nullptr) {
    return false;
  }

  /* This is just an utility function to keep the individual cases smaller. */
  auto dispatch = [&](auto math_function) -> bool {
    callback(math_function, *info);
    return true;
  };

  switch (operation) {
    case NODE_MATH_EXPONENT:
      return dispatch([](float a) { return expf(a); });
    case NODE_MATH_SQRT:
      return dispatch([](float a) { return safe_sqrtf(a); });
    case NODE_MATH_INV_SQRT:
      return dispatch([](float a) { return safe_inverse_sqrtf(a); });
    case NODE_MATH_ABSOLUTE:
      return dispatch([](float a) { return fabs(a); });
    case NODE_MATH_RADIANS:
      return dispatch([](float a) { return (float)DEG2RAD(a); });
    case NODE_MATH_DEGREES:
      return dispatch([](float a) { return (float)RAD2DEG(a); });
    case NODE_MATH_SIGN:
      return dispatch([](float a) { return compatible_signf(a); });
    case NODE_MATH_ROUND:
      return dispatch([](float a) { return floorf(a + 0.5f); });
    case NODE_MATH_FLOOR:
      return dispatch([](float a) { return floorf(a); });
    case NODE_MATH_CEIL:
      return dispatch([](float a) { return ceilf(a); });
    case NODE_MATH_FRACTION:
      return dispatch([](float a) { return a - floorf(a); });
    case NODE_MATH_TRUNC:
      return dispatch([](float a) { return a >= 0.0f ? floorf(a) : ceilf(a); });
    case NODE_MATH_SINE:
      return dispatch([](float a) { return sinf(a); });
    case NODE_MATH_COSINE:
      return dispatch([](float a) { return cosf(a); });
    case NODE_MATH_TANGENT:
      return dispatch([](float a) { return tanf(a); });
    case NODE_MATH_SINH:
      return dispatch([](float a) { return sinhf(a); });
    case NODE_MATH_COSH:
      return dispatch([](float a) { return coshf(a); });
    case NODE_MATH_TANH:
      return dispatch([](float a) { return tanhf(a); });
    case NODE_MATH_ARCSINE:
      return dispatch([](float a) { return safe_asinf(a); });
    case NODE_MATH_ARCCOSINE:
      return dispatch([](float a) { return safe_acosf(a); });
    case NODE_MATH_ARCTANGENT:
      return dispatch([](float a) { return atanf(a); });
  }
  return false;
}

/**
 * This is similar to try_dispatch_float_math_fl_to_fl, just with a different callback signature.
 */
template<typename Callback>
inline bool try_dispatch_float_math_fl_fl_to_fl(const int operation, Callback &&callback)
{
  const FloatMathOperationInfo *info = get_float_math_operation_info(operation);
  if (info == nullptr) {
    return false;
  }

  /* This is just an utility function to keep the individual cases smaller. */
  auto dispatch = [&](auto math_function) -> bool {
    callback(math_function, *info);
    return true;
  };

  switch (operation) {
    case NODE_MATH_ADD:
      return dispatch([](float a, float b) { return a + b; });
    case NODE_MATH_SUBTRACT:
      return dispatch([](float a, float b) { return a - b; });
    case NODE_MATH_MULTIPLY:
      return dispatch([](float a, float b) { return a * b; });
    case NODE_MATH_DIVIDE:
      return dispatch([](float a, float b) { return safe_divide(a, b); });
    case NODE_MATH_POWER:
      return dispatch([](float a, float b) { return safe_powf(a, b); });
    case NODE_MATH_LOGARITHM:
      return dispatch([](float a, float b) { return safe_logf(a, b); });
    case NODE_MATH_MINIMUM:
      return dispatch([](float a, float b) { return std::min(a, b); });
    case NODE_MATH_MAXIMUM:
      return dispatch([](float a, float b) { return std::max(a, b); });
    case NODE_MATH_LESS_THAN:
      return dispatch([](float a, float b) { return (float)(a < b); });
    case NODE_MATH_GREATER_THAN:
      return dispatch([](float a, float b) { return (float)(a > b); });
    case NODE_MATH_MODULO:
      return dispatch([](float a, float b) { return modf(a, b); });
    case NODE_MATH_SNAP:
      return dispatch([](float a, float b) { return floorf(safe_divide(a, b)) * b; });
    case NODE_MATH_ARCTAN2:
      return dispatch([](float a, float b) { return atan2f(a, b); });
    case NODE_MATH_PINGPONG:
      return dispatch([](float a, float b) { return pingpongf(a, b); });
  }
  return false;
}

/**
 * This is similar to try_dispatch_float_math_fl_to_fl, just with a different callback signature.
 */
template<typename Callback>
inline bool try_dispatch_float_math_fl_fl_fl_to_fl(const int operation, Callback &&callback)
{
  const FloatMathOperationInfo *info = get_float_math_operation_info(operation);
  if (info == nullptr) {
    return false;
  }

  /* This is just an utility function to keep the individual cases smaller. */
  auto dispatch = [&](auto math_function) -> bool {
    callback(math_function, *info);
    return true;
  };

  switch (operation) {
    case NODE_MATH_MULTIPLY_ADD:
      return dispatch([](float a, float b, float c) { return a * b + c; });
    case NODE_MATH_COMPARE:
      return dispatch([](float a, float b, float c) -> float {
        return ((a == b) || (fabsf(a - b) <= fmaxf(c, FLT_EPSILON))) ? 1.0f : 0.0f;
      });
    case NODE_MATH_SMOOTH_MIN:
      return dispatch([](float a, float b, float c) { return smoothminf(a, b, c); });
    case NODE_MATH_SMOOTH_MAX:
      return dispatch([](float a, float b, float c) { return -smoothminf(-a, -b, -c); });
    case NODE_MATH_WRAP:
      return dispatch([](float a, float b, float c) { return wrapf(a, b, c); });
  }
  return false;
}