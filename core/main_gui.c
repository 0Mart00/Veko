#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "../include/interface.h"

const char* MODULE_PATH = "./build/logic.so";

// Function to get file modification time
time_t get_file_time(const char* path) {
    struct stat attr;
    if (stat(path, &attr) == 0) {
        return attr.st_mtime;
    }
    return 0;
}

// Function pointers for GUI module
typedef void (*gui_init_fn)(EngineState*);
typedef void (*gui_mainloop_fn)(EngineState*);
typedef void (*gui_quit_fn)(EngineState*);
typedef int (*gui_is_running_fn)(EngineState*);
typedef void (*logic_update_fn)(EngineState*);

int main() {
    // Allocate EngineState on heap
    EngineState* global_state = (EngineState*)calloc(1, sizeof(EngineState));
    if (!global_state) {
        fprintf(stderr, "Failed to allocate memory for EngineState\n");
        return 1;
    }
    
    void* handle = NULL;
    logic_update_fn update = NULL;
    gui_init_fn gui_init = NULL;
    gui_mainloop_fn gui_mainloop = NULL;
    gui_quit_fn gui_quit = NULL;
    gui_is_running_fn gui_is_running = NULL;
    time_t last_mod_time = 0;

    printf("--- Veko Dynamic Engine v3.0 (GUI Mode) ---\n");
    fflush(stdout);

    // Load module initially
    handle = dlopen(MODULE_PATH, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "Failed to load module: %s\n", dlerror());
        free(global_state);
        return 1;
    }

    // Load function pointers
    update = (logic_update_fn)dlsym(handle, "update");
    gui_init = (gui_init_fn)dlsym(handle, "gui_init");
    gui_mainloop = (gui_mainloop_fn)dlsym(handle, "gui_mainloop");
    gui_quit = (gui_quit_fn)dlsym(handle, "gui_quit");
    gui_is_running = (gui_is_running_fn)dlsym(handle, "gui_is_running");
    
    if (!update) {
        fprintf(stderr, "Failed to find 'update' function in module\n");
        dlclose(handle);
        free(global_state);
        return 1;
    }

    last_mod_time = get_file_time(MODULE_PATH);

    // Execute input.txt once to initialize GUI
    printf(">>> [GUI] Executing input.txt to initialize GUI...\n");
    update(global_state);
    
    // Check if GUI was initialized
    if (!gui_is_running || !gui_is_running(global_state)) {
        printf(">>> [GUI] No GUI initialized. Make sure input.txt contains:\n");
        printf("    gui.init()\n");
        printf("    gui.window(\"Title\", 800, 600)\n");
        dlclose(handle);
        free(global_state);
        return 1;
    }

    printf(">>> [GUI] Starting GUI mainloop...\n");
    fflush(stdout);

    // GUI mainloop with hot-reload support
    if (gui_mainloop) {
        // Run GUI mainloop (blocking)
        gui_mainloop(global_state);
    } else {
        fprintf(stderr, ">>> [GUI ERROR] gui_mainloop not found in module\n");
    }

    // Cleanup
    if (gui_quit) {
        gui_quit(global_state);
    }
    
    if (handle) {
        dlclose(handle);
    }
    free(global_state);
    
    printf(">>> [GUI] Engine stopped\n");
    return 0;
}
