#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "../include/interface.h"

const char* MODULE_PATH = "./build/logic.so";

// Függvény a fájl utolsó módosítási idejének lekérdezésére
time_t get_file_time(const char* path) {
    struct stat attr;
    if (stat(path, &attr) == 0) {
        return attr.st_mtime;
    }
    return 0;
}

int main() {
    // Allocate EngineState on heap instead of stack (33 MB is too large for stack)
    EngineState* global_state = (EngineState*)calloc(1, sizeof(EngineState));
    if (!global_state) {
        fprintf(stderr, "Failed to allocate memory for EngineState\n");
        return 1;
    }
    
    void* handle = NULL;
    typedef void (*logic_initialize_fn)(EngineState*);
    typedef void (*logic_mainloop_fn)(EngineState*);
    
    logic_initialize_fn initialize = NULL;
    logic_mainloop_fn mainloop = NULL;

    printf("--- VEKO Engine Started (Event-Driven Mode) ---\n");
    fflush(stdout);

    // Load the logic module
    handle = dlopen(MODULE_PATH, RTLD_NOW);
    
    if (!handle) {
        fprintf(stderr, "Failed to load module: %s\n", dlerror());
        free(global_state);
        return 1;
    }
    
    // Get the initialize and mainloop functions
    initialize = (logic_initialize_fn)dlsym(handle, "initialize");
    mainloop = (logic_mainloop_fn)dlsym(handle, "mainloop");
    
    if (!initialize || !mainloop) {
        fprintf(stderr, "Failed to find initialize/mainloop functions\n");
        dlclose(handle);
        free(global_state);
        return 1;
    }
    
    // Initialize the Veko program (run once)
    printf(">>> [INIT] Running Veko initialization...\n");
    fflush(stdout);
    initialize(global_state);
    
    // Enter the main event loop (GUI takes control)
    printf(">>> [MAINLOOP] Entering event-driven mainloop...\n");
    fflush(stdout);
    mainloop(global_state);
    
    // Cleanup
    printf(">>> [SHUTDOWN] Cleaning up...\n");
    if (handle) {
        dlclose(handle);
    }
    free(global_state);
    return 0;
}
