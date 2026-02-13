# Veko GUI Module - Tkinter-like Syntax

SDL2 + Dear ImGui alapú GUI modul Python tkinter-szerű szintaxissal.

## 🎯 Tkinter Összehasonlítás

### Python Tkinter
```python
import tkinter as tk

root = tk.Tk()
root.title("My App")
root.geometry("800x600")

label = tk.Label(root, text="Hello World")
label.pack()

button = tk.Button(root, text="Click Me", command=on_click)
button.pack()

entry = tk.Entry(root, textvariable=name_var)
entry.pack()

root.mainloop()
```

### Veko GUI (Tkinter-like)
```python
import gui

gui.init()
gui.window("My App", 800, 600)

while gui.is_running()
    gui.frame_begin("Main")
    
    gui.label("Hello World")
    gui.pack()
    
    gui.button("Click Me", "on_click")
    gui.pack()
    
    gui.entry("name_var")
    gui.pack()
    
    gui.frame_end()
    gui.update()
end

gui.quit()
```

## 📦 Widgetek

### Label
```python
gui.label("Text to display")
```

### Button
```python
gui.button("Button Text", "callback_function")
```

### Entry (Single-line input)
```python
gui.entry("variable_name")
```

### Text (Multi-line input)
```python
gui.text("variable_name", 300, 200)  # width, height
```

### Checkbox
```python
gui.checkbox("Checkbox label", "bool_variable")
```

### Slider
```python
gui.slider("variable_name", 0, 100)  # min, max
```

### Combobox (Dropdown)
```python
gui.combobox("variable_name", "Item1\0Item2\0Item3\0")
```

### Listbox
```python
gui.listbox("variable_name", "Item1\0Item2\0Item3\0")
```

## 🏗️ Layout

### Pack (Automatic layout)
```python
gui.pack()
```

### Grid (Row/Column layout)
```python
gui.grid(0, 0)  # row, column
```

## 🪟 Frames (Windows)

```python
gui.frame_begin("Frame Title")
# ... widgets here ...
gui.frame_end()
```

## 📋 Menu

```python
gui.menu_begin()
gui.menu_item("File", "on_file_click")
gui.menu_item("Edit", "on_edit_click")
gui.menu_end()
```

## 💬 Dialog Boxes

### Message Box
```python
gui.messagebox("Title", "Message text")
```

### Ask String
```python
gui.askstring("Title", "Prompt text", "result_variable")
```

## 🎨 Styling

### Set Color
```python
gui.set_color(1.0, 0.0, 0.0, 1.0)  # RGBA
```

### Set Font Size
```python
gui.set_font_size(1.5)  # Scale factor
```

## 🔧 Window Management

### Create Window
```python
gui.window("Title", 800, 600)
```

### Change Title
```python
gui.window_title("New Title")
```

### Change Size
```python
gui.window_geometry(1024, 768)
```

## 🚀 Complete Example

```python
# Import GUI module
import gui

# Initialize
gui.init()
gui.window("Calculator", 400, 300)

# Variables
result = 0
num1 = 0
num2 = 0

# Main loop
while gui.is_running()
    gui.frame_begin("Calculator")
    
    gui.label("Simple Calculator")
    gui.pack()
    
    gui.label("Number 1:")
    gui.entry("num1")
    gui.pack()
    
    gui.label("Number 2:")
    gui.entry("num2")
    gui.pack()
    
    gui.button("Add", "calculate_add")
    gui.button("Subtract", "calculate_sub")
    gui.grid(0, 1)
    
    gui.pack()
    
    gui.label("Result:")
    print(result)
    
    gui.frame_end()
    gui.update()
end

gui.quit()
```

## 📊 Widget Comparison Table

| Tkinter | Veko GUI | Description |
|---------|----------|-------------|
| `Label(text="...")` | `gui.label("...")` | Text label |
| `Button(text="...", command=...)` | `gui.button("...", "...")` | Button |
| `Entry(textvariable=...)` | `gui.entry("...")` | Single-line input |
| `Text(width=..., height=...)` | `gui.text("...", w, h)` | Multi-line input |
| `Checkbutton(text="...", variable=...)` | `gui.checkbox("...", "...")` | Checkbox |
| `Scale(from_=..., to=..., variable=...)` | `gui.slider("...", min, max)` | Slider |
| `Combobox(values=[...])` | `gui.combobox("...", "...")` | Dropdown |
| `Listbox()` | `gui.listbox("...", "...")` | List selection |
| `Frame()` | `gui.frame_begin("...")` | Container |
| `Menu()` | `gui.menu_begin()` | Menu bar |
| `messagebox.showinfo()` | `gui.messagebox("...", "...")` | Message dialog |
| `simpledialog.askstring()` | `gui.askstring("...", "...", "...")` | Input dialog |

## 🔗 Dependencies

- SDL2
- SDL2_opengl
- Dear ImGui (included in external/)
- OpenGL 3.0+

## 📝 Notes

- ImGui uses immediate mode rendering (different from tkinter's retained mode)
- Layout is more automatic in ImGui compared to tkinter
- Callbacks are referenced by name (strings) and executed by the engine
- Variables are automatically bound to the engine's variable system
