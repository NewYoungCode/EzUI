chcp 65001
@echo off
cmake -S . -B build_x86 -A Win32 -DBUILD_EZUI=OFF
pause
