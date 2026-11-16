@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM Generate compile_commands.json (no compilation)
REM Usage: make_json.cmd [Debug|Release] [C++ standard] [C standard]
REM Output: <config>/compile_commands.json
REM ============================================================

REM --- Set code page to UTF-8 ---
chcp 65001 >nul 2>&1

REM --- Compiler paths (only used to build command strings) ---
set "GCC=E:/vsc/winlib_gcc_16_20/bin/gcc.exe"
set "GPP=E:/vsc/winlib_gcc_16_20/bin/g++.exe"

REM --- Arguments ---
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"
set "CPP_STANDARD=%~2"
if "%CPP_STANDARD%"=="" set "CPP_STANDARD=gnu++20"
set "C_STANDARD=%~3"
if "%C_STANDARD%"=="" set "C_STANDARD=gnu23"

REM --- Compile options (for command string) ---
set "COMMON_OPTS=-Wall -Wextra -I."
if /i "%CONFIG%"=="Debug" (
  set "CONFIG_OPTS=-g -O0 -DDEBUG"
  ) else (
  set "CONFIG_OPTS=-O2 -DNDEBUG"
)

REM --- Collect sources (exclude obj and output dirs) ---
set "CPP_SOURCES="
set "C_SOURCES="
for /r %%f in (*.cpp *.cc *.c++ *.c) do (
  set "fullpath=%%~f"
  set "skip=0"
  echo !fullpath! | findstr /i /c:"\obj\" >nul && set "skip=1"
  echo !fullpath! | findstr /i /c:"\%CONFIG%\" >nul && set "skip=1"
  if "!skip!"=="0" (
    set "ext=%%~xf"
    if /i "!ext!"==".cpp" set "CPP_SOURCES=!CPP_SOURCES! "%%f""
    if /i "!ext!"==".cc"   set "CPP_SOURCES=!CPP_SOURCES! "%%f""
    if /i "!ext!"==".c++"  set "CPP_SOURCES=!CPP_SOURCES! "%%f""
    if /i "!ext!"==".c"    set "C_SOURCES=!C_SOURCES! "%%f""
  )
)

if "%CPP_SOURCES%"=="" if "%C_SOURCES%"=="" (
  echo [ERROR] No source files found.
  exit /b 1
)

REM --- Prepare output directory and file ---
if not exist "%CONFIG%" mkdir "%CONFIG%"
set "CCDB=%CD%\compile_commands.json"
set "CD_ESCAPED=%CD:\=/%"

echo Generating compile_commands.json for %CONFIG% ...
echo [ > "%CCDB%"
set "FIRST_ENTRY=1"

REM --- Helper macro to append an entry (avoid empty echo) ---
set "APPEND_ENTRY="

REM --- Process C files ---
if not "%C_SOURCES%"=="" (
  for %%s in (%C_SOURCES%) do (
    set "src=%%~s"
    set "src_escaped=!src:\=/!"
    set "base=%%~ns"
    set "obj=obj/%CONFIG%/!base!.obj"
    set "cmd_line=!GCC! -c !src_escaped! !COMMON_OPTS! !CONFIG_OPTS! -std=!C_STANDARD! -o \"!obj!\""
    
    if !FIRST_ENTRY!==1 (
      set "FIRST_ENTRY=0"
      ) else (
      echo , >> "!CCDB!"
    )
    echo { >> "!CCDB!"
    echo  "directory": "!CD_ESCAPED!", >> "!CCDB!"
    echo  "file": "!src_escaped!", >> "!CCDB!"
    echo  "command": "!cmd_line!" >> "!CCDB!"
    echo } >> "!CCDB!"
  )
)

REM --- Process C++ files ---
if not "%CPP_SOURCES%"=="" (
  for %%s in (%CPP_SOURCES%) do (
    set "src=%%~s"
    set "src_escaped=!src:\=/!"
    set "base=%%~ns"
    set "obj=obj/%CONFIG%/!base!.obj"
    set "cmd_line=!GPP! -c !src_escaped! !COMMON_OPTS! !CONFIG_OPTS! -std=!CPP_STANDARD! -o \"!obj!\""
    
    if !FIRST_ENTRY!==1 (
      set "FIRST_ENTRY=0"
      ) else (
      echo , >> "!CCDB!"
    )
    echo { >> "!CCDB!"
    echo  "directory": "!CD_ESCAPED!", >> "!CCDB!"
    echo  "file": "!src_escaped!", >> "!CCDB!"
    echo  "command": "!cmd_line!" >> "!CCDB!"
    echo } >> "!CCDB!"
  )
)

echo ] >> "%CCDB%"
echo [SUCCESS] compile_commands.json generated at %CCDB%
endlocal
