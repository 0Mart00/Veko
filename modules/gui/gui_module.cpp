#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "gui_module.h"
#include "../core/memory.h"
#include <dlfcn.h>

// ImGui headers
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/backends/imgui_impl_sdl2.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"

// Global GUI state
static GUIState g_gui = {0};

// ============================================================================
// CORE SYSTEM
// ============================================================================

extern "C" void gui_init(EngineState* state) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf(">>> [GUI ERROR] SDL_Init failed: %s\n", SDL_GetError());
        return;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    g_gui.running = 1;
    printf(">>> [GUI] SDL2 initialized\n");
    (void)state;
}

extern "C" void gui_window_create(EngineState* state, const char* title, int width, int height) {
    strncpy(g_gui.title, title, 127);
    g_gui.width = width;
    g_gui.height = height;
    
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    
    g_gui.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     width, height, window_flags);
    
    if (!g_gui.window) {
        printf(">>> [GUI ERROR] Window creation failed: %s\n", SDL_GetError());
        return;
    }
    
    g_gui.gl_context = SDL_GL_CreateContext(g_gui.window);
    SDL_GL_MakeCurrent(g_gui.window, g_gui.gl_context);
    SDL_GL_SetSwapInterval(1);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Not available
    
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(g_gui.window, g_gui.gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    printf(">>> [GUI] Window created: %s (%dx%d)\n", title, width, height);
    (void)state;
}

extern "C" void gui_mainloop(EngineState* state) {
    // Load update function from logic.so for hot-reload support
    void* logic_handle = dlopen("./build/logic.so", RTLD_NOW);
    if (!logic_handle) {
        fprintf(stderr, "Failed to load logic.so in mainloop: %s\n", dlerror());
        return;
    }
    
    typedef void (*logic_update_fn)(EngineState*);
    logic_update_fn update = (logic_update_fn)dlsym(logic_handle, "update");
    
    while (g_gui.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) g_gui.running = 0;
            if (event.type == SDL_WINDOWEVENT && 
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(g_gui.window)) {
                g_gui.running = 0;
            }
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Execute user GUI code from input.txt every frame
        if (update && state) {
            update(state);
        }
        
        ImGui::Render();
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(g_gui.window);
    }
    
    if (logic_handle) {
        dlclose(logic_handle);
    }
}
extern "C" void gui_quit(EngineState* state) {
    g_gui.running = 0;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    if (g_gui.gl_context) SDL_GL_DeleteContext(g_gui.gl_context);
    if (g_gui.window) SDL_DestroyWindow(g_gui.window);
    SDL_Quit();
    printf(">>> [GUI] Shutdown complete\n");
    (void)state;
}

extern "C" int gui_is_running(EngineState* state) {
    (void)state;
    return g_gui.running;
}

extern "C" void gui_update(EngineState* state) {
    (void)state;
}

// ============================================================================
// 1. KONTÉNEREK ÉS RENDSZEREZŐK
// ============================================================================

extern "C" void gui_window_title(EngineState* state, const char* title) {
    if (g_gui.window) {
        SDL_SetWindowTitle(g_gui.window, title);
        strncpy(g_gui.title, title, 127);
    }
    (void)state;
}

extern "C" void gui_window_geometry(EngineState* state, int width, int height) {
    if (g_gui.window) {
        SDL_SetWindowSize(g_gui.window, width, height);
        g_gui.width = width;
        g_gui.height = height;
    }
    (void)state;
}

extern "C" void gui_window_begin(EngineState* state, const char* title, int* open) {
    bool is_open = open ? (*open != 0) : true;
    ImGui::Begin(title, open ? &is_open : nullptr);
    if (open) *open = is_open ? 1 : 0;
    (void)state;
}

extern "C" void gui_window_end(EngineState* state) {
    ImGui::End();
    (void)state;
}

extern "C" void gui_frame_begin(EngineState* state, const char* title) {
    ImGui::Begin(title);
    (void)state;
}

extern "C" void gui_frame_end(EngineState* state) {
    ImGui::End();
    (void)state;
}

extern "C" void gui_panel_begin(EngineState* state, const char* title, int width, int height) {
    ImGui::BeginChild(title, ImVec2((float)width, (float)height), true);
    (void)state;
}

extern "C" void gui_panel_end(EngineState* state) {
    ImGui::EndChild();
    (void)state;
}

extern "C" void gui_scrollbox_begin(EngineState* state, const char* id, int width, int height) {
    ImGui::BeginChild(id, ImVec2((float)width, (float)height), false, ImGuiWindowFlags_HorizontalScrollbar);
    (void)state;
}

extern "C" void gui_scrollbox_end(EngineState* state) {
    ImGui::EndChild();
    (void)state;
}

