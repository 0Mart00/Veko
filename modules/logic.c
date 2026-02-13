#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "../include/interface.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_TAU
#define M_TAU 6.28318530717958647692
#endif

// Forward declarations
void execute_line(EngineState* state, char* line);
char* trim(char* str);

// ============================================================================
// 1. MEMÓRIA ÉS VÁLTOZÓ KEZELŐ
// ============================================================================

CustomVar* get_var(EngineState* state, const char* name) {
    for (int i = 0; i < state->var_count; i++) {
        if (strcmp(state->vars[i].name, name) == 0) return &state->vars[i];
    }
    return NULL;
}

void set_number(EngineState* state, const char* name, double val) {
    CustomVar* v = get_var(state, name);
    if (!v && state->var_count < MAX_VARS) {
        v = &state->vars[state->var_count++];
        strncpy(v->name, name, NAME_LEN - 1);
    }
    if (v) {
        v->type = T_NUMBER;
        v->data.num_val = val;
    }
}

void set_string(EngineState* state, const char* name, const char* val) {
    CustomVar* v = get_var(state, name);
    if (!v && state->var_count < MAX_VARS) {
        v = &state->vars[state->var_count++];
        strncpy(v->name, name, NAME_LEN - 1);
    }
    if (v) {
        v->type = T_STRING;
        strncpy(v->data.str_val, val, 127);
    }
}

void set_bool(EngineState* state, const char* name, int val) {
    CustomVar* v = get_var(state, name);
    if (!v && state->var_count < MAX_VARS) {
        v = &state->vars[state->var_count++];
        strncpy(v->name, name, NAME_LEN - 1);
    }
    if (v) {
        v->type = T_BOOL;
        v->data.bool_val = val;
    }
}

double resolve_value(EngineState* state, const char* token) {
    CustomVar* v = get_var(state, token);
    if (v && v->type == T_NUMBER) return v->data.num_val;
    if (v && v->type == T_BOOL) return v->data.bool_val ? 1.0 : 0.0;
    return atof(token);
}

