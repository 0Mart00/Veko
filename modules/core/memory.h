#ifndef MEMORY_H
#define MEMORY_H

#include "../../include/interface.h"

// Variable management
CustomVar* get_var(EngineState* state, const char* name);
void set_number(EngineState* state, const char* name, double val);
void set_string(EngineState* state, const char* name, const char* val);
void set_bool(EngineState* state, const char* name, int val);
double resolve_value(EngineState* state, const char* token);
char* trim(char* str);

#endif
