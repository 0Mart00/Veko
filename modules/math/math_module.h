#ifndef MATH_MODULE_H
#define MATH_MODULE_H

#include "../../include/interface.h"

// Math module functions
void import_math_module(EngineState* state);
int is_math_imported(EngineState* state);
void call_math_function(EngineState* state, const char* func_name, 
                       const char* result, const char* arg1, const char* arg2);

// Number-theoretic
double math_factorial(int n);
int math_gcd(int a, int b);
int math_lcm(int a, int b);
int math_isqrt(int n);
double math_comb(int n, int k);
double math_perm(int n, int k);

// Floating point arithmetic
double math_ceil(double x);
double math_fabs(double x);
double math_floor(double x);
double math_fma(double x, double y, double z);
double math_fmod(double x, double y);
double math_remainder(double x, double y);
double math_trunc(double x);
double math_modf_frac(double x);

// Power, exponential and logarithmic
double math_cbrt(double x);
double math_exp(double x);
double math_exp2(double x);
double math_expm1(double x);
double math_log(double x);
double math_log1p(double x);
double math_log2(double x);
double math_log10(double x);
double math_pow(double x, double y);
double math_sqrt(double x);

// Trigonometric
double math_acos(double x);
double math_asin(double x);
double math_atan(double x);
double math_atan2(double y, double x);
double math_cos(double x);
double math_sin(double x);
double math_tan(double x);

// Hyperbolic
double math_acosh(double x);
double math_asinh(double x);
double math_atanh(double x);
double math_cosh(double x);
double math_sinh(double x);
double math_tanh(double x);

// Special functions
double math_erf(double x);
double math_erfc(double x);
double math_gamma(double x);
double math_lgamma(double x);

// Angular conversion
double math_degrees(double x);
double math_radians(double x);

// Floating point manipulation
double math_copysign(double x, double y);
int math_isfinite(double x);
int math_isinf(double x);
int math_isnan(double x);

// Distance
double math_hypot(double x, double y);

#endif
