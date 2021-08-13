@echo off
pushd %~dp0
mkdir bin     2> NUL
mkdir bin\obj 2> NUL

set app_name=hot_reload.exe
set game_dll_name=game.dll

pushd bin

del *.dll       > NUL 2> NUL
del *.lib       > NUL 2> NUL
del *.exp       > NUL 2> NUL
del *.exe       > NUL 2> NUL
del *.pdb       > NUL 2> NUL
del /Q obj\*.*  > NUL 2> NUL

:: compiling the game
cl /nologo /Z7 /MD /std:c++latest ^
   /Fo: obj\ /LD /Fe: %game_dll_name% ^
   ../src/game.cpp /link /INCREMENTAL:NO -EXPORT:tick || exit /b 1


tasklist /fi "ImageName eq %app_name%" /fo csv 2> NUL | find /I "%app_name%" > NUL

if "%ERRORLEVEL%" == "0" goto hot_reload

:: compiling the platform
cl /nologo /Z7 /std:c++latest ^
   /Dgame_dll_name=\"%game_dll_name%\" ^
   /Fo: obj\ /Fe: %app_name% ^
   ..\src\platform_windows.cpp ^
   /link /INCREMENTAL:NO ^
   user32.lib kernel32.lib || exit /b 1

goto end

:hot_reload
echo.
echo === platform layer running: letting it reload ===
goto end

:end
popd

popd
