#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/interface.h"

void set_var(EngineState* state, const char* name, double val) {
    for (int i = 0; i < state->var_count; i++) {
        if (strcmp(state->vars[i].name, name) == 0) {
            state->vars[i].value = val;
            return;
        }
    }
    if (state->var_count < MAX_VARS) {
        strncpy(state->vars[state->var_count].name, name, NAME_LEN - 1);
        state->vars[state->var_count].value = val;
        state->var_count++;
    }
}

// Rekurzív metóduskeresés öröklődéshez
CustomFunc* find_method(EngineState* state, CustomClass* cls, const char* m_name, int args) {
    for (int i = 0; i < cls->method_count; i++) {
        if (strcmp(cls->methods[i].name, m_name) == 0 && cls->methods[i].arg_count == args) {
            return &cls->methods[i];
        }
    }
    // Ha van szülő, keressük ott
    if (strlen(cls->parent) > 0) {
        for (int i = 0; i < state->class_count; i++) {
            if (strcmp(state->classes[i].name, cls->parent) == 0) {
                return find_method(state, &state->classes[i], m_name, args);
            }
        }
    }
    return NULL;
}

void execute_method(EngineState* state, CustomFunc* method) {
    for (int i = 0; i < method->line_count; i++) {
        char c[16], n[32]; double v;
        if (sscanf(method->lines[i], " %s %s %lf", c, n, &v) == 3) {
            if (strcmp(c, "SET") == 0) set_var(state, n, v);
        }
    }
}

void update(EngineState* state) {
    FILE *file = fopen("input.txt", "r");
    if (!file) return;

    state->class_count = 0;
    char line[128];
    CustomClass* current_class = NULL;
    CustomFunc* current_method = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char name[32], parent[32];
        int args = 0;

        // CLASS Definíció öröklődéssel: CLASS Titan : Spaceship
        if (sscanf(line, "CLASS %s : %s", name, parent) == 2) {
            current_class = &state->classes[state->class_count++];
            strncpy(current_class->name, name, NAME_LEN - 1);
            strncpy(current_class->parent, parent, NAME_LEN - 1);
            current_class->method_count = 0;
            continue;
        } else if (sscanf(line, "CLASS %s", name) == 1) {
            current_class = &state->classes[state->class_count++];
            strncpy(current_class->name, name, NAME_LEN - 1);
            current_class->parent[0] = '\0';
            current_class->method_count = 0;
            continue;
        }

        if (sscanf(line, " FUNC %s %d", name, &args) == 2) {
            if (current_class) {
                current_method = &current_class->methods[current_class->method_count++];
                strncpy(current_method->name, name, NAME_LEN - 1);
                current_method->arg_count = args;
                current_method->line_count = 0;
            }
            continue;
        }

        if (strstr(line, "END_FUNC")) { current_method = NULL; continue; }
        if (strstr(line, "END_CLASS")) { current_class = NULL; continue; }

        if (current_method) {
            strncpy(current_method->lines[current_method->line_count++], line, 127);
        } else {
            // NEW hívás rekurzív kereséssel
            char c_n[32], m_n[32]; float p;
            int f = sscanf(line, "NEW %s %s %f", c_n, m_n, &p);
            if (f >= 2) {
                for (int i = 0; i < state->class_count; i++) {
                    if (strcmp(state->classes[i].name, c_n) == 0) {
                        CustomFunc* m = find_method(state, &state->classes[i], m_n, (f == 3 ? 1 : 0));
                        if (m) execute_method(state, m);
                    }
                }
            }
        }
    }
    fclose(file);

    printf("\033[H\033[J=== ENGINE STATUS: %d CLASSES ===\n", state->class_count);
    for (int i = 0; i < state->var_count; i++) 
        printf("  %s: %.2f\n", state->vars[i].name, state->vars[i].value);
}