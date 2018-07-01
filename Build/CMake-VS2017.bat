@echo off

rem
rem Copyright (c) 2018 Piotr Doan. All rights reserved.
rem

rem Create an empty directory.
mkdir "CMake-VS2017" >nul 2>&1
cd "CMake-VS2017"

rem Generate solution files.
cmake -G "Visual Studio 15" ./../..

if %ERRORLEVEL% NEQ 0 (
    pause
    exit
)

rem Open the solution file.
echo -- Opening the solution file..
start "" "GameEngine.sln"