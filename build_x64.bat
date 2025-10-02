chcp 65001
@echo off
cmake -S . -B build_x64 -A x64 -DBUILD_EZUI=OFF
pause
