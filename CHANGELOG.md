# Veko Changelog

## v4.0 - Event-Driven Architecture (2024)

### 🎉 Major Changes

**Átállás frame-alapú végrehajtásról eseményvezérelt architektúrára!**

#### Előtte (v3.0 - Frame-based):
- A program minden 500ms-ben újraolvasta és újrafutatta az `input.txt` fájlt
- A változók nem maradtak meg a frame-ek között
- A GUI nem működött rendesen
- A math modul feleslegesen újraszámolt minden frame-ben

#### Most (v4.0 - Event-driven):
- ✅ A program **egyszer fut le** inicializáláskor
- ✅ A változók **megmaradnak a memóriában**
- ✅ A GUI **eseményvezérelt** és folyamatosan fut
- ✅ A math modul **csak akkor számol**, amikor kell
- ✅ **Nincs 500ms-es késleltetés**, a GUI azonnal reagál

### 🔧 Technikai Változások

#### 1. Új Végrehajtási Modell

**Két fő függvény:**
- `initialize()` - Egyszer fut le a program indulásakor
- `mainloop()` - Az eseményvezérelt főciklus (GUI mainloop)

**Előtte:**
```c
while (1) {
    update(state);  // Újraolvassa input.txt-t minden frame-ben
    usleep(500000);
}
```

**Most:**
```c
initialize(state);  // Egyszer fut le
mainloop(state);    // GUI mainloop veszi át a vezérlést
```

#### 2. GUI Render Loop

A `while gui.is_running()` loop tartalma most **minden frame-ben renderelődik** a GUI mainloop-ban:

```veko
# Inicializálás (egyszer fut le)
import gui
gui_init()
gui_window_create("My App", 800, 600)

counter = 0

# GUI render loop (minden frame-ben fut)
while 1 == 1
    gui_frame_begin("Main")
    gui_label("Hello World!")
    print(counter)
    counter = counter + 1
    gui_frame_end()
end
```

#### 3. Változók Perzisztenciája

A változók most **megmaradnak** a frame-ek között:

```veko
counter = 0

while 1 == 1
    gui_label("Counter:")
    print(counter)
    counter = counter + 1  # Növekszik minden frame-ben!
end
```

#### 4. Math Modul Optimalizálás

A math modul függvények most **csak akkor futnak**, amikor explicit meghívják őket:

```veko
import math

# Egyszer számol
x = math.sin(1.5)

while 1 == 1
    # x értéke megmarad, nem számol újra
    gui_label("Sin(1.5):")
    print(x)
end
```

### 📝 API Változások

#### Eltávolított Függvények:
- `gui.is_running()` - Már nem szükséges, a while loop automatikusan fut
- `gui.update()` - Automatikusan történik a mainloop-ban
- `gui.quit()` - Automatikusan történik a program végén

#### Új Függvények:
- `gui_init()` - GUI rendszer inicializálása
- `gui_window_create(title, width, height)` - Ablak létrehozása
- `gui_frame_begin(title)` - Frame kezdete
- `gui_frame_end()` - Frame vége
- `gui_label(text)` - Címke widget
- `gui_slider(var_name, min, max)` - Csúszka widget
- `gui_separator()` - Elválasztó vonal

### 🚀 Teljesítmény Javulások

- **Nincs 500ms késleltetés** - A GUI azonnal reagál
- **Nincs felesleges újraolvasás** - Az input.txt csak egyszer olvasódik be
- **Nincs felesleges újraszámítás** - A math műveletek csak egyszer futnak le
- **Memória hatékonyság** - A változók nem törlődnek minden frame-ben

### 📚 Példák

Lásd:
- `gui_example.ve` - Egyszerű GUI példa
- `gui_complex_example.ve` - Összetett GUI példa

### 🔄 Migrációs Útmutató

Ha van régi Veko kódod (v3.0), így kell átírni:

**Előtte (v3.0):**
```veko
import gui
gui.init()
gui.window("App", 800, 600)

while gui.is_running()
    gui.label("Hello")
    gui.update()
end

gui.quit()
```

**Most (v4.0):**
```veko
import gui
gui_init()
gui_window_create("App", 800, 600)

while 1 == 1
    gui_frame_begin("Main")
    gui_label("Hello")
    gui_frame_end()
end
```

### 🐛 Ismert Problémák

- Headless környezetben (nincs X11) a GUI nem indul el (normális)
- A while loop feltételének mindig `1 == 1`-nek kell lennie GUI esetén

### 🎯 Következő Lépések

- [ ] Callback rendszer a GUI eseményekhez (gombok, sliderek)
- [ ] Hot-reload támogatás az új architektúrában
- [ ] Több GUI widget implementálása
- [ ] Dokumentáció bővítése

