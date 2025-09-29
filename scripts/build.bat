@echo off
mkdir build
cd build
cmake ..
cmake --build . --config Release
pause
