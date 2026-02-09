#include <stdio.h>
#include "../include/interface.h"

// Fontos: a név egyezzen a dlsym hívással a core-ban
void update(EngineState* state) {
    state->wealth += 1.5; // Növeljük a vagyont minden ticknél
    
    printf("\n[LOGIC UPDATED]\n");
    printf("Wealth: %.2f\n", state->wealth);
    printf("Frame: %d\n", state->frame_count);
    printf("--- Edit this file to see changes live! ---\n");
}