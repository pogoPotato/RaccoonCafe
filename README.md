# 🦝 Raccoon Cafe

**Powered by STUPA Engine 2D**

Raccoon Cafe is a lightweight **2D prototype** built to showcase the power and simplicity of the **STUPA Engine**.  
It’s open-source, intentionally **"messy in a good way"**, and serves as a playground for developers who prefer **lean tools over bloated industry standards**.

---

## 🎮 Game Status

| Feature        | Status |
|---------------|--------|
| Core Gameplay | ✅ Fully Functional |
| Build System  | ✅ Stable (via `bind`) |
| Audio         | ❌ Not implemented |
| Assets        | ❌ Prototype / Placeholder art |
| Development   | 🧪 Early Alpha / Active Evolution |

---

## 🧰 The Build System: `bind`

Stop fighting your build system.

Raccoon Cafe uses **bind**, a custom **Python-based build tool** designed specifically for STUPA projects.  
The goal is simple: **it just works**.

❌ No CMake  
❌ No Visual Studio `.sln` files  
❌ No bloated Makefiles  
✅ Simple `.st` files (STUPA Build Files)

### Why `bind`?

If you’ve ever spent **four hours configuring a `CMakeLists.txt`** just to compile *Hello World*,  
you already understand why `bind` exists.

It wraps **MSVC tools cleanly** without XML, generators, or headaches.

---

## 🚀 Getting Started

### 🧪 Requirements

- **Windows** (primary target)
- **MSVC / Visual C++ Build Tools**
- **SDL3** (pre-included in `/lib`)
- **Python** (only required if running the raw script)

---

### 1️⃣ Clone the Repo

```bash
git clone https://github.com/pogoPotato/RaccoonCafe.git
cd RaccoonCafe
```

---

### 2️⃣ Prepare `bind`

You have **two ways** to use the build tool:

#### Option A: Run with Python (Quickest)

```bash
python bind.py RaccoonCafe
```

#### Option B: Compile to `.exe` (Recommended)

```bash
pip install pyinstaller
pyinstaller --onefile bind.py
# Move bind.exe to your PATH or project root
bind RaccoonCafe
```

---

### 3️⃣ Build the Game

The `.st` file handles:
- source files
- include paths
- SDL3 linking

So you only need **one command**:

```bash
bind RaccoonCafe
```

---

## 🤝 Contributing

We like people who enjoy **breaking things and then fixing them**.

This is an **experimental project**, not a corporate repo.

### How to Help

- **The "Vibe" Check**  
  Add proper assets or sound effects.

- **Refactoring**  
  Turn "good messy" code into "clean efficient" code.

- **Bug Hunting**  
  Find ’em. Squash ’em.

- **Engine Work**  
  Optimize how **STUPA Engine** interacts with `bind`.

**Rule of Thumb:**  
Use common sense, don’t break the build for everyone else, and have fun.

---

## 📄 License

**Open Source**

You are free to use, modify, and distribute this code.

Just **don’t claim you wrote everything from scratch** —  
give credit to the **STUPA ecosystem** where it’s due.

---

## 🧠 Final Note

Raccoon Cafe is **opinionated software**.

It’s built for developers who are tired of standard overhead and want to get back to the **basics of 2D game dev**.

If it feels *non-standard* — good.  
That’s the point. 😈
