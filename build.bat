@echo off
REM 使用默认 VS 版本构建 32 位项目
cmake -S . -B build -A Win32
pause