extern "C" void gui_tabview_begin(EngineState* state, const char* id) {
    ImGui::BeginTabBar(id);
    (void)state;
}

extern "C" int gui_tabview_item(EngineState* state, const char* label) {
    (void)state;
    return ImGui::BeginTabItem(label) ? 1 : 0;
}

extern "C" void gui_tabview_end(EngineState* state) {
    ImGui::EndTabBar();
    (void)state;
}

extern "C" void gui_splitter_vertical(EngineState* state, const char* id, float* ratio) {
    ImGui::PushID(id);
    ImGui::Separator();
    ImGui::PopID();
    (void)state;
}

extern "C" void gui_splitter_horizontal(EngineState* state, const char* id, float* ratio) {
    ImGui::PushID(id);
    ImGui::Separator();
    ImGui::PopID();
    (void)state;
}

// ============================================================================
// 2. ADATMEGJELENÍTŐK
// ============================================================================

extern "C" void gui_label(EngineState* state, const char* text) {
    ImGui::Text("%s", text);
    (void)state;
}

extern "C" void gui_label_colored(EngineState* state, const char* text, float r, float g, float b) {
    ImGui::TextColored(ImVec4(r, g, b, 1.0f), "%s", text);
    (void)state;
}

extern "C" void gui_viewport_begin(EngineState* state, const char* id, int width, int height) {
    ImGui::BeginChild(id, ImVec2((float)width, (float)height), true, ImGuiWindowFlags_NoScrollbar);
    (void)state;
}

extern "C" void gui_viewport_end(EngineState* state) {
    ImGui::EndChild();
    (void)state;
}

extern "C" void gui_canvas_begin(EngineState* state, const char* id, int width, int height) {
    ImGui::BeginChild(id, ImVec2((float)width, (float)height), true);
    (void)state;
}

extern "C" void gui_canvas_end(EngineState* state) {
    ImGui::EndChild();
    (void)state;
}

extern "C" void gui_plot_lines(EngineState* state, const char* label, float* values, int count) {
    ImGui::PlotLines(label, values, count);
    (void)state;
}

extern "C" void gui_plot_histogram(EngineState* state, const char* label, float* values, int count) {
    ImGui::PlotHistogram(label, values, count);
    (void)state;
}

extern "C" void gui_plot_scatter(EngineState* state, const char* label, float* x, float* y, int count) {
    // ImGui doesn't have native scatter plot, use custom drawing
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    for (int i = 0; i < count; i++) {
        ImVec2 point(canvas_pos.x + x[i] * canvas_size.x, 
                    canvas_pos.y + y[i] * canvas_size.y);
        draw_list->AddCircleFilled(point, 3.0f, IM_COL32(255, 255, 0, 255));
    }
    (void)state;
    (void)label;
}

extern "C" void gui_progressbar(EngineState* state, float fraction, int width, int height) {
    ImGui::ProgressBar(fraction, ImVec2((float)width, (float)height));
    (void)state;
}

extern "C" void gui_progressbar_labeled(EngineState* state, const char* label, float fraction) {
    ImGui::Text("%s", label);
    ImGui::ProgressBar(fraction);
    (void)state;
}

