#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "core/memory.h"
#include "math/math_module.h"
#include "oop/class_system.h"
#include "builtins/builtins.h"
#include "gui/gui_module.h"
// Forward declarations
void execute_line(EngineState* state, char* line);
void gui_render_loop_body(void* data);

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
        }
        else if (strcmp(func_name, "gui") == 0) {
        import_gui_module(state); // Ezt a függvényt a gui_module.h-ból kell hívni
    }
        return;
    }
    // GUI function calls with parameters
    char gui_func[64], gui_arg1[128], gui_arg2[64], gui_arg3[64];
    
    // gui_window_create("title", width, height)
    int width, height;
    if (sscanf(line, "gui_window_create(\"%[^\"]\", %d, %d)", gui_arg1, &width, &height) == 3) {
        gui_window_create(state, gui_arg1, width, height);
        return;
    }
    
    // gui_frame_begin("title")
    if (sscanf(line, "gui_frame_begin(\"%[^\"]\")", gui_arg1) == 1) {
        gui_frame_begin(state, gui_arg1);
        return;
    }
    
    // gui_label("text")
    if (sscanf(line, "gui_label(\"%[^\"]\")", gui_arg1) == 1) {
        gui_label(state, gui_arg1);
        return;
    }
    
    // gui_slider("var_name", min, max)
    double min_val, max_val;
    if (sscanf(line, "gui_slider(\"%[^\"]\", %lf, %lf)", gui_arg1, &min_val, &max_val) == 3) {
        gui_slider(state, gui_arg1, min_val, max_val);
        return;
    }
    
    // gui_separator()
    if (strncmp(line, "gui_separator()", 15) == 0) {
        gui_separator(state);
        return;
    }
    
    // gui_frame_end()
    if (strncmp(line, "gui_frame_end()", 15) == 0) {
        gui_frame_end(state);
        return;
    }
    
    // 2. A gui_init() és egyéb direkt hívások kezelése (paraméter nélküli)
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
// INITIALIZATION (Run once at startup)
// ============================================================================

void initialize(EngineState* state) {
    printf(">>> [VEKO] Initializing program from input.txt...\n");
    fflush(stdout);
    
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        fprintf(stderr, ">>> [ERROR] Cannot open input.txt\n");
        return;
    }

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

        // WHILE loop - Store for mainloop execution
        if (sscanf(trimmed, "while %s %s %s", left, op, right) == 3) {
            printf(">>> [VEKO] While loop detected, storing for mainloop...\n");
            
            // Store loop condition
            strncpy(state->gui_loop.left, left, NAME_LEN - 1);
            strncpy(state->gui_loop.op, op, 7);
            strncpy(state->gui_loop.right, right, NAME_LEN - 1);
            state->gui_loop.line_count = 0;
            state->gui_loop.active = 1;
            
            // Read loop body
            char loop_line[128];
            while (fgets(loop_line, sizeof(loop_line), file)) {
                char* loop_trimmed = trim(loop_line);
                if (loop_trimmed[0] == '#' || loop_trimmed[0] == '\0') continue;
                if (strncmp(loop_trimmed, "end", 3) == 0) break;
                
                if (state->gui_loop.line_count < MAX_LOOP_LINES) {
                    strncpy(state->gui_loop.lines[state->gui_loop.line_count++], loop_line, 127);
                }
            }
            
            printf(">>> [VEKO] Stored %d lines in GUI loop body\n", state->gui_loop.line_count);
            
            // Set up GUI render callback
            state->gui_render_callback = gui_render_loop_body;
            state->gui_render_data = state;
            
            fclose(file);
            return; // Stop initialization, mainloop will take over
        }

        // END keyword
        if (strncmp(trimmed, "end", 3) == 0) continue;

        // Execute other commands
        execute_line(state, line);
    }
    fclose(file);

    printf(">>> [VEKO] Initialization complete\n");
    printf("    Variables: %d | Classes: %d | Objects: %d | Imports: %d\n", 
           state->var_count, state->class_count, state->object_count, state->import_count);
    fflush(stdout);
}

// ============================================================================
// GUI RENDER CALLBACK (Called every frame by GUI mainloop)
// ============================================================================

void gui_render_loop_body(void* data) {
    EngineState* state = (EngineState*)data;
    
    if (!state || !state->gui_loop.active) {
        return;
    }
    
    // Check loop condition
    if (!evaluate_condition(state, state->gui_loop.left, state->gui_loop.op, state->gui_loop.right)) {
        // Condition false, stop rendering
        state->gui_loop.active = 0;
        return;
    }
    
    // Execute loop body (GUI commands)
    for (int i = 0; i < state->gui_loop.line_count; i++) {
        execute_line(state, state->gui_loop.lines[i]);
    }
}

// ============================================================================
// MAIN EVENT LOOP (Event-driven execution)
// ============================================================================

void mainloop(EngineState* state) {
    printf(">>> [VEKO] Entering mainloop...\n");
    fflush(stdout);
    
    // Check if GUI is initialized
    int gui_active = 0;
    for (int i = 0; i < state->import_count; i++) {
        if (strcmp(state->imports[i].module_name, "gui") == 0) {
            gui_active = 1;
            break;
        }
    }
    
    if (gui_active) {
        // GUI mainloop takes control
        printf(">>> [VEKO] GUI detected, starting GUI mainloop...\n");
        fflush(stdout);
        gui_mainloop(state);
    } else {
        // No GUI, just display final state
        printf("\n=== VEKO PROGRAM FINISHED ===\n");
        printf("Final State:\n");
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
        printf("--------------------------------------------\n");
    }
}
