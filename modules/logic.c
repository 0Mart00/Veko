#include <stdio.h>
#include "../include/interface.h"

void update(EngineState* state) {
    state->wealth += 1.5; 
    printf("alma");
    
    // Egy egyszerű vizuális "scanner" a vagyonod növekedéséhez
    printf("\033[H\033[J"); // Képernyő törlése (terminál escape kód)
    printf("=== DYNAMIC LOGIC ENGINE SCANNER ===\n");
    printf("Status: ONLINE | Target: Global Dominance\n");
    printf("------------------------------------\n");
    
    printf("Wealth:   [%.2f USD]\n", state->wealth);
    printf("Frame:    [%d]\n", state->frame_count);
    
    // Szimulált "Six Eyes" adatfolyam
    printf("Patterns: ");
    for(int i = 0; i < (int)(state->wealth / 10) % 20; i++) {
        printf("█"); // Ez vizuálisan mutatja a vagyonod "súlyát"
    }
    
    printf("\n\n[LOG: %s]\n", "hotdog"); // Most már van pontosvessző!
    printf("------------------------------------\n");
}