extern "C" void gui_table_begin(EngineState* state, const char* id, int columns) {
    ImGui::BeginTable(id, columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
    (void)state;
}

extern "C" void gui_table_header(EngineState* state, const char* label) {
    ImGui::TableSetupColumn(label);
    (void)state;
}

extern "C" void gui_table_row(EngineState* state) {
    ImGui::TableNextRow();
    (void)state;
}

extern "C" void gui_table_cell(EngineState* state, const char* text) {
    ImGui::TableNextColumn();
    ImGui::Text("%s", text);
    (void)state;
}

extern "C" void gui_table_end(EngineState* state) {
    ImGui::EndTable();
    (void)state;
}

// ============================================================================
// 3. INTERAKTÍV BEVITELI ELEMEK
// ============================================================================

extern "C" void gui_textbox(EngineState* state, const char* label, const char* var_name, int width) {
    static char buffer[256] = "";
    ImGui::SetNextItemWidth((float)width);
    if (ImGui::InputText(label, buffer, 256)) {
        set_string(state, var_name, buffer);
    }
}

extern "C" void gui_textbox_multiline(EngineState* state, const char* label, const char* var_name, int width, int height) {
    static char buffer[1024] = "";
    if (ImGui::InputTextMultiline(label, buffer, 1024, ImVec2((float)width, (float)height))) {
        set_string(state, var_name, buffer);
    }
}

extern "C" void gui_inputfield(EngineState* state, const char* label, const char* var_name) {
    static char buffer[256] = "";
    if (ImGui::InputText(label, buffer, 256)) {
        set_string(state, var_name, buffer);
    }
}

extern "C" void gui_slider(EngineState* state, const char* var_name, double min, double max) {
    CustomVar* v = get_var(state, var_name);
    float value = v ? (float)v->data.num_val : (float)min;
    if (ImGui::SliderFloat(var_name, &value, (float)min, (float)max)) {
        set_number(state, var_name, (double)value);
    }
}

extern "C" void gui_slider_int(EngineState* state, const char* var_name, int min, int max) {
    CustomVar* v = get_var(state, var_name);
    int value = v ? (int)v->data.num_val : min;
    if (ImGui::SliderInt(var_name, &value, min, max)) {
        set_number(state, var_name, (double)value);
    }
}

extern "C" void gui_slider_angle(EngineState* state, const char* var_name) {
    CustomVar* v = get_var(state, var_name);
    float value = v ? (float)v->data.num_val : 0.0f;
    if (ImGui::SliderAngle(var_name, &value)) {
        set_number(state, var_name, (double)value);
    }
}

extern "C" void gui_spinbox(EngineState* state, const char* var_name, double min, double max, double step) {
    CustomVar* v = get_var(state, var_name);
    float value = v ? (float)v->data.num_val : (float)min;
    if (ImGui::InputFloat(var_name, &value, (float)step, (float)step * 10)) {
        if (value < min) value = (float)min;
        if (value > max) value = (float)max;
        set_number(state, var_name, (double)value);
    }
}

extern "C" void gui_spinbox_int(EngineState* state, const char* var_name, int min, int max, int step) {
    CustomVar* v = get_var(state, var_name);
    int value = v ? (int)v->data.num_val : min;
    if (ImGui::InputInt(var_name, &value, step, step * 10)) {
        if (value < min) value = min;
        if (value > max) value = max;
        set_number(state, var_name, (double)value);
    }
}

extern "C" void gui_checkbox(EngineState* state, const char* text, const char* var_name) {
    CustomVar* v = get_var(state, var_name);
    bool checked = v ? (v->type == T_BOOL ? v->data.bool_val : false) : false;
    if (ImGui::Checkbox(text, &checked)) {
        set_bool(state, var_name, checked ? 1 : 0);
    }
}

extern "C" void gui_toggle(EngineState* state, const char* label, const char* var_name) {
    CustomVar* v = get_var(state, var_name);
    bool checked = v ? (v->type == T_BOOL ? v->data.bool_val : false) : false;
    if (ImGui::Checkbox(label, &checked)) {
        set_bool(state, var_name, checked ? 1 : 0);
    }
}

extern "C" void gui_combobox(EngineState* state, const char* var_name, const char* items) {
    static int current_item = 0;
    if (ImGui::Combo(var_name, &current_item, items)) {
        set_number(state, var_name, (double)current_item);
    }
}

extern "C" void gui_dropdown(EngineState* state, const char* label, const char* var_name, const char** items, int count) {
    CustomVar* v = get_var(state, var_name);
    int current = v ? (int)v->data.num_val : 0;
    if (ImGui::BeginCombo(label, items[current])) {
        for (int i = 0; i < count; i++) {
            bool is_selected = (current == i);
            if (ImGui::Selectable(items[i], is_selected)) {
                set_number(state, var_name, (double)i);
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

// ============================================================================
// 4. AKCIÓ VEZÉRLŐK
// ============================================================================

extern "C" void gui_button(EngineState* state, const char* text, const char* callback) {
    if (ImGui::Button(text)) {
        printf(">>> [GUI] Button clicked: %s (callback: %s)\n", text, callback);
    }
    (void)state;
}

extern "C" void gui_button_colored(EngineState* state, const char* text, const char* callback, float r, float g, float b) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(r, g, b, 1.0f));
    if (ImGui::Button(text)) {
        printf(">>> [GUI] Button clicked: %s (callback: %s)\n", text, callback);
    }
    ImGui::PopStyleColor();
    (void)state;
}

extern "C" void gui_iconbutton(EngineState* state, const char* icon, const char* callback) {
    if (ImGui::Button(icon)) {
        printf(">>> [GUI] Icon button clicked: %s (callback: %s)\n", icon, callback);
    }
    (void)state;
}

extern "C" void gui_iconbutton_tooltip(EngineState* state, const char* icon, const char* tooltip, const char* callback) {
    if (ImGui::Button(icon)) {
        printf(">>> [GUI] Icon button clicked: %s (callback: %s)\n", icon, callback);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
    (void)state;
}

extern "C" void gui_toolbar_begin(EngineState* state) {
    ImGui::BeginGroup();
    (void)state;
}

extern "C" void gui_toolbar_button(EngineState* state, const char* icon, const char* callback) {
    if (ImGui::Button(icon)) {
        printf(">>> [GUI] Toolbar button: %s (callback: %s)\n", icon, callback);
    }
    ImGui::SameLine();
    (void)state;
}

extern "C" void gui_toolbar_separator(EngineState* state) {
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    (void)state;
}

extern "C" void gui_toolbar_end(EngineState* state) {
    ImGui::EndGroup();
    (void)state;
}

// ============================================================================
// LAYOUT RENDSZER
// ============================================================================

extern "C" void gui_pack(EngineState* state) {
    ImGui::Spacing();
    (void)state;
}

extern "C" void gui_grid(EngineState* state, int row, int col) {
    if (col > 0) ImGui::SameLine();
    (void)state;
    (void)row;
}

extern "C" void gui_sameline(EngineState* state) {
    ImGui::SameLine();
    (void)state;
}

extern "C" void gui_separator(EngineState* state) {
    ImGui::Separator();
    (void)state;
}

extern "C" void gui_spacing(EngineState* state) {
    ImGui::Spacing();
    (void)state;
}

extern "C" void gui_indent(EngineState* state, float amount) {
    ImGui::Indent(amount);
    (void)state;
}

extern "C" void gui_unindent(EngineState* state, float amount) {
    ImGui::Unindent(amount);
    (void)state;
}

// ============================================================================
// MENU, DIALOG, STYLING, TOOLTIP
// ============================================================================

extern "C" void gui_menu_begin(EngineState* state) {
    if (ImGui::BeginMenuBar()) {
        (void)state;
    }
}

extern "C" void gui_menu_item(EngineState* state, const char* text, const char* callback) {
    if (ImGui::MenuItem(text)) {
        printf(">>> [GUI] Menu item: %s (callback: %s)\n", text, callback);
    }
    (void)state;
}

extern "C" void gui_menu_separator(EngineState* state) {
    ImGui::Separator();
    (void)state;
}

extern "C" void gui_menu_end(EngineState* state) {
    ImGui::EndMenuBar();
    (void)state;
}

extern "C" void gui_messagebox(EngineState* state, const char* title, const char* message) {
    ImGui::OpenPopup(title);
    if (ImGui::BeginPopupModal(title, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", message);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    (void)state;
}

extern "C" void gui_askstring(EngineState* state, const char* title, const char* prompt, const char* var_name) {
    static char buffer[256] = "";
    ImGui::OpenPopup(title);
    if (ImGui::BeginPopupModal(title, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", prompt);
        ImGui::InputText("##input", buffer, 256);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            set_string(state, var_name, buffer);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

extern "C" void gui_confirm_dialog(EngineState* state, const char* title, const char* message, const char* callback_yes, const char* callback_no) {
    ImGui::OpenPopup(title);
    if (ImGui::BeginPopupModal(title, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", message);
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            printf(">>> [GUI] Confirm Yes: %s\n", callback_yes);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120, 0))) {
            printf(">>> [GUI] Confirm No: %s\n", callback_no);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    (void)state;
}

extern "C" void gui_set_color(EngineState* state, float r, float g, float b, float a) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(r, g, b, a));
    (void)state;
}

extern "C" void gui_set_font_size(EngineState* state, float size) {
    ImGui::SetWindowFontScale(size);
    (void)state;
}

extern "C" void gui_theme_dark(EngineState* state) {
    ImGui::StyleColorsDark();
    (void)state;
}

extern "C" void gui_theme_light(EngineState* state) {
    ImGui::StyleColorsLight();
    (void)state;
}

extern "C" void gui_theme_classic(EngineState* state) {
    ImGui::StyleColorsClassic();
    (void)state;
}

extern "C" void gui_tooltip(EngineState* state, const char* text) {
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text);
    }
    (void)state;
}

extern "C" void gui_help_marker(EngineState* state, const char* text) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text);
    }
    (void)state;
}

// ============================================================================
// GUI CALL DISPATCHER
// ============================================================================

extern "C" void handle_gui_call(EngineState* state, char* method_name) {
    // Core system
    if (strcmp(method_name, "init") == 0) {
        gui_init(state);
    } else if (strcmp(method_name, "mainloop") == 0) {
        gui_mainloop(state);
    } else if (strcmp(method_name, "quit") == 0) {
        gui_quit(state);
    } else if (strcmp(method_name, "update") == 0) {
        gui_update(state);
    } else if (strcmp(method_name, "theme_dark") == 0) {
        gui_theme_dark(state);
    } else if (strcmp(method_name, "theme_light") == 0) {
        gui_theme_light(state);
    } else if (strcmp(method_name, "theme_classic") == 0) {
        gui_theme_classic(state);
    } else {
        printf(">>> [GUI WARNING] Unknown GUI method: gui_%s()\n", method_name);
    }
}

extern "C" void import_gui_module(EngineState* state) {
    printf(">>> [GUI] Module imported\n");
    state->import_count++;
}
