@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM ============================================================
REM g++/gcc Build Script (for VS Code tasks)
REM Usage: build.cmd [Debug|Release] [C++ standard] [C standard]
REM ============================================================

REM --- Set your g++/gcc paths here ---
set "GCC=E:/vsc/winlib_gcc_16_20/bin/gcc.exe"
set "GPP=E:/vsc/winlib_gcc_16_20/bin/g++.exe"

REM --- Check if compilers exist ---
if not exist "%GCC%" (
  echo [ERROR] gcc.exe not found at "%GCC%"
  echo Please edit build.cmd and set the correct path to gcc.exe.
  exit /b 1
)
if not exist "%GPP%" (
  echo [ERROR] g++.exe not found at "%GPP%"
  echo Please edit build.cmd and set the correct path to g++.exe.
  exit /b 1
)

REM --- Parse arguments ---
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

set "CPP_STANDARD=%~2"
if "%CPP_STANDARD%"=="" set "CPP_STANDARD=gnu++20"

set "C_STANDARD=%~3"
if "%C_STANDARD%"=="" set "C_STANDARD=gnu23"

REM --- Directories ---
REM obj files go to  <CONFIG>\obj  (Debug\obj, Release\obj)
REM a.exe stays in the project root (the parent of Debug\)
set "OBJDIR=%CONFIG%\obj"
set "EXE=cpytool.exe"

if not exist "%OBJDIR%" mkdir "%OBJDIR%"

REM --- Collect source files recursively, excluding obj and out dirs ---
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
  echo [ERROR] No .c, .cc, .c++, or .cpp files found.
  exit /b 1
)

echo [INFO] Configuration: %CONFIG%
echo [INFO] C++ standard: %CPP_STANDARD%
echo [INFO] C standard:  %C_STANDARD%

REM --- Compile options ---
set "COMMON_OPTS=-Wall -Wextra -I%CD%"

if /i "%CONFIG%"=="Debug" (
  set "CONFIG_OPTS=-g -O0 -DDEBUG"
  ) else (
  set "CONFIG_OPTS=-O2 -DNDEBUG"
)

REM --- Compile C sources ---
if not "%C_SOURCES%"=="" (
  echo [INFO] Compiling C sources...
  for %%s in (%C_SOURCES%) do (
    set "src=%%~s"
    set "base=%%~ns"
    set "obj=%OBJDIR%\!base!.obj"
    echo  gcc -c !src! -o !obj!
    "%GCC%" -c !src! %COMMON_OPTS% %CONFIG_OPTS% -std=%C_STANDARD% -o "!obj!"
    if errorlevel 1 (
      echo [ERROR] C compilation failed for !src!
      exit /b %errorlevel%
    )
  )
)

REM --- Compile C++ sources ---
if not "%CPP_SOURCES%"=="" (
  echo [INFO] Compiling C++ sources...
  for %%s in (%CPP_SOURCES%) do (
    set "src=%%~s"
    set "base=%%~ns"
    set "obj=%OBJDIR%\!base!.obj"
    echo  g++ -c !src! -o !obj!
    "%GPP%" -c !src! %COMMON_OPTS% %CONFIG_OPTS% -std=%CPP_STANDARD% -o "!obj!"
    if errorlevel 1 (
      echo [ERROR] C++ compilation failed for !src!
      exit /b %errorlevel%
    )
  )
)

REM --- Link ---
echo [INFO] Linking...
"%GPP%" %COMMON_OPTS% %CONFIG_OPTS% -o "%EXE%" "%OBJDIR%\*.obj" -lpathcch
if errorlevel 1 (
  echo [ERROR] Linking failed.
  exit /b %errorlevel%
)

echo [SUCCESS] Build completed: %EXE%
endlocal
