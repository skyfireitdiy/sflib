@echo off
set /p install_path="input the path you want to install sflib:"
cmake .. -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="%install_path%"
mingw32-make
mingw32-make install
pause