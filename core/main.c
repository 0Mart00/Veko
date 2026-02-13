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
    logic_update_fn update = NULL;
    time_t last_mod_time = 0;

    printf("--- Dynamic Logic Engine Started ---\n");
    fflush(stdout);

    while (1) {
        time_t current_mod_time = get_file_time(MODULE_PATH);

        // Csak akkor töltjük újra a modult, ha a fájl megváltozott (hot-reload)
        if (current_mod_time > last_mod_time) {
            if (handle) {
                dlclose(handle); // Zárjuk be a régit
                printf(">>> [HOT-RELOAD] Reloading module...\n");
                fflush(stdout);
            }
            
            handle = dlopen(MODULE_PATH, RTLD_NOW);
            
            if (!handle) {
                fprintf(stderr, "Waiting for module: %s\n", dlerror());
                fflush(stderr);
            } else {
                update = (logic_update_fn)dlsym(handle, "update");
                last_mod_time = current_mod_time;
            }
        }

        // Call update every frame (not just when module loads!)
        if (update) {
            update(global_state);
        }

        global_state->info.frame_count++; 
        usleep(500000); 
    }
    
    if (handle) {
        dlclose(handle);
    }
    free(global_state);
    return 0;
}
