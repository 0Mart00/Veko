# 🚀 Veko Dynamic Engine v2.0

**Python-szerű dinamikus szkript motor C-ben, Assembly támogatással**

Veko egy valós idejű, hot-reload képes szkript motor, amely Python szintaxist és Assembly utasításokat futtat C környezetben.

---

## ✨ Főbb Funkciók

- 🐍 **20+ Python Built-in Függvény** (abs, int, float, str, len, max, min, pow, round, sum, chr, ord, hex, bin, oct, stb.)
- ⚙️ **Assembly Nyelv Támogatás** (MOV, ADD, SUB, MUL, XOR utasítások)
- 🔄 **FOR és WHILE Ciklusok**
- 📊 **Dinamikus Típusrendszer** (float, string, bool, list, dict)
- 🔥 **Hot-Reload** - Élő kód módosítás futás közben
- 🎯 **Modular Architecture** - Dinamikus modul betöltés

---

## 🛠️ Telepítés és Használat

### Előfeltételek
- GCC fordító
- Make build tool
- Linux/Unix környezet

### Fordítás
```bash
make clean && make
```

### Futtatás
```bash
./build/engine_host
```

### Élő Módosítás (Hot-Reload)
```bash
# Másik terminálban
./scripts/watcher.sh
```

---

## 📝 Szintaxis Példák

### Python Built-in Függvények
```python
# Matematikai műveletek
result = abs(-10)         # 10.00
power = pow(2, 8)         # 256.00
maximum = max(10, 20)     # 20.00

# String műveletek
name = "Veko"
length = len(name)        # 4.00
char = chr(65)            # "A"

# Számrendszer konverziók
hex_val = hex(255)        # "0xff"
bin_val = bin(15)         # "0b1111"

# Típus konverziók
int_val = int(3.14)       # 3.00
str_val = str(42)         # "42.00"
bool_val = bool(1)        # True
```

### Ciklusok
```python
# FOR ciklus
counter = 0
for i 0 10
  counter = counter + i
end

# WHILE ciklus
x = 1
while x < 100
  x = x * 2
end
```

### Assembly Műveletek
```assembly
# Regiszter műveletek
__asm__("mov eax 100")
__asm__("mov ebx 50")
asm("add eax ebx")        # eax = 150
asm("mul eax 2")          # eax = 300
asm("xor ecx ecx")        # ecx = 0

# Eredmény kiírása
print(eax)
```

---

## 📚 Támogatott Függvények

### Matematikai
- `abs(x)` - Abszolút érték
- `int(x)` - Egész számmá alakítás
- `float(x)` - Lebegőpontos számmá alakítás
- `round(x)` - Kerekítés
- `pow(x, y)` - Hatványozás
- `divmod(x, y)` - Osztás
- `sum(start, end)` - Összegzés tartományban
- `max(a, b)` - Maximum
- `min(a, b)` - Minimum

### String
- `str(x)` - Stringgé alakítás
- `len(x)` - Hossz
- `chr(x)` - ASCII kód → karakter
- `ord(x)` - Karakter → ASCII kód

### Számrendszer
- `hex(x)` - Hexadecimális
- `bin(x)` - Bináris
- `oct(x)` - Oktális

### Típus
- `bool(x)` - Boolean konverzió
- `type(x)` - Típus lekérdezés
- `typeof(x)` - Típus ellenőrzés

### I/O
- `print(x)` - Kiírás

### Assembly
- `__asm__("instruction")` - Assembly utasítás
- `asm("instruction")` - Assembly utasítás (rövid forma)

**Támogatott Assembly műveletek:**
- `mov reg value` - Érték betöltés
- `add reg1 reg2` - Összeadás
- `sub reg1 reg2` - Kivonás
- `mul reg1 reg2` - Szorzás
- `xor reg1 reg2` - XOR művelet

---

## 📂 Projekt Struktúra

```
Veko/
├── core/
│   └── main.c              # Fő motor (dynamic loader)
├── modules/
│   └── logic.c             # Szkript végrehajtó modul
├── include/
│   └── interface.h         # Típus definíciók
├── build/
│   ├── engine_host         # Fő program
│   └── logic.so            # Dinamikus modul
├── scripts/
│   └── watcher.sh          # Hot-reload script
├── input.txt               # Szkript fájl
└── Makefile                # Build konfiguráció
```

---

## 🎯 Példa Program

```python
# Veko Dynamic Engine v2.0 - Példa

# Változók
x = 10
y = 20
name = "Veko"

# Built-in függvények
sum_val = x + y
power = pow(2, 10)
hex_val = hex(255)

# Ciklus
counter = 0
for i 1 5
  counter = counter + i
end

# Assembly
__asm__("mov eax 100")
asm("add eax 50")

# Kimenet
print(sum_val)
print(power)
print(eax)
type(name)
```

---

## 🔧 Fejlesztés

### Modul Újrafordítás
```bash
make module
```

### Teljes Újrafordítás
```bash
make clean && make
```

### Debug Mód
```bash
gcc -g -Wall -Wextra core/main.c -o build/engine_host -ldl
```

---

## 📖 Dokumentáció

### input.txt Szintaxis
- Egy parancs soronként
- `#` karakterrel kezdődő sorok megjegyzések
- Változók automatikusan létrejönnek értékadáskor
- Ciklusok `end` kulcsszóval zárulnak

### Típusok
- **float** - Lebegőpontos számok (alapértelmezett)
- **str** - Szövegek (idézőjelek között)
- **bool** - Boolean értékek (True/False)
- **list** - Listák (előkészítve)
- **dict** - Dictionary-k (előkészítve)

---

## 🚀 Teljesítmény

- **Frissítési gyakoriság:** 0.5 másodperc (500ms)
- **Maximum változók:** 100
- **Maximum ciklus iterációk:** 1000 (while védelem)
- **Maximum ciklus törzs:** 100 sor

---

## 📜 Licenc

MIT License - Szabad felhasználás és módosítás

---

## 🤝 Közreműködés

Pull request-ek és issue-k várhatóak!

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

---

## 📞 Kapcsolat

**Projekt:** Veko Dynamic Engine  
**Verzió:** 2.0  
**Nyelv:** C + Python-like syntax + Assembly

---

**Készítette:** 0Mart00  
**Utolsó frissítés:** 2024

🎨 **Happy Coding!** 🚀
