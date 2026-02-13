#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "memory.h"

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
