#ifndef BUILTINS_H
#define BUILTINS_H

#include "../../include/interface.h"

// Built-in functions
void builtin_abs(EngineState* state, const char* result, const char* arg);
void builtin_print(EngineState* state, const char* arg);
void builtin_type(EngineState* state, const char* arg);

// Assembly support
void handle_asm(EngineState* state, const char* asm_code);

// Control flow
int evaluate_condition(EngineState* state, const char* left, const char* op, const char* right);
void handle_math_op(EngineState* state, char* target, char* arg1, char* op, char* arg2);
void handle_for_loop(EngineState* state, FILE* file, char* var_name, int start, int end);
void handle_while_loop(EngineState* state, FILE* file, char* left, char* op, char* right);

#endif
