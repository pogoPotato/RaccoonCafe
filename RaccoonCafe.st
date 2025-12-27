[project]
name = RaccoonCafe
type = executable

[output]
name = RaccoonCafe.exe
directory = build

[compiler]
cpp_compiler = g++
cpp_flags = -std=c++20 -O2 -Wall

[sources]
files = src/RaccoonCafe.cpp, src/Game.cpp, src/Coin.cpp, src/Customer.cpp
include_dirs = include, lib/SDL3-devel-3.2.28-VC/include, lib/SDL3_image-devel-3.2.4-VC/include, lib/SDL3_ttf-devel-3.2.2-VC/include

[linking]
libraries = SDL3, SDL3_image, SDL3_ttf, user32, gdi32, shell32
lib_dirs = lib/SDL3-devel-3.2.28-VC/lib/x64, lib/SDL3_image-devel-3.2.4-VC/lib/x64, lib/SDL3_ttf-devel-3.2.2-VC/lib/x64
