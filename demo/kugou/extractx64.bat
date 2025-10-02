@echo off
setlocal

set OUTDIR=%1
set MARKER_FILE=%OUTDIR%\dll_marker.txt
set SCRIPT_DIR=%~dp0

if exist "%MARKER_FILE%" (
    echo DLL already extracted. Skipping.
    goto :EOF
)

echo Extracting dll.zip to %OUTDIR%...

"%SCRIPT_DIR%7z.exe" x "%SCRIPT_DIR%dllx64.zip" -o"%OUTDIR%" -aoa -y

if errorlevel 1 (
    echo Extraction failed!
    exit /b 1
)

echo extracted > "%MARKER_FILE%"
echo Extraction complete.

endlocal
