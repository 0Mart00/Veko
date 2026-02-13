#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_VARS 100
#define MAX_FUNCS 50
#define MAX_CLASSES 10
#define MAX_OBJECTS 50  // Új: ennyi aktív objektumunk lehet
#define MAX_LINES 20
#define NAME_LEN 32

typedef struct {
    double wealth;
    int frame_count;
} BaseState;

typedef enum {
    T_UNDEFINED = 0,
    T_NUMBER,
    T_STRING
} VarType;

typedef struct {
    char name[NAME_LEN];
    VarType type;
    
    // Union: vagy számot, vagy szöveget tárol, egyszerre csak egyet
    union {
        double num_val;
        char str_val[64];
    } data;
} CustomVar;
typedef struct {
    char name[NAME_LEN];
    char lines[MAX_LINES][128];
    int line_count;
    int arg_count; 
} CustomFunc;

typedef struct {
    char name[NAME_LEN];
    char parent[NAME_LEN]; // Öröklődéshez
    CustomFunc methods[MAX_FUNCS];
    int method_count;
} CustomClass;

// Új: Egy konkrét objektum példány
typedef struct {
    char name[NAME_LEN];
    char class_name[NAME_LEN];
    double local_vars[10]; // Objektum-szintű adatok
} CustomObject;

typedef struct {
    BaseState info;
    CustomVar vars[MAX_VARS];
    int var_count;
    CustomClass classes[MAX_CLASSES];
    int class_count;
    CustomObject objects[MAX_OBJECTS]; // Objektum tároló
    int object_count;
} EngineState;



typedef void (*logic_update_fn)(EngineState* state);

#endif