#ifndef CLASS_SYSTEM_H
#define CLASS_SYSTEM_H

#include "../../include/interface.h"
#include <stdio.h>

// Class and object management
CustomClass* get_class(EngineState* state, const char* name);
CustomObject* get_object(EngineState* state, const char* name);
void handle_class_definition(EngineState* state, FILE* file, const char* class_name);
void create_object_instance(EngineState* state, const char* obj_name, const char* class_name);
void call_object_method(EngineState* state, const char* obj_name, const char* method_name);

#endif
