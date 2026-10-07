@echo off
setlocal

REM ----------------------------------------
REM EmuFlix release packaging script
REM Put this in the project root
REM ----------------------------------------

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "BUILD_DIR=%ROOT%\build-release"
set "PACKAGE_DIR=%ROOT%\release-package"
set "EXE_NAME=EmuFlix.exe"

set "CMAKE_GENERATOR=Ninja"
set "QT_BIN=C:\msys64\ucrt64\bin"
set "WINDEPLOYQT=%QT_BIN%\windeployqt.exe"
set "SDL_DLL=%QT_BIN%\SDL2.dll"

echo.
echo ROOT=%ROOT%
echo BUILD_DIR=%BUILD_DIR%
echo PACKAGE_DIR=%PACKAGE_DIR%
echo.

echo ===== Building EmuFlix Release =====
echo.

if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "%CMAKE_GENERATOR%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail

cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 goto :fail

echo.
echo ===== Creating package folder =====
echo.

if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"
mkdir "%PACKAGE_DIR%"

if not exist "%BUILD_DIR%\%EXE_NAME%" (
    echo ERROR: Could not find %EXE_NAME% in %BUILD_DIR%
    goto :fail
)

copy "%BUILD_DIR%\%EXE_NAME%" "%PACKAGE_DIR%\"
if errorlevel 1 goto :fail

echo.
echo ===== Running windeployqt =====
echo.

if not exist "%WINDEPLOYQT%" (
    echo ERROR: windeployqt not found at:
    echo %WINDEPLOYQT%
    goto :fail
)

"%WINDEPLOYQT%" "%PACKAGE_DIR%\%EXE_NAME%"
if errorlevel 1 goto :fail

echo.
echo ===== Copying SDL2 =====
echo.

if exist "%SDL_DLL%" (
    copy "%SDL_DLL%" "%PACKAGE_DIR%\"
)

echo.
echo ===== Copying project files =====
echo.

if exist "%ROOT%\src\data\schema.sql" (
    copy "%ROOT%\src\data\schema.sql" "%PACKAGE_DIR%\"
)

if exist "%ROOT%\assets" (
    xcopy "%ROOT%\assets" "%PACKAGE_DIR%\assets\" /E /I /Y
)

if exist "%ROOT%\licenses" (
    xcopy "%ROOT%\licenses" "%PACKAGE_DIR%\licenses\" /E /I /Y
)

if exist "%ROOT%\emulators" (
    xcopy "%ROOT%\emulators" "%PACKAGE_DIR%\emulators\" /E /I /Y
)

if exist "%ROOT%\README.md" (
    copy "%ROOT%\README.md" "%PACKAGE_DIR%\"
)

echo.
echo ===== Optional zip output =====
echo.

powershell -NoProfile -Command "if (Test-Path '%ROOT%\EmuFlix-Release.zip') { Remove-Item '%ROOT%\EmuFlix-Release.zip' -Force }; Compress-Archive -Path '%PACKAGE_DIR%\*' -DestinationPath '%ROOT%\EmuFlix-Release.zip'"
if errorlevel 1 (
    echo Zip step skipped or failed. Package folder is still ready.
)

echo.
echo SUCCESS: Release package created here:
echo %PACKAGE_DIR%
echo.
echo Optional zip created here:
echo %ROOT%\EmuFlix-Release.zip
echo.
goto :end

:fail
echo.
echo Packaging failed.
pause
exit /b 1

:end
echo.
echo Packaging complete.
pause
endlocal