#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include "../include/interface.h"

const char* MODULE_PATH = "./build/logic.so";

int main() {
    EngineState global_state = {0}; // Itt lakik az összes adatod
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
                update(&global_state); // Átadjuk a memóriát a modulnak
            }
            
            dlclose(handle); // Bezárjuk, hogy a fájl felülírható legyen
        }

        global_state.frame_count++;
        usleep(500000); // 0.5 másodperces ciklusidő
    }

    return 0;
}