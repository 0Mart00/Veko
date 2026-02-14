#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "core/memory.h"
#include "math/math_module.h"
#include "oop/class_system.h"
#include "builtins/builtins.h"
#include "gui/gui_module.h"
// Forward declaration
void execute_line(EngineState* state, char* line);

// ============================================================================
// LINE EXECUTION
// ============================================================================

void execute_line(EngineState* state, char* line) {
    if (line[0] == '#' || line[0] == '\n') return;

    char target[64], arg1[64], arg2[64], op[8];
    char func_name[64], asm_code[128], class_name[64], obj_name[64], method_name[64];

    // Import: import math
    if (sscanf(line, "import %s", func_name) == 1) {
        if (strcmp(func_name, "math") == 0) {
            import_math_module(state);
        } else if (strcmp(func_name, "gui") == 0) {
            import_gui_module(state);
        }
        return;
    }
    // 2. A gui_init() és egyéb direkt hívások kezelése
    if (sscanf(line, "gui_%[^(]()", method_name) == 1) {
        // Itt hívod meg a gui_module-ban definiált funkciókat
        handle_gui_call(state, method_name); 
        return;
    }
    // Assembly
    if (sscanf(line, "__asm__(\"%[^\"]\")", asm_code) == 1 || 
        sscanf(line, "asm(\"%[^\"]\")", asm_code) == 1) {
        handle_asm(state, asm_code);
        return;
    }

    // Math module functions with 2 args
    if (sscanf(line, "%s = math.%[^(](%[^,], %[^)])", target, func_name, arg1, arg2) == 4) {
        char full_name[128];
        snprintf(full_name, 127, "math.%s", func_name);
        trim(arg1); trim(arg2);
        call_math_function(state, full_name, target, arg1, arg2);
        return;
    }

    // Math module functions with 1 arg
    if (sscanf(line, "%s = math.%[^(](%[^)])", target, func_name, arg1) == 3) {
        char full_name[128];
        snprintf(full_name, 127, "math.%s", func_name);
        trim(arg1);
        call_math_function(state, full_name, target, arg1, NULL);
        return;
    }

    // GUI module method call with parameters: gui.window("title", width, height)
    char gui_title[128], gui_arg1[64], gui_arg2[64];
    if (sscanf(line, "gui.window(\"%[^\"]\", %[^,], %[^)])", gui_title, gui_arg1, gui_arg2) == 3) {
        trim(gui_arg1);
        trim(gui_arg2);
        int width = atoi(gui_arg1);
        int height = atoi(gui_arg2);
        gui_window_create(state, gui_title, width, height);
        return;
    }

    // GUI module method call: gui.method()
    if (sscanf(line, "gui.%[^(]()", method_name) == 1) {
        handle_gui_call(state, method_name);
        return;
    }

    // Object method call: obj.method()
    if (sscanf(line, "%[^.].%[^(]()", obj_name, method_name) == 2) {
        call_object_method(state, obj_name, method_name);
        return;
    }

    // Object creation: obj = ClassName()
    // Additional check: line must end with () to be valid object creation
    int line_len = strlen(line);
    if (sscanf(line, "%s = %[^(]()", obj_name, class_name) == 2 &&
        line_len >= 2 && 
        line[line_len-1] == ')' && line[line_len-2] == '(') {
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
        handle_math_op(state, target, arg1, op, arg2);
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
// MAIN UPDATE LOOP
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
    printf("\033[H\033[J=== VEKO DYNAMIC ENGINE v3.0 (MODULAR) ===\n");
    printf("Status: ONLINE | Frame: %d\n", state->info.frame_count);
    printf("Classes: %d | Objects: %d | Imports: %d\n", 
           state->class_count, state->object_count, state->import_count);
    printf("--------------------------------------------\n");
    fflush(stdout);
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
