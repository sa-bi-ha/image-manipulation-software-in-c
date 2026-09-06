
📌overview

A simple **24-bit uncompressed BMP** image manipulation program written in **C**, using the **IUP GUI toolkit**.It supports basic image operations and displays image information

---

# Features~


✅ Open 24-bit uncompressed BMP image
✅Save images as BMP
✅ Grayscale
✅ Brightness adjustment
✅Invert colors
✅ Horizontal flip
✅ Vertical flip
✅ Rotate
✅Crop
✅ Blur
✅Sharpen
✅ Undo
✅Image information display
✅ GUI built with IUP

---

#  Project Structure

```
🖼️🖼️Image Manipulation Software/
│
├── include/              # Header files
│   ├── bmp.h
│   ├── gui.h
│   ├── image.h
│   └── operations.h
│
├── src/                  # Source files
│   ├── bmp.c
│   ├── gui.c
│   ├── image.c
│   ├── main.c
│   └── operations.c

│
├── third_party/          # External libraries
│   └── iup/
└── README.md



## ⚙️ Requirements

### Windows

- GCC / MinGW-w64
- IUP
- IUP Draw
- Windows system libraries required by IUP

### Linux

- GCC
- Linux-compatible IUP installation
- IUP Draw
- Required IUP dependencies

---

## 🛠️ Building on Windows

Open the **MSYS2 MINGW64** terminal in the project directory and run:


```bash
gcc src/*.c -o image_editor.exe -std=c17 -Wall -Wextra -mwindows -Iinclude -Ithird_party/iup/include -Lthird_party/iup -liupcontrols -liupgl -liupimglib -liup -lgdi32 -lcomctl32 -lole32 -luuid -lmingw32
```

Then run:

```bash

./image_editor.exe
```

---

## 🐧 Building on Linux

Make sure the Linux version of **IUP** and its required libraries are available.

Then compile using GCC with the appropriate IUP include and library paths.

Example:

```bash
gcc src/*.c -o image_editor -std=c17 -Wall -Wextra -Iinclude -Ithird_party/iup/include -Lthird_party/iup -liup
```

> Depending on the Linux IUP installation, additional libraries may need to be linked.

Run:

```bash
./image_editor
```

---

## 📝 Notes

- The image manipulation algorithms are implemented in **C** as part of this project.

