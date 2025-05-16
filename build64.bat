@echo off
REM 使用默认 VS 版本构建 64 位项目
cmake -S . -B build64 -A x64
pause
