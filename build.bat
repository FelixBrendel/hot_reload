@echo off
pushd %~dp0
mkdir bin 2>nul
mkdir bin\obj 2>nul

set app_name=hot_reload

pushd bin

del *.exe       > NUL 2> NUL
del *.pdb       > NUL 2> NUL
del /Q obj\*.*  > NUL 2> NUL

:: compiling the game
cl /nologo /Z7 /MD /std:c++latest ^
   /Fo: obj\ /LD /Fe: game.dll ^
   ../src/game.cpp /link /INCREMENTAL:NO -EXPORT:tick || exit 1


tasklist /fi "ImageName eq %app_name%.exe" /fo csv 2>NUL | find /I "%app_name%.exe">NUL

if "%ERRORLEVEL%" == "0" goto hot_reload

:: compiling the platform
cl /nologo /std:c++latest ^
   /Fo: obj\ /Fe: %app_name% ^
   ..\src\platform_windows.cpp ^
   /link /INCREMENTAL:NO ^
   -PDB:%app_name%.pdb user32.lib kernel32.lib || exit /B 1
goto end

:hot_reload
echo.
echo === platform layer running: letting it reload ===
goto end

:end
popd

popd