char* trim(char* str) {
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// ============================================================================
// 2. MATH MODULE - Number-theoretic functions
// ============================================================================

// factorial(n)
double math_factorial(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

// gcd(a, b)
int math_gcd(int a, int b) {
    a = abs(a); b = abs(b);
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// lcm(a, b)
int math_lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return abs(a * b) / math_gcd(a, b);
}

// isqrt(n) - Integer square root
int math_isqrt(int n) {
    if (n < 0) return 0;
    return (int)sqrt((double)n);
}

// comb(n, k) - Combinations
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

// perm(n, k) - Permutations
double math_perm(int n, int k) {
    if (k > n || k < 0) return 0;
    double result = 1;
    for (int i = 0; i < k; i++) {
        result *= (n - i);
    }
    return result;
}

// ============================================================================
// 3. MATH MODULE - Floating point arithmetic
// ============================================================================

double math_ceil(double x) { return ceil(x); }
double math_fabs(double x) { return fabs(x); }
double math_floor(double x) { return floor(x); }
double math_fma(double x, double y, double z) { return (x * y) + z; }
double math_fmod(double x, double y) { return fmod(x, y); }
double math_remainder(double x, double y) { return remainder(x, y); }
double math_trunc(double x) { return trunc(x); }

// modf(x) - returns fractional part
double math_modf_frac(double x) {
    double intpart;
    return modf(x, &intpart);
}

// ============================================================================
// 4. MATH MODULE - Power, exponential and logarithmic
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
// 5. MATH MODULE - Trigonometric functions
// ============================================================================

double math_acos(double x) { return acos(x); }
double math_asin(double x) { return asin(x); }
double math_atan(double x) { return atan(x); }
double math_atan2(double y, double x) { return atan2(y, x); }
double math_cos(double x) { return cos(x); }
double math_sin(double x) { return sin(x); }
double math_tan(double x) { return tan(x); }

// ============================================================================
// 6. MATH MODULE - Hyperbolic functions
// ============================================================================

double math_acosh(double x) { return acosh(x); }
double math_asinh(double x) { return asinh(x); }
double math_atanh(double x) { return atanh(x); }
double math_cosh(double x) { return cosh(x); }
double math_sinh(double x) { return sinh(x); }
double math_tanh(double x) { return tanh(x); }

// ============================================================================
// 7. MATH MODULE - Special functions
// ============================================================================

double math_erf(double x) { return erf(x); }
double math_erfc(double x) { return erfc(x); }
double math_gamma(double x) { return tgamma(x); }
double math_lgamma(double x) { return lgamma(x); }

// ============================================================================
// 8. MATH MODULE - Angular conversion
// ============================================================================

double math_degrees(double x) { return x * 180.0 / M_PI; }
double math_radians(double x) { return x * M_PI / 180.0; }

// ============================================================================
// 9. MATH MODULE - Floating point manipulation
// ============================================================================

double math_copysign(double x, double y) { return copysign(x, y); }
int math_isfinite(double x) { return isfinite(x); }
int math_isinf(double x) { return isinf(x); }
int math_isnan(double x) { return isnan(x); }

// ============================================================================
// 10. MATH MODULE - Distance and summation
// ============================================================================

double math_hypot(double x, double y) { return hypot(x, y); }

// ============================================================================
// 11. IMPORT SYSTEM
// ============================================================================

void import_math_module(EngineState* state) {
    // Check if already imported
    for (int i = 0; i < state->import_count; i++) {
        if (strcmp(state->imports[i].module_name, "math") == 0) {
            return; // Already imported
        }
    }
    
    // Add to imports
    if (state->import_count < MAX_IMPORTS) {
        strncpy(state->imports[state->import_count].module_name, "math", NAME_LEN - 1);
        state->imports[state->import_count].loaded = 1;
        state->import_count++;
    }
    
    // Load math constants
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
// 12. MATH FUNCTION DISPATCHER
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

// ============================================================================
// 13. CLASS SYSTEM
// ============================================================================

CustomClass* get_class(EngineState* state, const char* name) {
    for (int i = 0; i < state->class_count; i++) {
        if (strcmp(state->classes[i].name, name) == 0) {
            return &state->classes[i];
        }
    }
    return NULL;
}

CustomObject* get_object(EngineState* state, const char* name) {
    for (int i = 0; i < state->object_count; i++) {
        if (strcmp(state->objects[i].name, name) == 0) {
            return &state->objects[i];
        }
    }
    return NULL;
}

// Handle class definition
void handle_class_definition(EngineState* state, FILE* file, const char* class_name) {
    if (state->class_count >= MAX_CLASSES) {
        printf(">>> [ERROR] Maximum classes reached\n");
        return;
    }
    
    CustomClass* cls = &state->classes[state->class_count++];
    strncpy(cls->name, class_name, NAME_LEN - 1);
    cls->method_count = 0;
    cls->attr_count = 0;
    cls->has_constructor = 0;
    cls->parent[0] = '\0';
    
    char line[128];
    int in_method = 0;
    CustomFunc* current_method = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        
        // Skip empty lines and comments
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        // End of class
        if (strncmp(trimmed, "end", 3) == 0) {
            if (in_method) {
                in_method = 0;
                current_method = NULL;
            } else {
                break;
            }
            continue;
        }
        
        // Method definition: def method_name(self, param1, param2):
        char method_name[NAME_LEN], params_str[256];
        if (sscanf(trimmed, "def %[^(](%[^)])", method_name, params_str) == 2) {
            trim(method_name);
            
            // Check if it's __init__ constructor
            if (strcmp(method_name, "__init__") == 0) {
                cls->has_constructor = 1;
                current_method = &cls->constructor;
            } else {
                if (cls->method_count < MAX_FUNCS) {
                    current_method = &cls->methods[cls->method_count++];
                }
            }
            
            if (current_method) {
                strncpy(current_method->name, method_name, NAME_LEN - 1);
                current_method->line_count = 0;
                current_method->param_count = 0;
                
                // Parse parameters
                char* token = strtok(params_str, ",");
                while (token && current_method->param_count < 10) {
                    token = trim(token);
                    strncpy(current_method->params[current_method->param_count++], 
                           token, NAME_LEN - 1);
                    token = strtok(NULL, ",");
                }
            }
            
            in_method = 1;
            continue;
        }
        
        // Method body
        if (in_method && current_method && current_method->line_count < MAX_LINES) {
            strncpy(current_method->lines[current_method->line_count++], 
                   line, 127);
        }
    }
    
    printf(">>> [CLASS] %s defined with %d methods\n", class_name, cls->method_count);
}

// Create object instance
void create_object_instance(EngineState* state, const char* obj_name, const char* class_name) {
    CustomClass* cls = get_class(state, class_name);
    if (!cls) {
        printf(">>> [ERROR] Class '%s' not found\n", class_name);
        return;
    }
    
    if (state->object_count >= MAX_OBJECTS) {
        printf(">>> [ERROR] Maximum objects reached\n");
        return;
    }
    
    CustomObject* obj = &state->objects[state->object_count++];
    strncpy(obj->name, obj_name, NAME_LEN - 1);
    strncpy(obj->class_name, class_name, NAME_LEN - 1);
    obj->local_var_count = 0;
    
    // Call constructor if exists
    if (cls->has_constructor) {
        printf(">>> [OBJECT] %s created from class %s (constructor called)\n", 
               obj_name, class_name);
    } else {
        printf(">>> [OBJECT] %s created from class %s\n", obj_name, class_name);
    }
}

// Call object method
void call_object_method(EngineState* state, const char* obj_name, const char* method_name) {
    CustomObject* obj = get_object(state, obj_name);
    if (!obj) {
        printf(">>> [ERROR] Object '%s' not found\n", obj_name);
        return;
    }
    
    CustomClass* cls = get_class(state, obj->class_name);
    if (!cls) {
        printf(">>> [ERROR] Class '%s' not found\n", obj->class_name);
        return;
    }
    
    // Find method
    CustomFunc* method = NULL;
    for (int i = 0; i < cls->method_count; i++) {
        if (strcmp(cls->methods[i].name, method_name) == 0) {
            method = &cls->methods[i];
            break;
        }
    }
    
    if (!method) {
        printf(">>> [ERROR] Method '%s' not found in class '%s'\n", 
               method_name, cls->name);
        return;
    }
    
    // Execute method lines
    printf(">>> [METHOD] %s.%s() called\n", obj_name, method_name);
    for (int i = 0; i < method->line_count; i++) {
        execute_line(state, method->lines[i]);
    }
}

// ============================================================================
// 14. BUILT-IN FUNCTIONS (from previous version)
// ============================================================================

void builtin_abs(EngineState* state, const char* result, const char* arg) {
    set_number(state, result, fabs(resolve_value(state, arg)));
}

void builtin_print(EngineState* state, const char* arg) {
    CustomVar* v = get_var(state, arg);
    if (v) {
        if (v->type == T_NUMBER) printf(">>> PRINT: %.2f\n", v->data.num_val);
        else if (v->type == T_STRING) printf(">>> PRINT: %s\n", v->data.str_val);
        else if (v->type == T_BOOL) printf(">>> PRINT: %s\n", v->data.bool_val ? "True" : "False");
    } else {
        printf(">>> PRINT: %s\n", arg);
    }
}

void builtin_type(EngineState* state, const char* arg) {
    CustomVar* v = get_var(state, arg);
    printf(">>> TYPE [%s]: ", arg);
    if (!v) printf("<class 'undefined'>\n");
    else if (v->type == T_NUMBER) printf("<class 'float'>\n");
    else if (v->type == T_STRING) printf("<class 'str'>\n");
    else if (v->type == T_BOOL) printf("<class 'bool'>\n");
    else if (v->type == T_OBJECT) printf("<class 'object'>\n");
}

// ============================================================================
// 15. ASSEMBLY SUPPORT
// ============================================================================

void handle_asm(EngineState* state, const char* asm_code) {
    printf(">>> [ASM] %s\n", asm_code);
    
    char reg1[32], reg2[32];
    int val;
    
    if (sscanf(asm_code, "mov %s %d", reg1, &val) == 2) {
        set_number(state, reg1, (double)val);
        return;
    }
    
    if (sscanf(asm_code, "add %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 + v2);
        return;
    }
    
    if (sscanf(asm_code, "sub %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 - v2);
        return;
    }
    
    if (sscanf(asm_code, "mul %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 * v2);
        return;
    }
    
    if (sscanf(asm_code, "xor %s %s", reg1, reg2) == 2) {
        if (strcmp(reg1, reg2) == 0) {
            set_number(state, reg1, 0);
        } else {
            int v1 = (int)resolve_value(state, reg1);
            int v2 = (int)resolve_value(state, reg2);
            set_number(state, reg1, (double)(v1 ^ v2));
        }
        return;
    }
}

// ============================================================================
// 16. CONTROL FLOW
// ============================================================================

int evaluate_condition(EngineState* state, const char* left, const char* op, const char* right) {
    double l = resolve_value(state, left);
    double r = resolve_value(state, right);
    
    if (strcmp(op, "<") == 0) return l < r;
    if (strcmp(op, ">") == 0) return l > r;
    if (strcmp(op, "<=") == 0) return l <= r;
    if (strcmp(op, ">=") == 0) return l >= r;
    if (strcmp(op, "==") == 0) return l == r;
    if (strcmp(op, "!=") == 0) return l != r;
    
    return 0;
}

void handle_math(EngineState* state, char* target, char* arg1, char* op, char* arg2) {
    double v1 = resolve_value(state, arg1);
    double v2 = resolve_value(state, arg2);
    double res = 0;

    switch(op[0]) {
        case '+': res = v1 + v2; break;
        case '-': res = v1 - v2; break;
        case '*': res = v1 * v2; break;
        case '/': res = (v2 != 0) ? v1 / v2 : 0; break;
        case '%': res = fmod(v1, v2); break;
    }
    set_number(state, target, res);
}

void handle_for_loop(EngineState* state, FILE* file, char* var_name, int start, int end) {
    char loop_lines[100][128];
    int loop_line_count = 0;
    
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        if (strncmp(trimmed, "end", 3) == 0) break;
        if (loop_line_count < 100) {
            strncpy(loop_lines[loop_line_count++], line, 127);
        }
    }
    
    for (int i = start; i <= end; i++) {
        set_number(state, var_name, (double)i);
        for (int j = 0; j < loop_line_count; j++) {
            execute_line(state, loop_lines[j]);
        }
    }
}

void handle_while_loop(EngineState* state, FILE* file, char* left, char* op, char* right) {
    char loop_lines[100][128];
    int loop_line_count = 0;
    
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        if (strncmp(trimmed, "end", 3) == 0) break;
        if (loop_line_count < 100) {
            strncpy(loop_lines[loop_line_count++], line, 127);
        }
    }
    
    int iterations = 0;
    while (evaluate_condition(state, left, op, right) && iterations < 1000) {
        for (int j = 0; j < loop_line_count; j++) {
            execute_line(state, loop_lines[j]);
        }
        iterations++;
    }
    
    if (iterations >= 1000) {
        printf(">>> [WARNING] While loop exceeded 1000 iterations\n");
    }
}

// ============================================================================
// 17. LINE EXECUTION
// ============================================================================

void execute_line(EngineState* state, char* line) {
    if (line[0] == '#' || line[0] == '\n') return;

    char target[64], arg1[64], arg2[64], op[8];
    char func_name[64], asm_code[128], class_name[64], obj_name[64], method_name[64];

    // Import: import math
    if (sscanf(line, "import %s", func_name) == 1) {
        if (strcmp(func_name, "math") == 0) {
            import_math_module(state);
        }
        return;
    }

    // Assembly
    if (sscanf(line, "__asm__(\"%[^\"]\")", asm_code) == 1 || 
        sscanf(line, "asm(\"%[^\"]\")", asm_code) == 1) {
        handle_asm(state, asm_code);
        return;
    }

    // Math module functions with 2 args: result = math.func(arg1, arg2)
    if (sscanf(line, "%s = math.%[^(](%[^,], %[^)])", target, func_name, arg1, arg2) == 4) {
        char full_name[128];
        snprintf(full_name, 127, "math.%s", func_name);
        trim(arg1); trim(arg2);
        call_math_function(state, full_name, target, arg1, arg2);
        return;
    }

    // Math module functions with 1 arg: result = math.func(arg)
    if (sscanf(line, "%s = math.%[^(](%[^)])", target, func_name, arg1) == 3) {
        char full_name[128];
        snprintf(full_name, 127, "math.%s", func_name);
        trim(arg1);
        call_math_function(state, full_name, target, arg1, NULL);
        return;
    }

    // Object method call: obj.method()
    if (sscanf(line, "%[^.].%[^(]()", obj_name, method_name) == 2) {
        call_object_method(state, obj_name, method_name);
        return;
    }

    // Object creation: obj = ClassName()
    if (sscanf(line, "%s = %[^(]()", obj_name, class_name) == 2) {
        create_object_instance(state, obj_name, class_name);
        return;
    }

    // Built-in functions
    if (sscanf(line, "%s = abs(%[^)])", target, arg1) == 2) {
        builtin_abs(state, target, arg1);
        return;
    }

    if (sscanf(line, "print(%[^)])", arg1) == 1) {
        builtin_print(state, arg1);
        return;
    }

    if (sscanf(line, "type(%[^)])", arg1) == 1) {
        builtin_type(state, arg1);
        return;
    }

    // Math operations: x = a + b
    if (sscanf(line, "%s = %s %1s %s", target, arg1, op, arg2) == 4) {
        handle_math(state, target, arg1, op, arg2);
        return;
    }

    // String assignment: x = "text"
    char str_val[128];
    if (sscanf(line, "%s = \"%[^\"]\"", target, str_val) == 2) {
        set_string(state, target, str_val);
        return;
    }

    // Simple assignment: x = y
    if (sscanf(line, "%s = %s", target, arg1) == 2) {
        set_number(state, target, resolve_value(state, arg1));
    }
}

// ============================================================================
// 18. MAIN UPDATE LOOP
// ============================================================================

void update(EngineState* state) {
    FILE *file = fopen("input.txt", "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;

        char var_name[64], left[64], op[8], right[64], class_name[64];
        int start, end;

        // Class definition: class ClassName:
        if (sscanf(trimmed, "class %[^:]:", class_name) == 1) {
            handle_class_definition(state, file, class_name);
            continue;
        }

        // FOR loop
        if (sscanf(trimmed, "for %s %d %d", var_name, &start, &end) == 3) {
            handle_for_loop(state, file, var_name, start, end);
            continue;
        }

        // WHILE loop
        if (sscanf(trimmed, "while %s %s %s", left, op, right) == 3) {
            handle_while_loop(state, file, left, op, right);
            continue;
        }

        // END keyword
        if (strncmp(trimmed, "end", 3) == 0) continue;

        // Execute other commands
        execute_line(state, line);
    }
    fclose(file);

    // Display
    printf("\033[H\033[J=== VEKO DYNAMIC ENGINE v3.0 (OOP + MATH) ===\n");
    printf("Status: ONLINE | Frame: %d\n", state->info.frame_count);
    printf("Classes: %d | Objects: %d | Imports: %d\n", 
           state->class_count, state->object_count, state->import_count);
    printf("--------------------------------------------\n");
    for (int i = 0; i < state->var_count && i < 20; i++) {
        if (state->vars[i].type == T_NUMBER)
            printf("  (float) %-15s = %.4f\n", state->vars[i].name, state->vars[i].data.num_val);
        else if (state->vars[i].type == T_STRING)
            printf("  (str)   %-15s = \"%s\"\n", state->vars[i].name, state->vars[i].data.str_val);
        else if (state->vars[i].type == T_BOOL)
            printf("  (bool)  %-15s = %s\n", state->vars[i].name, 
                   state->vars[i].data.bool_val ? "True" : "False");
    }
    if (state->var_count > 20) {
        printf("  ... and %d more variables\n", state->var_count - 20);
    }
}
