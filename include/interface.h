#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_VARS 200
#define MAX_FUNCS 50
#define MAX_CLASSES 20
#define MAX_OBJECTS 50
#define MAX_LINES 50
#define MAX_LIST_SIZE 100
#define MAX_IMPORTS 10
#define NAME_LEN 64
#define MAX_LOOP_LINES 100

typedef struct {
    double wealth;
    int frame_count;
} BaseState;

typedef enum {
    T_UNDEFINED = 0,
    T_NUMBER,
    T_STRING,
    T_LIST,
    T_DICT,
    T_BOOL,
    T_OBJECT
} VarType;

// Lista típus
typedef struct {
    double items[MAX_LIST_SIZE];
    int size;
} VarList;

// Dictionary típus
typedef struct {
    char keys[MAX_LIST_SIZE][NAME_LEN];
    double values[MAX_LIST_SIZE];
    int size;
} VarDict;

// Változó struktúra
typedef struct {
    char name[NAME_LEN];
    VarType type;
    
    union {
        double num_val;
        char str_val[128];
        VarList list_val;
        VarDict dict_val;
        int bool_val;
    } data;
} CustomVar;

// Függvény struktúra
typedef struct {
    char name[NAME_LEN];
    char params[10][NAME_LEN];  // Paraméterek
    int param_count;
    char lines[MAX_LINES][128];
    int line_count;
} CustomFunc;

// Osztály struktúra
typedef struct {
    char name[NAME_LEN];
    char parent[NAME_LEN];
    CustomVar attributes[50];   // Osztály attribútumok
    int attr_count;
    CustomFunc methods[MAX_FUNCS];
    int method_count;
    CustomFunc constructor;     // __init__ konstruktor
    int has_constructor;
} CustomClass;

// Objektum példány
typedef struct {
    char name[NAME_LEN];
    char class_name[NAME_LEN];
    CustomVar local_vars[50];   // Objektum lokális változók
    int local_var_count;
} CustomObject;

// Import struktúra
typedef struct {
    char module_name[NAME_LEN];
    int loaded;
} ImportedModule;

// GUI render callback
typedef void (*gui_render_callback_fn)(void* state);

// While loop storage (for GUI mainloop)
typedef struct {
    char left[NAME_LEN];
    char op[8];
    char right[NAME_LEN];
    char lines[MAX_LOOP_LINES][128];
    int line_count;
    int active;
} WhileLoopData;

// Fő állapot
typedef struct {
    BaseState info;
    CustomVar vars[MAX_VARS];
    int var_count;
    CustomClass classes[MAX_CLASSES];
    int class_count;
    CustomObject objects[MAX_OBJECTS];
    int object_count;
    CustomFunc functions[MAX_FUNCS];
    int func_count;
    ImportedModule imports[MAX_IMPORTS];
    int import_count;
    
    // GUI render callback (called every frame in GUI mainloop)
    gui_render_callback_fn gui_render_callback;
    void* gui_render_data;
    
    // While loop data (for GUI mainloop rendering)
    WhileLoopData gui_loop;
} EngineState;

typedef void (*logic_update_fn)(EngineState* state);

#endif
