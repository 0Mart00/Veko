#include <stdio.h>
#include <string.h>
#include "class_system.h"
#include "../core/memory.h"

// Forward declaration
void execute_line(EngineState* state, char* line);

CustomClass* get_class(EngineState* state, const char* name) {
    for (int i = 0; i < state->class_count; i++) {
        if (strcmp(state->classes[i].name, name) == 0) {
            return &state->classes[i];
        }
    }
    return NULL;
}

CustomObject* get_object(EngineState* state, const char* name) {
    for (int i = 0; i < state->object_count; i++) {
        if (strcmp(state->objects[i].name, name) == 0) {
            return &state->objects[i];
        }
    }
    return NULL;
}

void handle_class_definition(EngineState* state, FILE* file, const char* class_name) {
    if (state->class_count >= MAX_CLASSES) {
        printf(">>> [ERROR] Maximum classes reached\n");
        return;
    }
    
    CustomClass* cls = &state->classes[state->class_count++];
    strncpy(cls->name, class_name, NAME_LEN - 1);
    cls->method_count = 0;
    cls->attr_count = 0;
    cls->has_constructor = 0;
    cls->parent[0] = '\0';
    
    char line[128];
    int in_method = 0;
    CustomFunc* current_method = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim(line);
        
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        if (strncmp(trimmed, "end", 3) == 0) {
            if (in_method) {
                in_method = 0;
                current_method = NULL;
            } else {
                break;
            }
            continue;
        }
        
        char method_name[NAME_LEN], params_str[256];
        if (sscanf(trimmed, "def %[^(](%[^)])", method_name, params_str) == 2) {
            trim(method_name);
            
            if (strcmp(method_name, "__init__") == 0) {
                cls->has_constructor = 1;
                current_method = &cls->constructor;
            } else {
                if (cls->method_count < MAX_FUNCS) {
                    current_method = &cls->methods[cls->method_count++];
                }
            }
            
            if (current_method) {
                strncpy(current_method->name, method_name, NAME_LEN - 1);
                current_method->line_count = 0;
                current_method->param_count = 0;
                
                char* token = strtok(params_str, ",");
                while (token && current_method->param_count < 10) {
                    token = trim(token);
                    strncpy(current_method->params[current_method->param_count++], 
                           token, NAME_LEN - 1);
                    token = strtok(NULL, ",");
                }
            }
            
            in_method = 1;
            continue;
        }
        
        if (in_method && current_method && current_method->line_count < MAX_LINES) {
            strncpy(current_method->lines[current_method->line_count++], 
                   line, 127);
        }
    }
    
    printf(">>> [CLASS] %s defined with %d methods\n", class_name, cls->method_count);
}

void create_object_instance(EngineState* state, const char* obj_name, const char* class_name) {
    CustomClass* cls = get_class(state, class_name);
    if (!cls) {
        printf(">>> [ERROR] Class '%s' not found\n", class_name);
        return;
    }
    
    if (state->object_count >= MAX_OBJECTS) {
        printf(">>> [ERROR] Maximum objects reached\n");
        return;
    }
    
    CustomObject* obj = &state->objects[state->object_count++];
    strncpy(obj->name, obj_name, NAME_LEN - 1);
    strncpy(obj->class_name, class_name, NAME_LEN - 1);
    obj->local_var_count = 0;
    
    if (cls->has_constructor) {
        printf(">>> [OBJECT] %s created from class %s (constructor called)\n", 
               obj_name, class_name);
    } else {
        printf(">>> [OBJECT] %s created from class %s\n", obj_name, class_name);
    }
}

void call_object_method(EngineState* state, const char* obj_name, const char* method_name) {
    CustomObject* obj = get_object(state, obj_name);
    if (!obj) {
        printf(">>> [ERROR] Object '%s' not found\n", obj_name);
        return;
    }
    
    CustomClass* cls = get_class(state, obj->class_name);
    if (!cls) {
        printf(">>> [ERROR] Class '%s' not found\n", obj->class_name);
        return;
    }
    
    CustomFunc* method = NULL;
    for (int i = 0; i < cls->method_count; i++) {
        if (strcmp(cls->methods[i].name, method_name) == 0) {
            method = &cls->methods[i];
            break;
        }
    }
    
    if (!method) {
        printf(">>> [ERROR] Method '%s' not found in class '%s'\n", 
               method_name, cls->name);
        return;
    }
    
    printf(">>> [METHOD] %s.%s() called\n", obj_name, method_name);
    for (int i = 0; i < method->line_count; i++) {
        execute_line(state, method->lines[i]);
    }
}
