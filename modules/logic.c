#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "../include/interface.h"

// ============================================================================
// 1. MEMÓRIA ÉS VÁLTOZÓ KEZELŐ (HELPERS)
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
        strncpy(v->data.str_val, val, 63);
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
// 2. BUILT-IN FUNCTIONS
// ============================================================================

// abs(x) - Abszolút érték
void builtin_abs(EngineState* state, const char* result, const char* arg) {
    double val = resolve_value(state, arg);
    set_number(state, result, fabs(val));
}

// int(x) - Egész számmá alakítás
void builtin_int(EngineState* state, const char* result, const char* arg) {
    double val = resolve_value(state, arg);
    set_number(state, result, (double)(int)val);
}

// float(x) - Lebegőpontos számmá alakítás
void builtin_float(EngineState* state, const char* result, const char* arg) {
    double val = resolve_value(state, arg);
    set_number(state, result, val);
}

// str(x) - Stringgé alakítás
void builtin_str(EngineState* state, const char* result, const char* arg) {
    CustomVar* v = get_var(state, arg);
    char buffer[64];
    if (v && v->type == T_NUMBER) {
        snprintf(buffer, 63, "%.2f", v->data.num_val);
    } else if (v && v->type == T_STRING) {
        strncpy(buffer, v->data.str_val, 63);
    } else {
        snprintf(buffer, 63, "%.2f", resolve_value(state, arg));
    }
    set_string(state, result, buffer);
}

// bool(x) - Boolean értékké alakítás
void builtin_bool(EngineState* state, const char* result, const char* arg) {
    double val = resolve_value(state, arg);
    set_bool(state, result, val != 0.0);
}

// len(x) - String hossza
void builtin_len(EngineState* state, const char* result, const char* arg) {
    CustomVar* v = get_var(state, arg);
    if (v && v->type == T_STRING) {
        set_number(state, result, (double)strlen(v->data.str_val));
    } else if (v && v->type == T_LIST) {
        set_number(state, result, (double)v->data.list_val.size);
    } else {
        set_number(state, result, 0);
    }
}

// max(a, b) - Maximum
void builtin_max(EngineState* state, const char* result, const char* arg1, const char* arg2) {
    double v1 = resolve_value(state, arg1);
    double v2 = resolve_value(state, arg2);
    set_number(state, result, v1 > v2 ? v1 : v2);
}

// min(a, b) - Minimum
void builtin_min(EngineState* state, const char* result, const char* arg1, const char* arg2) {
    double v1 = resolve_value(state, arg1);
    double v2 = resolve_value(state, arg2);
    set_number(state, result, v1 < v2 ? v1 : v2);
}

// pow(x, y) - Hatványozás
void builtin_pow(EngineState* state, const char* result, const char* arg1, const char* arg2) {
    double base = resolve_value(state, arg1);
    double exp = resolve_value(state, arg2);
    set_number(state, result, pow(base, exp));
}

// round(x) - Kerekítés
void builtin_round(EngineState* state, const char* result, const char* arg) {
    double val = resolve_value(state, arg);
    set_number(state, result, round(val));
}

// sum(start, end) - Összegzés tartományban
void builtin_sum(EngineState* state, const char* result, const char* arg1, const char* arg2) {
    int start = (int)resolve_value(state, arg1);
    int end = (int)resolve_value(state, arg2);
    double sum = 0;
    for (int i = start; i <= end; i++) {
        sum += i;
    }
    set_number(state, result, sum);
}

// chr(x) - ASCII kód -> karakter
void builtin_chr(EngineState* state, const char* result, const char* arg) {
    int code = (int)resolve_value(state, arg);
    char buffer[2] = {(char)code, '\0'};
    set_string(state, result, buffer);
}

// ord(x) - Karakter -> ASCII kód
void builtin_ord(EngineState* state, const char* result, const char* arg) {
    CustomVar* v = get_var(state, arg);
    if (v && v->type == T_STRING && strlen(v->data.str_val) > 0) {
        set_number(state, result, (double)(unsigned char)v->data.str_val[0]);
    } else {
        set_number(state, result, 0);
    }
}

// hex(x) - Hexadecimális
void builtin_hex(EngineState* state, const char* result, const char* arg) {
    int val = (int)resolve_value(state, arg);
    char buffer[32];
    snprintf(buffer, 31, "0x%x", val);
    set_string(state, result, buffer);
}

// bin(x) - Bináris
void builtin_bin(EngineState* state, const char* result, const char* arg) {
    int val = (int)resolve_value(state, arg);
    char buffer[64] = "0b";
    int idx = 2;
    for (int i = 31; i >= 0; i--) {
        if ((val >> i) & 1) buffer[idx++] = '1';
        else if (idx > 2) buffer[idx++] = '0';
    }
    if (idx == 2) buffer[idx++] = '0';
    buffer[idx] = '\0';
    set_string(state, result, buffer);
}

