#include <stdio.h>
#include <string.h>
#include <math.h>
#include "builtins.h"
#include "../core/memory.h"

// Forward declaration
void execute_line(EngineState* state, char* line);

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

void handle_math_op(EngineState* state, char* target, char* arg1, char* op, char* arg2) {
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
