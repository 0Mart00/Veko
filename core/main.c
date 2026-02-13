#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include "../include/interface.h"

const char* MODULE_PATH = "./build/logic.so";

int main() {
    // Allocate EngineState on heap instead of stack (33 MB is too large for stack)
    EngineState* global_state = (EngineState*)calloc(1, sizeof(EngineState));
    if (!global_state) {
        fprintf(stderr, "Failed to allocate memory for EngineState\n");
        return 1;
    }
    
    void* handle = NULL;
    logic_update_fn update = NULL;

    printf("--- Dynamic Logic Engine Started ---\n");

    while (1) {
        // Megpróbáljuk betölteni a modult
        handle = dlopen(MODULE_PATH, RTLD_NOW);
        
        if (!handle) {
            fprintf(stderr, "Waiting for module: %s\n", dlerror());
        } else {
            update = (logic_update_fn)dlsym(handle, "update");
            
            if (update) {
                update(global_state); // Átadjuk a memóriát a modulnak
            }
            
            dlclose(handle); // Bezárjuk, hogy a fájl felülírható legyen
        }

        global_state->info.frame_count++; 
        usleep(500000); 
    }
    
    free(global_state);
    return 0;
}
