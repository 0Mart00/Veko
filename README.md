# Veko Dynamic Engine v4.0

An event-driven scripting language and runtime environment implemented in C, featuring integrated GUI capabilities and a high-performance mathematics subsystem.

## Overview

Veko is a lightweight, high-performance scripting runtime executing a Python-influenced syntax with inline assembly generation capabilities. Version 4.0 introduces a stateful, reactive programming model, moving away from stateless frame-bound execution architectures.

### Key Architectural Updates in v4.0

* **Stateful Memory Persistence:** Scripts execute strictly once during the lifecycle initialization phase. Subsystem states and variables persist natively in virtual memory across runtime frames.
* **Asynchronous GUI Processing:** The graphical user interface operates via an asynchronous event loop, eliminating arbitrary thread sleep latency thresholds (0ms polling delay).
* **On-Demand Computation Subsystems:** Mathematics pipelines remain idle until explicit invocation, minimizing CPU overhead during continuous frame rendering.

---

## Technical Specifications & Features

* **Asynchronous Graphical Subsystem:** Powered by a customized wrapper binding SDL2 and Dear ImGui.
* **Modular Mathematics Subsystem:** Dynamic loading of transcendental, trigonometric, and arithmetic pipelines.
* **Standard Built-in Library:** Includes a compliant subset of Python-standard primitives (`abs`, `int`, `float`, `str`, `len`, `max`, `min`, `pow`, `round`, `sum`, `chr`, `ord`, `hex`, `bin`, `oct`).
* **Dynamic Type Topology:** Dynamic type inference engine natively evaluating `float`, `string`, `bool`, `list`, and `dict` structures.
* **Control Flow Evaluators:** Iterative `for` blocks and conditional `while` loops with built-in infinite loop mitigation safeguards.
* **Low-Level Assembly Interfaces:** Inline ISA emulation subsystem supporting explicit register allocation (`eax`, `ebx`, `ecx`) and arithmetic operators (`mov`, `add`, `sub`, `mul`, `xor`).

### Engine Limits & Boundary Constraints

* **Maximum Concurrent Symbol Table Entries:** 100 allocation slots
* **Infinite Loop Mitigation Threshold:** 1000 loop cycles maximum
* **Maximum Control Flow Block Size:** 100 logical lines per routine

---

## Installation & Deployment

### Subsystem Dependencies

#### Debian / Ubuntu Architecture

```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libgl1-mesa-dev

```

#### Fedora / RedHat Architecture

```bash
sudo dnf install gcc gcc-c++ make SDL2-devel mesa-libGL-devel

```

### Build Instructions

To compile the primary host binary and execute modular library linkage:

```bash
make clean && make

```

To recompile the shared logic modules exclusively:

```bash
make module

```

### Execution Directives

#### Headless Mode (Standard CLI Execution)

```bash
echo -e "x = 10\ny = 20\nz = x + y\nprint(z)" > input.txt
./build/engine_host

```

#### Graphical Mode (GUI Subsystem Instantiation)

```bash
cp gui_example.ve input.txt
./build/engine_host

```

---

## Subsystem APIs & Reference Manual

### Native Environment Built-ins

| Target Domain | Function Signature | Operational Specification |
| --- | --- | --- |
| **Arithmetic** | `abs(x)` | Computes the absolute scalar value. |
|  | `int(x)` / `float(x)` | Casts the expression to the respective numeric primitive. |
|  | `round(x)` | Performs standard arithmetic rounding operations. |
|  | `pow(x, y)` | Raises base $x$ to the exponent power $y$. |
|  | `divmod(x, y)` | Returns the division quotient and remainder vector. |
|  | `sum(start, end)` | Computes the aggregate sum across the designated range. |
|  | `max(a, b)` / `min(a, b)` | Returns the extreme boundary values. |
| **String Manipulation** | `str(x)` | Converts the target expression into a string primitive. |
|  | `len(x)` | Evaluates the byte length or element count of the object. |
|  | `chr(x)` / `ord(x)` | Performs ASCII character/integer mappings. |
| **Base Conversions** | `hex(x)` / `bin(x)` / `oct(x)` | Serializes integer types to radix-16, radix-2, or radix-8 string formats. |
| **Reflection & IO** | `type(x)` / `typeof(x)` | Inspects the runtime allocation type descriptor. |
|  | `print(x)` | Standard output stream buffer flush writer. |

### Inline ISA Assembly Subsystem

Instruction syntax formatting: `__asm__("opcode destination source")` or `asm("opcode destination source")`.

* `mov [reg] [val]` — Loads an immediate value or secondary register state into the destination register.
* `add [reg1] [reg2]` — Performs binary addition: $\text{reg1} \leftarrow \text{reg1} + \text{reg2}$.
* `sub [reg1] [reg2]` — Performs binary subtraction: $\text{reg1} \leftarrow \text{reg1} - \text{reg2}$.
* `mul [reg1] [reg2]` — Multiplies designated registers: $\text{reg1} \leftarrow \text{reg1} \times \text{reg2}$.
* `xor [reg1] [reg2]` — Computes bitwise exclusive-OR logical operations.

---

## Implementation Examples

### Event-Driven User Interface Construction (`gui.ve`)

```python
# Link graphical subsystem libraries
import gui

# Initialize window configuration contexts
gui_init()
gui_window_create("Veko Execution Environment", 800, 600)

# Instantiate persistent global scope variables
counter = 0
slider_value = 50

# Main Reactive Frame Evaluation Loop
while 1 == 1
    gui_frame_begin("Control Panel")
    
    gui_label("Veko Dynamic Engine Subsystem Engine")
    gui_separator()
    
    gui_label("Telemetry Frame Index:")
    print(counter)
    counter = counter + 1
    
    gui_separator()
    
    gui_label("Control Variable Modification:")
    gui_slider("slider_value", 0, 100)
    
    gui_label("Evaluated State:")
    print(slider_value)
    
    gui_frame_end()
end

```

### Transcendental Mathematics Pipeline (`math.ve`)

```python
import math

# Analytical coordinate definitions
angle = 1.57
sin_val = math.sin(angle)
cos_val = math.cos(angle)

# Algorithmic calculation verification
sqrt_val = math.sqrt(16)
pow_val = math.pow(2, 8)

print(sin_val)
print(sqrt_val)

```

---

## Repository Architecture

```
Veko/
├── core/
│   └── main.c              # Dynamic linker host and core orchestration engine
├── modules/
│   └── logic.c             # Virtual machine parsing and instruction execution pipeline
├── include/
│   └── interface.h         # System-wide explicit data structures and type signatures
├── build/
│   ├── engine_host         # Compiled host entry-point execution file
│   └── logic.so            # Dynamically loaded runtime shared object library
├── scripts/
│   └── watcher.sh          # Hot-reloading daemon for code injection automation
├── input.txt               # Default runtime script execution buffer target
└── Makefile                # Automations configuration pipeline file

```

---

## Licensing & Architecture Maintenance

* **License Model:** MIT License — Open-source usage, modification, and redistribution criteria permitted.
* **Maintainer Profile:** Developed by `0Mart00`.
