#ifndef INTERFACE_H
#define INTERFACE_H

// Ez a struktúra tárolja az adatokat, amik nem vesznek el kódcsere közben
typedef struct {
    double wealth;
    float cruiser_position[3];
    int frame_count;
    int reload_count;
} EngineState;

// A függvény szignatúra, amit a modulban megvalósítasz
typedef void (*logic_update_fn)(EngineState* state);

#endif