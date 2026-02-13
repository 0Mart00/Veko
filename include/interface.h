#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_VARS 100
#define MAX_FUNCS 50
#define MAX_CLASSES 10
#define MAX_OBJECTS 50
#define MAX_LINES 20
#define MAX_LIST_SIZE 100
#define NAME_LEN 32

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
    T_BOOL
} VarType;

// Lista típus
typedef struct {
    double items[MAX_LIST_SIZE];
    int size;
} VarList;

// Dictionary típus (egyszerűsített)
typedef struct {
    char keys[MAX_LIST_SIZE][NAME_LEN];
    double values[MAX_LIST_SIZE];
    int size;
} VarDict;

typedef struct {
    char name[NAME_LEN];
    VarType type;
    
    union {
        double num_val;
        char str_val[64];
        VarList list_val;
        VarDict dict_val;
        int bool_val;
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
    char parent[NAME_LEN];
    CustomFunc methods[MAX_FUNCS];
    int method_count;
} CustomClass;

typedef struct {
    char name[NAME_LEN];
    char class_name[NAME_LEN];
    double local_vars[10];
} CustomObject;

typedef struct {
    BaseState info;
    CustomVar vars[MAX_VARS];
    int var_count;
    CustomClass classes[MAX_CLASSES];
    int class_count;
    CustomObject objects[MAX_OBJECTS];
    int object_count;
} EngineState;

typedef void (*logic_update_fn)(EngineState* state);

#endif
