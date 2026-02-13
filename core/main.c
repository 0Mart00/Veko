#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
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
    EngineState global_state = {0};
    void* handle = NULL;
    logic_update_fn update = NULL;
    time_t last_mod_time = 0;

    printf("--- Dynamic Logic Engine Started ---\n");

    while (1) {
        time_t current_mod_time = get_file_time(MODULE_PATH);

        // Csak akkor töltjük újra a modult, ha a fájl megváltozott (hot-reload)
        if (current_mod_time > last_mod_time) {
            if (handle) {
                dlclose(handle); // Zárjuk be a régit
                printf(">>> [HOT-RELOAD] Reloading module...\n");
            }
            
            handle = dlopen(MODULE_PATH, RTLD_NOW);
            
            if (!handle) {
                fprintf(stderr, ">>> [ERROR] Waiting for module: %s\n", dlerror());
            } else {
                update = (logic_update_fn)dlsym(handle, "update");
                last_mod_time = current_mod_time;
            }
        }

        // Ha a modul be van töltve, folyamatosan futtatjuk az update-et
        if (update) {
            update(&global_state);
        }

        global_state.info.frame_count++; 
        
        // Ciklus sebessége (16ms = kb. 60 FPS a GUI-nak)
        usleep(16000); 
    }
    return 0;
}