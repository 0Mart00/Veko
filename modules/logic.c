#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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

// Trim whitespace
char* trim(char* str) {
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
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

// Összehasonlítás kiértékelése
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

// Sor végrehajtása (rekurzív híváshoz)
void execute_line(EngineState* state, char* line);

// ============================================================================
// 3. CIKLUS KEZELÉS
// ============================================================================

// FOR ciklus: for i 0 10
void handle_for_loop(EngineState* state, FILE* file, char* var_name, int start, int end) {
    long loop_start = ftell(file);
    char loop_lines[100][128];
    int loop_line_count = 0;
    
    // Ciklus törzsének beolvasása
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        if (strncmp(trimmed, "end", 3) == 0) break;
        
        if (loop_line_count < 100) {
            strncpy(loop_lines[loop_line_count++], line, 127);
        }
    }
    
    // Ciklus végrehajtása
    for (int i = start; i <= end; i++) {
        set_number(state, var_name, (double)i);
        
        for (int j = 0; j < loop_line_count; j++) {
            execute_line(state, loop_lines[j]);
        }
    }
}

// WHILE ciklus: while x < 100
void handle_while_loop(EngineState* state, FILE* file, char* left, char* op, char* right) {
    long loop_start = ftell(file);
    char loop_lines[100][128];
    int loop_line_count = 0;
    
    // Ciklus törzsének beolvasása
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        if (strncmp(trimmed, "end", 3) == 0) break;
        
        if (loop_line_count < 100) {
            strncpy(loop_lines[loop_line_count++], line, 127);
        }
    }
    
    // Ciklus végrehajtása (max 1000 iteráció a végtelen ciklus elkerülésére)
    int iterations = 0;
    while (evaluate_condition(state, left, op, right) && iterations < 1000) {
        for (int j = 0; j < loop_line_count; j++) {
            execute_line(state, loop_lines[j]);
        }
        iterations++;
    }
    
    if (iterations >= 1000) {
        printf(">>> [WARNING] While loop exceeded 1000 iterations, stopped.\n");
    }
}

// ============================================================================
// 4. SOR VÉGREHAJTÁS
// ============================================================================

void execute_line(EngineState* state, char* line) {
    if (line[0] == '#' || line[0] == '\n') return;

    char target[32], arg1[32], op[8], arg2[32];

    // 1. Reflection: typeof(x)
    if (sscanf(line, "typeof(%[^)])", target) == 1) {
        handle_typeof(state, target);
        return;
    }

    // 2. Math Operations: x = a + b
    if (sscanf(line, "%s = %s %1s %s", target, arg1, op, arg2) == 4) {
        handle_math(state, target, arg1, op, arg2);
        return;
    }

    // 3. String Assignment: x = "text"
    char str_val[64];
    if (sscanf(line, "%s = \"%[^\"]\"", target, str_val) == 2) {
        set_string(state, target, str_val);
        return;
    }

    // 4. Simple Assignment: x = y
    if (sscanf(line, "%s = %s", target, arg1) == 2) {
        set_number(state, target, resolve_value(state, arg1));
    }
}

// ============================================================================
// 5. MAIN UPDATE LOOP
// ============================================================================

void update(EngineState* state) {
    FILE *file = fopen("input.txt", "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;

        char var_name[32], left[32], op[8], right[32];
        int start, end;

        // 1. FOR ciklus: for i 0 10
        if (sscanf(trimmed, "for %s %d %d", var_name, &start, &end) == 3) {
            handle_for_loop(state, file, var_name, start, end);
            continue;
        }

        // 2. WHILE ciklus: while x < 100
        if (sscanf(trimmed, "while %s %s %s", left, op, right) == 3) {
            handle_while_loop(state, file, left, op, right);
            continue;
        }

        // 3. END kulcsszó (ciklusok végét jelzi, itt már nem kell kezelni)
        if (strncmp(trimmed, "end", 3) == 0) {
            continue;
        }

        // 4. Egyéb parancsok végrehajtása
        execute_line(state, line);
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
