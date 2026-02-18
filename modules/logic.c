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
    char gui_title[128], gui_arg1[64], gui_arg2[64], gui_arg3[64], gui_arg4[64];
    if (sscanf(line, "gui.window(\"%[^\"]\", %[^,], %[^)])", gui_title, gui_arg1, gui_arg2) == 3) {
        trim(gui_arg1);
        trim(gui_arg2);
        int width = atoi(gui_arg1);
        int height = atoi(gui_arg2);
        gui_window_create(state, gui_title, width, height);
        return;
    }

    // gui.label("text")
    if (sscanf(line, "gui.label(\"%[^\"]\")", gui_title) == 1) {
        gui_label(state, gui_title);
        return;
    }

    // gui.label_colored("text", r, g, b)
    if (sscanf(line, "gui.label_colored(\"%[^\"]\", %[^,], %[^,], %[^)])", gui_title, gui_arg1, gui_arg2, gui_arg3) == 4) {
        trim(gui_arg1); trim(gui_arg2); trim(gui_arg3);
        float r = atof(gui_arg1), g = atof(gui_arg2), b = atof(gui_arg3);
        gui_label_colored(state, gui_title, r, g, b);
        return;
    }

    // gui.button("text", "callback")
    char gui_callback[128];
    if (sscanf(line, "gui.button(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_callback) == 2) {
        gui_button(state, gui_title, gui_callback);
        return;
    }

    // gui.button_colored("text", "callback", r, g, b)
    if (sscanf(line, "gui.button_colored(\"%[^\"]\", \"%[^\"]\", %[^,], %[^,], %[^)])", gui_title, gui_callback, gui_arg1, gui_arg2, gui_arg3) == 5) {
        trim(gui_arg1); trim(gui_arg2); trim(gui_arg3);
        float r = atof(gui_arg1), g = atof(gui_arg2), b = atof(gui_arg3);
        gui_button_colored(state, gui_title, gui_callback, r, g, b);
        return;
    }

    // gui.textbox("label", "var_name", width)
    char gui_varname[128];
    if (sscanf(line, "gui.textbox(\"%[^\"]\", \"%[^\"]\", %[^)])", gui_title, gui_varname, gui_arg1) == 3) {
        trim(gui_arg1);
        int width = atoi(gui_arg1);
        gui_textbox(state, gui_title, gui_varname, width);
        return;
    }

    // gui.textbox_multiline("label", "var_name", width, height)
    if (sscanf(line, "gui.textbox_multiline(\"%[^\"]\", \"%[^\"]\", %[^,], %[^)])", gui_title, gui_varname, gui_arg1, gui_arg2) == 4) {
        trim(gui_arg1); trim(gui_arg2);
        int width = atoi(gui_arg1), height = atoi(gui_arg2);
        gui_textbox_multiline(state, gui_title, gui_varname, width, height);
        return;
    }

    // gui.inputfield("label", "var_name")
    if (sscanf(line, "gui.inputfield(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_varname) == 2) {
        gui_inputfield(state, gui_title, gui_varname);
        return;
    }

    // gui.slider("var_name", min, max)
    if (sscanf(line, "gui.slider(\"%[^\"]\", %[^,], %[^)])", gui_varname, gui_arg1, gui_arg2) == 3) {
        trim(gui_arg1); trim(gui_arg2);
        double min = atof(gui_arg1), max = atof(gui_arg2);
        gui_slider(state, gui_varname, min, max);
        return;
    }

    // gui.slider_int("var_name", min, max)
    if (sscanf(line, "gui.slider_int(\"%[^\"]\", %[^,], %[^)])", gui_varname, gui_arg1, gui_arg2) == 3) {
        trim(gui_arg1); trim(gui_arg2);
        int min = atoi(gui_arg1), max = atoi(gui_arg2);
        gui_slider_int(state, gui_varname, min, max);
        return;
    }

    // gui.slider_angle("var_name")
    if (sscanf(line, "gui.slider_angle(\"%[^\"]\")", gui_varname) == 1) {
        gui_slider_angle(state, gui_varname);
        return;
    }

    // gui.checkbox("text", "var_name")
    if (sscanf(line, "gui.checkbox(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_varname) == 2) {
        gui_checkbox(state, gui_title, gui_varname);
        return;
    }

    // gui.toggle("label", "var_name")
    if (sscanf(line, "gui.toggle(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_varname) == 2) {
        gui_toggle(state, gui_title, gui_varname);
        return;
    }

    // gui.progressbar(fraction, width, height)
    if (sscanf(line, "gui.progressbar(%[^,], %[^,], %[^)])", gui_arg1, gui_arg2, gui_arg3) == 3) {
        trim(gui_arg1); trim(gui_arg2); trim(gui_arg3);
        float fraction = atof(gui_arg1);
        int width = atoi(gui_arg2), height = atoi(gui_arg3);
        gui_progressbar(state, fraction, width, height);
        return;
    }

    // gui.progressbar_labeled("label", fraction)
    if (sscanf(line, "gui.progressbar_labeled(\"%[^\"]\", %[^)])", gui_title, gui_arg1) == 2) {
        trim(gui_arg1);
        float fraction = atof(gui_arg1);
        gui_progressbar_labeled(state, gui_title, fraction);
        return;
    }

    // gui.frame_begin("title")
    if (sscanf(line, "gui.frame_begin(\"%[^\"]\")", gui_title) == 1) {
        gui_frame_begin(state, gui_title);
        return;
    }

    // gui.panel_begin("title", width, height)
    if (sscanf(line, "gui.panel_begin(\"%[^\"]\", %[^,], %[^)])", gui_title, gui_arg1, gui_arg2) == 3) {
        trim(gui_arg1); trim(gui_arg2);
        int width = atoi(gui_arg1), height = atoi(gui_arg2);
        gui_panel_begin(state, gui_title, width, height);
        return;
    }

    // gui.table_begin("id", columns)
    if (sscanf(line, "gui.table_begin(\"%[^\"]\", %[^)])", gui_title, gui_arg1) == 2) {
        trim(gui_arg1);
        int columns = atoi(gui_arg1);
        gui_table_begin(state, gui_title, columns);
        return;
    }

    // gui.table_header("label")
    if (sscanf(line, "gui.table_header(\"%[^\"]\")", gui_title) == 1) {
        gui_table_header(state, gui_title);
        return;
    }

    // gui.table_cell("text")
    if (sscanf(line, "gui.table_cell(\"%[^\"]\")", gui_title) == 1) {
        gui_table_cell(state, gui_title);
        return;
    }

    // gui.menu_item("text", "callback")
    if (sscanf(line, "gui.menu_item(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_callback) == 2) {
        gui_menu_item(state, gui_title, gui_callback);
        return;
    }

    // gui.messagebox("title", "message")
    char gui_message[256];
    if (sscanf(line, "gui.messagebox(\"%[^\"]\", \"%[^\"]\")", gui_title, gui_message) == 2) {
        gui_messagebox(state, gui_title, gui_message);
        return;
    }

    // gui.askstring("title", "prompt", "var_name")
    char gui_prompt[256];
    if (sscanf(line, "gui.askstring(\"%[^\"]\", \"%[^\"]\", \"%[^\"]\")", gui_title, gui_prompt, gui_varname) == 3) {
        gui_askstring(state, gui_title, gui_prompt, gui_varname);
        return;
    }

    // gui.confirm_dialog("title", "message", "callback_yes", "callback_no")
    char gui_cb_yes[128], gui_cb_no[128];
    if (sscanf(line, "gui.confirm_dialog(\"%[^\"]\", \"%[^\"]\", \"%[^\"]\", \"%[^\"]\")", gui_title, gui_message, gui_cb_yes, gui_cb_no) == 4) {
        gui_confirm_dialog(state, gui_title, gui_message, gui_cb_yes, gui_cb_no);
        return;
    }

    // gui.set_color(r, g, b, a)
    if (sscanf(line, "gui.set_color(%[^,], %[^,], %[^,], %[^)])", gui_arg1, gui_arg2, gui_arg3, gui_arg4) == 4) {
        trim(gui_arg1); trim(gui_arg2); trim(gui_arg3); trim(gui_arg4);
        float r = atof(gui_arg1), g = atof(gui_arg2), b = atof(gui_arg3), a = atof(gui_arg4);
        gui_set_color(state, r, g, b, a);
        return;
    }

    // gui.set_font_size(size)
    if (sscanf(line, "gui.set_font_size(%[^)])", gui_arg1) == 1) {
        trim(gui_arg1);
        float size = atof(gui_arg1);
        gui_set_font_size(state, size);
        return;
    }

    // gui.indent(amount)
    if (sscanf(line, "gui.indent(%[^)])", gui_arg1) == 1) {
        trim(gui_arg1);
        float amount = atof(gui_arg1);
        gui_indent(state, amount);
        return;
    }

    // gui.unindent(amount)
    if (sscanf(line, "gui.unindent(%[^)])", gui_arg1) == 1) {
        trim(gui_arg1);
        float amount = atof(gui_arg1);
        gui_unindent(state, amount);
        return;
    }

    // gui.help_marker("text")
    if (sscanf(line, "gui.help_marker(\"%[^\"]\")", gui_title) == 1) {
        gui_help_marker(state, gui_title);
        return;
    }

    // gui.tooltip("text")
    if (sscanf(line, "gui.tooltip(\"%[^\"]\")", gui_title) == 1) {
        gui_tooltip(state, gui_title);
        return;
    }

    // GUI module method call: gui.method() - parameterless methods
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

// ============================================================================
// EXECUTION MODE DETECTION
// ============================================================================

typedef enum {
    MODE_TERMINAL,
    MODE_GUI
} ExecutionMode;

static ExecutionMode detect_execution_mode(EngineState* state) {
    // Check if GUI module is imported
    for (int i = 0; i < state->import_count; i++) {
        if (strcmp(state->imports[i].module_name, "gui") == 0) {
            // GUI module is imported, assume GUI mode
            return MODE_GUI;
        }
    }
    return MODE_TERMINAL;
}

// ============================================================================
// SCRIPT EXECUTION (Pure logic, no display)
// ============================================================================

static int execute_script_file(EngineState* state, const char* filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open script file: %s\n", filepath);
        return -1;
    }

    char line[256];  // Increased buffer size for longer lines
    int line_number = 0;
    int error_count = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        // Trim whitespace
        char* trimmed = trim(line);
        
        // Skip empty lines and comments
        if (trimmed[0] == '#' || trimmed[0] == '\0') {
            continue;
        }

        // Parse control structures
        char var_name[64], left[64], op[8], right[64], class_name[64];
        int start, end;

        // Class definition: class ClassName:
        if (sscanf(trimmed, "class %[^:]:", class_name) == 1) {
            handle_class_definition(state, file, class_name);
            continue;
        }

        // FOR loop: for var start end
        if (sscanf(trimmed, "for %s %d %d", var_name, &start, &end) == 3) {
            handle_for_loop(state, file, var_name, start, end);
            continue;
        }

        // WHILE loop: while left op right
        if (sscanf(trimmed, "while %s %s %s", left, op, right) == 3) {
            handle_while_loop(state, file, left, op, right);
            continue;
        }

        // END keyword (loop/class terminator)
        if (strncmp(trimmed, "end", 3) == 0) {
            continue;
        }

        // Execute regular commands
        execute_line(state, line);
    }

    fclose(file);
    return error_count;
}

// ============================================================================
// DISPLAY RENDERING (Separated from execution logic)
// ============================================================================

static void render_terminal_display(EngineState* state) {
    // Clear screen and show header
    printf("\033[H\033[J=== VEKO DYNAMIC ENGINE v3.0 (MODULAR) ===\n");
    printf("Status: ONLINE | Frame: %d\n", state->info.frame_count);
    printf("Classes: %d | Objects: %d | Imports: %d\n", 
           state->class_count, state->object_count, state->import_count);
    printf("--------------------------------------------\n");
    fflush(stdout);

    // Display variables (limit to 20 for readability)
    int display_limit = (state->var_count < 20) ? state->var_count : 20;
    
    for (int i = 0; i < display_limit; i++) {
        switch (state->vars[i].type) {
            case T_NUMBER:
                printf("  (float) %-15s = %.4f\n", 
                       state->vars[i].name, 
                       state->vars[i].data.num_val);
                break;
            
            case T_STRING:
                printf("  (str)   %-15s = \"%s\"\n", 
                       state->vars[i].name, 
                       state->vars[i].data.str_val);
                break;
            
            case T_BOOL:
                printf("  (bool)  %-15s = %s\n", 
                       state->vars[i].name, 
                       state->vars[i].data.bool_val ? "True" : "False");
                break;
            
            default:
                break;
        }
    }

    // Show overflow indicator
    if (state->var_count > 20) {
        printf("  ... and %d more variables\n", state->var_count - 20);
    }
    
    fflush(stdout);
}

// ============================================================================
// MAIN UPDATE FUNCTION (Clean separation of concerns)
// ============================================================================

void update(EngineState* state) {
    if (!state) {
        fprintf(stderr, "[ERROR] update() called with NULL state\n");
        return;
    }

    // Detect execution mode
    ExecutionMode mode = detect_execution_mode(state);

    // Execute script file
    int result = execute_script_file(state, "input.txt");
    if (result < 0) {
        fprintf(stderr, "[ERROR] Script execution failed\n");
        return;
    }

    // Increment frame counter
    state->info.frame_count++;

    // Render display ONLY in terminal mode
    // In GUI mode, rendering is handled by ImGui in the mainloop
    if (mode == MODE_TERMINAL) {
        render_terminal_display(state);
    }
}
