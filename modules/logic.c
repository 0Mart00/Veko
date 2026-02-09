#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/interface.h"

// ============================================================================
// 1. MEMÓRIA ÉS VÁLTOZÓ KEZELŐ (HELPERS)
// ============================================================================

// Változó keresése név alapján
CustomVar* get_var(EngineState* state, const char* name) {
    for (int i = 0; i < state->var_count; i++) {
        if (strcmp(state->vars[i].name, name) == 0) return &state->vars[i];
    }
    return NULL;
}

// Szám típusú változó mentése (Python float szimuláció)
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

// Szöveg típusú változó mentése (Python str szimuláció)
void set_string(EngineState* state, const char* name, const char* val) {
    CustomVar* v = get_var(state, name);
    if (!v && state->var_count < MAX_VARS) {
        v = &state->vars[state->var_count++];
        strncpy(v->name, name, NAME_LEN - 1);
    }
    if (v) {
        v->type = T_STRING;
        strncpy(v->data.str_val, val, 63);
    }
}

// Érték feloldása (visszaadja a számot, ha változó, ha konkrét szám)
double resolve_value(EngineState* state, const char* token) {
    CustomVar* v = get_var(state, token);
    if (v && v->type == T_NUMBER) return v->data.num_val;
    return atof(token);
}

// ============================================================================
// 2. NYELVI FUNKCIÓK (CORE ENGINE)
// ============================================================================

// typeof() implementáció
void handle_typeof(EngineState* state, const char* name) {
    CustomVar* v = get_var(state, name);
    printf(">>> [TYPE CHECK] %s: ", name);
    if (!v) printf("undefined\n");
    else if (v->type == T_NUMBER) printf("<class 'float'>\n");
    else if (v->type == T_STRING) printf("<class 'str'>\n");
}

// OOP Metódus keresés (Öröklődéssel)
CustomFunc* find_method_recursive(EngineState* state, CustomClass* cls, const char* m_name, int args) {
    for (int i = 0; i < cls->method_count; i++) {
        if (strcmp(cls->methods[i].name, m_name) == 0 && cls->methods[i].arg_count == args) {
            return &cls->methods[i];
        }
    }
    if (strlen(cls->parent) > 0) {
        for (int i = 0; i < state->class_count; i++) {
            if (strcmp(state->classes[i].name, cls->parent) == 0) {
                return find_method_recursive(state, &state->classes[i], m_name, args);
            }
        }
    }
    return NULL;
}

// Alapvető műveletek (Python-szerű operátorok)
void handle_math(EngineState* state, char* target, char* arg1, char* op, char* arg2) {
    double v1 = resolve_value(state, arg1);
    double v2 = resolve_value(state, arg2);
    double res = 0;

    switch(op[0]) {
        case '+': res = v1 + v2; break;
        case '-': res = v1 - v2; break;
        case '*': res = v1 * v2; break;
        case '/': res = (v2 != 0) ? v1 / v2 : 0; break;
    }
    set_number(state, target, res);
}

// ============================================================================
// 3. MAIN UPDATE LOOP
// ============================================================================

void update(EngineState* state) {
    FILE *file = fopen("input.txt", "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char target[32], arg1[32], op[2], arg2[32];

        // 1. Reflection: typeof(x)
        if (sscanf(line, "typeof(%[^)])", target) == 1) {
            handle_typeof(state, target);
            continue;
        }

        // 2. Math Operations: x = a + b
        if (sscanf(line, "%s = %s %1s %s", target, arg1, op, arg2) == 4) {
            handle_math(state, target, arg1, op, arg2);
            continue;
        }

        // 3. String Assignment: x = "text"
        char str_val[64];
        if (sscanf(line, "%s = \"%[^\"]\"", target, str_val) == 2) {
            set_string(state, target, str_val);
            continue;
        }

        // 4. Simple Assignment: x = y
        if (sscanf(line, "%s = %s", target, arg1) == 2) {
            set_number(state, target, resolve_value(state, arg1));
        }
    }
    fclose(file);

    // Vizuális kijelzés
    printf("\033[H\033[J=== PYTHON-STYLE DYNAMIC ENGINE (MODULAR) ===\n");
    printf("Status: ONLINE | Frame: %d\n", state->info.frame_count);
    printf("--------------------------------------------\n");
    for (int i = 0; i < state->var_count; i++) {
        if (state->vars[i].type == T_NUMBER)
            printf("  (float) %-10s = %.2f\n", state->vars[i].name, state->vars[i].data.num_val);
        else
            printf("  (str)   %-10s = \"%s\"\n", state->vars[i].name, state->vars[i].data.str_val);
    }
}