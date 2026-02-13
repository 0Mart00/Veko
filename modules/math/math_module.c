#include <stdio.h>
#include <string.h>
#include <math.h>
#include "math_module.h"
#include "../core/memory.h"
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_TAU
#define M_TAU 6.28318530717958647692
#endif

// ============================================================================
// NUMBER-THEORETIC FUNCTIONS
// ============================================================================

double math_factorial(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

int math_gcd(int a, int b) {
    a = abs(a); b = abs(b);
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int math_lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return abs(a * b) / math_gcd(a, b);
}

int math_isqrt(int n) {
    if (n < 0) return 0;
    return (int)sqrt((double)n);
}

double math_comb(int n, int k) {
    if (k > n || k < 0) return 0;
    if (k == 0 || k == n) return 1;
    k = (k < n - k) ? k : n - k;
    double result = 1;
    for (int i = 0; i < k; i++) {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}

double math_perm(int n, int k) {
    if (k > n || k < 0) return 0;
    double result = 1;
    for (int i = 0; i < k; i++) {
        result *= (n - i);
    }
    return result;
}

// ============================================================================
// FLOATING POINT ARITHMETIC
// ============================================================================

double math_ceil(double x) { return ceil(x); }
double math_fabs(double x) { return fabs(x); }
double math_floor(double x) { return floor(x); }
double math_fma(double x, double y, double z) { return (x * y) + z; }
double math_fmod(double x, double y) { return fmod(x, y); }
double math_remainder(double x, double y) { return remainder(x, y); }
double math_trunc(double x) { return trunc(x); }

double math_modf_frac(double x) {
    double intpart;
    return modf(x, &intpart);
}

// ============================================================================
// POWER, EXPONENTIAL AND LOGARITHMIC
// ============================================================================

double math_cbrt(double x) { return cbrt(x); }
double math_exp(double x) { return exp(x); }
double math_exp2(double x) { return exp2(x); }
double math_expm1(double x) { return expm1(x); }
double math_log(double x) { return log(x); }
double math_log1p(double x) { return log1p(x); }
double math_log2(double x) { return log2(x); }
double math_log10(double x) { return log10(x); }
double math_pow(double x, double y) { return pow(x, y); }
double math_sqrt(double x) { return sqrt(x); }

// ============================================================================
// TRIGONOMETRIC FUNCTIONS
// ============================================================================

double math_acos(double x) { return acos(x); }
double math_asin(double x) { return asin(x); }
double math_atan(double x) { return atan(x); }
double math_atan2(double y, double x) { return atan2(y, x); }
double math_cos(double x) { return cos(x); }
double math_sin(double x) { return sin(x); }
double math_tan(double x) { return tan(x); }

// ============================================================================
// HYPERBOLIC FUNCTIONS
// ============================================================================

double math_acosh(double x) { return acosh(x); }
double math_asinh(double x) { return asinh(x); }
double math_atanh(double x) { return atanh(x); }
double math_cosh(double x) { return cosh(x); }
double math_sinh(double x) { return sinh(x); }
double math_tanh(double x) { return tanh(x); }

// ============================================================================
// SPECIAL FUNCTIONS
// ============================================================================

double math_erf(double x) { return erf(x); }
double math_erfc(double x) { return erfc(x); }
double math_gamma(double x) { return tgamma(x); }
double math_lgamma(double x) { return lgamma(x); }

// ============================================================================
// ANGULAR CONVERSION
// ============================================================================

double math_degrees(double x) { return x * 180.0 / M_PI; }
double math_radians(double x) { return x * M_PI / 180.0; }

// ============================================================================
// FLOATING POINT MANIPULATION
// ============================================================================

double math_copysign(double x, double y) { return copysign(x, y); }
int math_isfinite(double x) { return isfinite(x); }
int math_isinf(double x) { return isinf(x); }
int math_isnan(double x) { return isnan(x); }

// ============================================================================
// DISTANCE
// ============================================================================

double math_hypot(double x, double y) { return hypot(x, y); }

// ============================================================================
// IMPORT SYSTEM
// ============================================================================

void import_math_module(EngineState* state) {
    for (int i = 0; i < state->import_count; i++) {
        if (strcmp(state->imports[i].module_name, "math") == 0) {
            return;
        }
    }
    
    if (state->import_count < MAX_IMPORTS) {
        strncpy(state->imports[state->import_count].module_name, "math", NAME_LEN - 1);
        state->imports[state->import_count].loaded = 1;
        state->import_count++;
    }
    
    set_number(state, "math.pi", M_PI);
    set_number(state, "math.e", M_E);
    set_number(state, "math.tau", M_TAU);
    set_number(state, "math.inf", INFINITY);
    set_number(state, "math.nan", NAN);
    
    printf(">>> [IMPORT] math module loaded\n");
}

int is_math_imported(EngineState* state) {
    for (int i = 0; i < state->import_count; i++) {
        if (strcmp(state->imports[i].module_name, "math") == 0) {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// MATH FUNCTION DISPATCHER
// ============================================================================

void call_math_function(EngineState* state, const char* func_name, 
                       const char* result, const char* arg1, const char* arg2) {
    if (!is_math_imported(state)) {
        printf(">>> [ERROR] math module not imported. Use: import math\n");
        return;
    }
    
    double v1 = resolve_value(state, arg1);
    double v2 = arg2 ? resolve_value(state, arg2) : 0;
    double res = 0;
    
    // Number-theoretic
    if (strcmp(func_name, "math.factorial") == 0) res = math_factorial((int)v1);
    else if (strcmp(func_name, "math.gcd") == 0) res = math_gcd((int)v1, (int)v2);
    else if (strcmp(func_name, "math.lcm") == 0) res = math_lcm((int)v1, (int)v2);
    else if (strcmp(func_name, "math.isqrt") == 0) res = math_isqrt((int)v1);
    else if (strcmp(func_name, "math.comb") == 0) res = math_comb((int)v1, (int)v2);
    else if (strcmp(func_name, "math.perm") == 0) res = math_perm((int)v1, (int)v2);
    
    // Floating point arithmetic
    else if (strcmp(func_name, "math.ceil") == 0) res = math_ceil(v1);
    else if (strcmp(func_name, "math.fabs") == 0) res = math_fabs(v1);
    else if (strcmp(func_name, "math.floor") == 0) res = math_floor(v1);
    else if (strcmp(func_name, "math.fmod") == 0) res = math_fmod(v1, v2);
    else if (strcmp(func_name, "math.remainder") == 0) res = math_remainder(v1, v2);
    else if (strcmp(func_name, "math.trunc") == 0) res = math_trunc(v1);
    else if (strcmp(func_name, "math.modf") == 0) res = math_modf_frac(v1);
    
    // Power and logarithmic
    else if (strcmp(func_name, "math.cbrt") == 0) res = math_cbrt(v1);
    else if (strcmp(func_name, "math.exp") == 0) res = math_exp(v1);
    else if (strcmp(func_name, "math.exp2") == 0) res = math_exp2(v1);
    else if (strcmp(func_name, "math.expm1") == 0) res = math_expm1(v1);
    else if (strcmp(func_name, "math.log") == 0) res = math_log(v1);
    else if (strcmp(func_name, "math.log1p") == 0) res = math_log1p(v1);
    else if (strcmp(func_name, "math.log2") == 0) res = math_log2(v1);
    else if (strcmp(func_name, "math.log10") == 0) res = math_log10(v1);
    else if (strcmp(func_name, "math.pow") == 0) res = math_pow(v1, v2);
    else if (strcmp(func_name, "math.sqrt") == 0) res = math_sqrt(v1);
    
    // Trigonometric
    else if (strcmp(func_name, "math.acos") == 0) res = math_acos(v1);
    else if (strcmp(func_name, "math.asin") == 0) res = math_asin(v1);
    else if (strcmp(func_name, "math.atan") == 0) res = math_atan(v1);
    else if (strcmp(func_name, "math.atan2") == 0) res = math_atan2(v1, v2);
    else if (strcmp(func_name, "math.cos") == 0) res = math_cos(v1);
    else if (strcmp(func_name, "math.sin") == 0) res = math_sin(v1);
    else if (strcmp(func_name, "math.tan") == 0) res = math_tan(v1);
    
    // Hyperbolic
    else if (strcmp(func_name, "math.acosh") == 0) res = math_acosh(v1);
    else if (strcmp(func_name, "math.asinh") == 0) res = math_asinh(v1);
    else if (strcmp(func_name, "math.atanh") == 0) res = math_atanh(v1);
    else if (strcmp(func_name, "math.cosh") == 0) res = math_cosh(v1);
    else if (strcmp(func_name, "math.sinh") == 0) res = math_sinh(v1);
    else if (strcmp(func_name, "math.tanh") == 0) res = math_tanh(v1);
    
    // Special functions
    else if (strcmp(func_name, "math.erf") == 0) res = math_erf(v1);
    else if (strcmp(func_name, "math.erfc") == 0) res = math_erfc(v1);
    else if (strcmp(func_name, "math.gamma") == 0) res = math_gamma(v1);
    else if (strcmp(func_name, "math.lgamma") == 0) res = math_lgamma(v1);
    
    // Angular conversion
    else if (strcmp(func_name, "math.degrees") == 0) res = math_degrees(v1);
    else if (strcmp(func_name, "math.radians") == 0) res = math_radians(v1);
    
    // Floating point manipulation
    else if (strcmp(func_name, "math.copysign") == 0) res = math_copysign(v1, v2);
    else if (strcmp(func_name, "math.isfinite") == 0) res = math_isfinite(v1);
    else if (strcmp(func_name, "math.isinf") == 0) res = math_isinf(v1);
    else if (strcmp(func_name, "math.isnan") == 0) res = math_isnan(v1);
    
    // Distance
    else if (strcmp(func_name, "math.hypot") == 0) res = math_hypot(v1, v2);
    
    else {
        printf(">>> [ERROR] Unknown math function: %s\n", func_name);
        return;
    }
    
    set_number(state, result, res);
}