// oct(x) - Oktális
void builtin_oct(EngineState* state, const char* result, const char* arg) {
    int val = (int)resolve_value(state, arg);
    char buffer[32];
    snprintf(buffer, 31, "0o%o", val);
    set_string(state, result, buffer);
}

// divmod(a, b) - Osztás és maradék (csak osztást ad vissza)
void builtin_divmod(EngineState* state, const char* result, const char* arg1, const char* arg2) {
    double v1 = resolve_value(state, arg1);
    double v2 = resolve_value(state, arg2);
    if (v2 != 0) {
        set_number(state, result, floor(v1 / v2));
    } else {
        set_number(state, result, 0);
    }
}

// print(x) - Kiírás
void builtin_print(EngineState* state, const char* arg) {
    CustomVar* v = get_var(state, arg);
    if (v) {
        if (v->type == T_NUMBER) {
            printf(">>> PRINT: %.2f\n", v->data.num_val);
        } else if (v->type == T_STRING) {
            printf(">>> PRINT: %s\n", v->data.str_val);
        } else if (v->type == T_BOOL) {
            printf(">>> PRINT: %s\n", v->data.bool_val ? "True" : "False");
        }
    } else {
        printf(">>> PRINT: %s\n", arg);
    }
}

// input() - Felhasználói bemenet (egyszerűsített)
void builtin_input(EngineState* state, const char* result, const char* prompt) {
    printf(">>> INPUT [%s]: ", prompt);
    char buffer[64];
    if (fgets(buffer, 63, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        set_string(state, result, buffer);
    }
}

// type(x) - Típus lekérdezés
void builtin_type(EngineState* state, const char* arg) {
    CustomVar* v = get_var(state, arg);
    printf(">>> TYPE [%s]: ", arg);
    if (!v) printf("<class 'undefined'>\n");
    else if (v->type == T_NUMBER) printf("<class 'float'>\n");
    else if (v->type == T_STRING) printf("<class 'str'>\n");
    else if (v->type == T_BOOL) printf("<class 'bool'>\n");
    else if (v->type == T_LIST) printf("<class 'list'>\n");
    else if (v->type == T_DICT) printf("<class 'dict'>\n");
}

// ============================================================================
// 3. ASSEMBLY SUPPORT (__asm__ és asm)
// ============================================================================

void handle_asm(EngineState* state, const char* asm_code) {
    printf(">>> [ASM] Executing inline assembly: %s\n", asm_code);
    
    // Egyszerű assembly műveletek szimulációja
    char op[32], reg1[32], reg2[32];
    int val;
    
    // MOV művelet: mov eax 10
    if (sscanf(asm_code, "mov %s %d", reg1, &val) == 2) {
        set_number(state, reg1, (double)val);
        printf(">>> [ASM] MOV %s, %d\n", reg1, val);
        return;
    }
    
    // ADD művelet: add eax ebx
    if (sscanf(asm_code, "add %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 + v2);
        printf(">>> [ASM] ADD %s, %s (result: %.0f)\n", reg1, reg2, v1 + v2);
        return;
    }
    
    // SUB művelet: sub eax ebx
    if (sscanf(asm_code, "sub %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 - v2);
        printf(">>> [ASM] SUB %s, %s (result: %.0f)\n", reg1, reg2, v1 - v2);
        return;
    }
    
    // MUL művelet: mul eax ebx
    if (sscanf(asm_code, "mul %s %s", reg1, reg2) == 2) {
        double v1 = resolve_value(state, reg1);
        double v2 = resolve_value(state, reg2);
        set_number(state, reg1, v1 * v2);
        printf(">>> [ASM] MUL %s, %s (result: %.0f)\n", reg1, reg2, v1 * v2);
        return;
    }
    
    // XOR művelet: xor eax eax (nullázás)
    if (sscanf(asm_code, "xor %s %s", reg1, reg2) == 2) {
        if (strcmp(reg1, reg2) == 0) {
            set_number(state, reg1, 0);
            printf(">>> [ASM] XOR %s, %s (zeroed)\n", reg1, reg2);
        } else {
            int v1 = (int)resolve_value(state, reg1);
            int v2 = (int)resolve_value(state, reg2);
            set_number(state, reg1, (double)(v1 ^ v2));
            printf(">>> [ASM] XOR %s, %s (result: %d)\n", reg1, reg2, v1 ^ v2);
        }
        return;
    }
    
    printf(">>> [ASM] Unknown instruction: %s\n", asm_code);
}

// ============================================================================
// 4. NYELVI FUNKCIÓK
// ============================================================================

void handle_typeof(EngineState* state, const char* name) {
    builtin_type(state, name);
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

void execute_line(EngineState* state, char* line);

// ============================================================================
// 5. CIKLUS KEZELÉS
// ============================================================================

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
// 6. SOR VÉGREHAJTÁS
// ============================================================================

void execute_line(EngineState* state, char* line) {
    if (line[0] == '#' || line[0] == '\n') return;

    char target[32], arg1[32], arg2[32], arg3[32], op[8];
    char func_name[32], asm_code[128];

    // Assembly: __asm__("mov eax 10") vagy asm("add eax ebx")
    if (sscanf(line, "__asm__(\"%[^\"]\")", asm_code) == 1 || 
        sscanf(line, "asm(\"%[^\"]\")", asm_code) == 1) {
        handle_asm(state, asm_code);
        return;
    }

    // Built-in functions with 2 args: result = func(arg1, arg2)
    if (sscanf(line, "%s = %[^(](%[^,], %[^)])", target, func_name, arg1, arg2) == 4) {
        trim(func_name); trim(arg1); trim(arg2);
        
        if (strcmp(func_name, "max") == 0) { builtin_max(state, target, arg1, arg2); return; }
        if (strcmp(func_name, "min") == 0) { builtin_min(state, target, arg1, arg2); return; }
        if (strcmp(func_name, "pow") == 0) { builtin_pow(state, target, arg1, arg2); return; }
        if (strcmp(func_name, "sum") == 0) { builtin_sum(state, target, arg1, arg2); return; }
        if (strcmp(func_name, "divmod") == 0) { builtin_divmod(state, target, arg1, arg2); return; }
    }

    // Built-in functions with 1 arg: result = func(arg)
    if (sscanf(line, "%s = %[^(](%[^)])", target, func_name, arg1) == 3) {
        trim(func_name); trim(arg1);
        
        if (strcmp(func_name, "abs") == 0) { builtin_abs(state, target, arg1); return; }
        if (strcmp(func_name, "int") == 0) { builtin_int(state, target, arg1); return; }
        if (strcmp(func_name, "float") == 0) { builtin_float(state, target, arg1); return; }
        if (strcmp(func_name, "str") == 0) { builtin_str(state, target, arg1); return; }
        if (strcmp(func_name, "bool") == 0) { builtin_bool(state, target, arg1); return; }
        if (strcmp(func_name, "len") == 0) { builtin_len(state, target, arg1); return; }
        if (strcmp(func_name, "round") == 0) { builtin_round(state, target, arg1); return; }
        if (strcmp(func_name, "chr") == 0) { builtin_chr(state, target, arg1); return; }
        if (strcmp(func_name, "ord") == 0) { builtin_ord(state, target, arg1); return; }
        if (strcmp(func_name, "hex") == 0) { builtin_hex(state, target, arg1); return; }
        if (strcmp(func_name, "bin") == 0) { builtin_bin(state, target, arg1); return; }
        if (strcmp(func_name, "oct") == 0) { builtin_oct(state, target, arg1); return; }
    }

    // Standalone functions: print(x), type(x), typeof(x)
    if (sscanf(line, "%[^(](%[^)])", func_name, arg1) == 2) {
        trim(func_name); trim(arg1);
        
        if (strcmp(func_name, "print") == 0) { builtin_print(state, arg1); return; }
        if (strcmp(func_name, "type") == 0) { builtin_type(state, arg1); return; }
        if (strcmp(func_name, "typeof") == 0) { handle_typeof(state, arg1); return; }
    }

    // Math Operations: x = a + b
    if (sscanf(line, "%s = %s %1s %s", target, arg1, op, arg2) == 4) {
        handle_math(state, target, arg1, op, arg2);
        return;
    }

    // String Assignment: x = "text"
    char str_val[64];
    if (sscanf(line, "%s = \"%[^\"]\"", target, str_val) == 2) {
        set_string(state, target, str_val);
        return;
    }

    // Simple Assignment: x = y
    if (sscanf(line, "%s = %s", target, arg1) == 2) {
        set_number(state, target, resolve_value(state, arg1));
    }
}

// ============================================================================
// 7. MAIN UPDATE LOOP
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
    printf("\033[H\033[J=== VEKO DYNAMIC ENGINE v2.0 (PYTHON + ASM) ===\n");
    printf("Status: ONLINE | Frame: %d\n", state->info.frame_count);
    printf("--------------------------------------------\n");
    for (int i = 0; i < state->var_count; i++) {
        if (state->vars[i].type == T_NUMBER)
            printf("  (float) %-10s = %.2f\n", state->vars[i].name, state->vars[i].data.num_val);
        else if (state->vars[i].type == T_STRING)
            printf("  (str)   %-10s = \"%s\"\n", state->vars[i].name, state->vars[i].data.str_val);
        else if (state->vars[i].type == T_BOOL)
            printf("  (bool)  %-10s = %s\n", state->vars[i].name, 
                   state->vars[i].data.bool_val ? "True" : "False");
    }
}
