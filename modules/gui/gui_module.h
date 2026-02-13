#ifndef GUI_MODULE_H
#define GUI_MODULE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "../../include/interface.h"
#include <SDL2/SDL.h>

// GUI State
typedef struct {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int running;
    int width;
    int height;
    char title[128];
} GUIState;

void import_gui_module(EngineState* state);
void handle_gui_call(EngineState* state, char* method_name);

// ============================================================================
// CORE SYSTEM
// ============================================================================
void gui_init(EngineState* state);
void gui_mainloop(EngineState* state);
void gui_quit(EngineState* state);
void gui_update(EngineState* state);
int gui_is_running(EngineState* state);

// ============================================================================
// 1. KONTÉNEREK ÉS RENDSZEREZŐK (Strukturális elemek)
// ============================================================================

// Window (Ablak) - Legfelső szintű lebegő konténer
void gui_window_create(EngineState* state, const char* title, int width, int height);
void gui_window_title(EngineState* state, const char* title);
void gui_window_geometry(EngineState* state, int width, int height);
void gui_window_begin(EngineState* state, const char* title, int* open);
void gui_window_end(EngineState* state);

// Panel / Frame (Keret) - Logikailag összetartozó elemek csoportosítása
void gui_frame_begin(EngineState* state, const char* title);
void gui_frame_end(EngineState* state);
void gui_panel_begin(EngineState* state, const char* title, int width, int height);
void gui_panel_end(EngineState* state);

// ScrollBox (Görgethető terület) - Automatikus görgetősáv
void gui_scrollbox_begin(EngineState* state, const char* id, int width, int height);
void gui_scrollbox_end(EngineState* state);

// TabView (Lapozó) - Több felület ugyanazon a területen
void gui_tabview_begin(EngineState* state, const char* id);
int gui_tabview_item(EngineState* state, const char* label);
void gui_tabview_end(EngineState* state);

// Splitter (Elválasztó) - Dinamikus átméretezés
void gui_splitter_vertical(EngineState* state, const char* id, float* ratio);
void gui_splitter_horizontal(EngineState* state, const char* id, float* ratio);

// ============================================================================
// 2. ADATMEGJELENÍTŐK (Vizuális kimenet)
// ============================================================================

// Label (Címke/Szöveg) - Nem szerkeszthető szöveg
void gui_label(EngineState* state, const char* text);
void gui_label_colored(EngineState* state, const char* text, float r, float g, float b);

// 3D Viewport / Canvas (Vászon) - OpenGL/Vulkan renderelés
void gui_viewport_begin(EngineState* state, const char* id, int width, int height);
void gui_viewport_end(EngineState* state);
void gui_canvas_begin(EngineState* state, const char* id, int width, int height);
void gui_canvas_end(EngineState* state);

// Graph / Plot (Grafikon) - Valós idejű adatvizualizáció
void gui_plot_lines(EngineState* state, const char* label, float* values, int count);
void gui_plot_histogram(EngineState* state, const char* label, float* values, int count);
void gui_plot_scatter(EngineState* state, const char* label, float* x, float* y, int count);

// ProgressBar (Folyamatjelző) - Számítási állapot
void gui_progressbar(EngineState* state, float fraction, int width, int height);
void gui_progressbar_labeled(EngineState* state, const char* label, float fraction);

// DataGrid / Table (Adattábla) - Mátrixszerű adatmegjelenítés
void gui_table_begin(EngineState* state, const char* id, int columns);
void gui_table_header(EngineState* state, const char* label);
void gui_table_row(EngineState* state);
void gui_table_cell(EngineState* state, const char* text);
void gui_table_end(EngineState* state);

// ============================================================================
// 3. INTERAKTÍV BEVITELI ELEMEK (Adatbemenet)
// ============================================================================

// TextBox / InputField (Szövegmező) - Parancsok, kódok bevitele
void gui_textbox(EngineState* state, const char* label, const char* var_name, int width);
void gui_textbox_multiline(EngineState* state, const char* label, const char* var_name, int width, int height);
void gui_inputfield(EngineState* state, const char* label, const char* var_name);

// Slider (Csúszka) - Folyamatos paraméter állítás
void gui_slider(EngineState* state, const char* var_name, double min, double max);
void gui_slider_int(EngineState* state, const char* var_name, int min, int max);
void gui_slider_angle(EngineState* state, const char* var_name);

// SpinBox (Számláló) - Numerikus finomhangolás
void gui_spinbox(EngineState* state, const char* var_name, double min, double max, double step);
void gui_spinbox_int(EngineState* state, const char* var_name, int min, int max, int step);

// CheckBox / ToggleSwitch (Jelölőnégyzet / Kapcsoló) - Kétállapotú logika
void gui_checkbox(EngineState* state, const char* text, const char* var_name);
void gui_toggle(EngineState* state, const char* label, const char* var_name);

// ComboBox / Dropdown (Legördülő lista) - Előre definiált opciók
void gui_combobox(EngineState* state, const char* var_name, const char* items);
void gui_dropdown(EngineState* state, const char* label, const char* var_name, const char** items, int count);

// ============================================================================
// 4. AKCIÓ VEZÉRLŐK
// ============================================================================

// Button (Gomb) - Azonnali esemény kiváltás
void gui_button(EngineState* state, const char* text, const char* callback);
void gui_button_colored(EngineState* state, const char* text, const char* callback, float r, float g, float b);

// IconButton (Ikonos gomb) - Vizuális piktogram
void gui_iconbutton(EngineState* state, const char* icon, const char* callback);
void gui_iconbutton_tooltip(EngineState* state, const char* icon, const char* tooltip, const char* callback);

// Toolbar (Eszköztár) - Gyorsbillentyűk
void gui_toolbar_begin(EngineState* state);
void gui_toolbar_button(EngineState* state, const char* icon, const char* callback);
void gui_toolbar_separator(EngineState* state);
void gui_toolbar_end(EngineState* state);

// ============================================================================
// LAYOUT RENDSZER
// ============================================================================
void gui_pack(EngineState* state);
void gui_grid(EngineState* state, int row, int col);
void gui_sameline(EngineState* state);
void gui_separator(EngineState* state);
void gui_spacing(EngineState* state);
void gui_indent(EngineState* state, float amount);
void gui_unindent(EngineState* state, float amount);

// ============================================================================
// MENU RENDSZER
// ============================================================================
void gui_menu_begin(EngineState* state);
void gui_menu_item(EngineState* state, const char* text, const char* callback);
void gui_menu_separator(EngineState* state);
void gui_menu_end(EngineState* state);

// ============================================================================
// DIALOG BOXES
// ============================================================================
void gui_messagebox(EngineState* state, const char* title, const char* message);
void gui_askstring(EngineState* state, const char* title, const char* prompt, const char* var_name);
void gui_confirm_dialog(EngineState* state, const char* title, const char* message, const char* callback_yes, const char* callback_no);

// ============================================================================
// STYLING ÉS TÉMÁK
// ============================================================================
void gui_set_color(EngineState* state, float r, float g, float b, float a);
void gui_set_font_size(EngineState* state, float size);
void gui_theme_dark(EngineState* state);
void gui_theme_light(EngineState* state);
void gui_theme_classic(EngineState* state);

// ============================================================================
// TOOLTIP ÉS HELP
// ============================================================================
void gui_tooltip(EngineState* state, const char* text);
void gui_help_marker(EngineState* state, const char* text);

#ifdef __cplusplus
}
#endif

#endif
