@echo off
echo ===============================
echo  Clearing QML Cache for Qt
echo ===============================

REM Kill Qt Creator if running
taskkill /IM qtcreator.exe /F >nul 2>&1

REM Remove QML cache folder
rmdir /s /q "%LOCALAPPDATA%\QtProject\qmlcache"

REM Check if cache folder still exists
if exist "%LOCALAPPDATA%\QtProject\qmlcache" (
    echo ⚠️ QML cache folder still exists!
) else (
    echo ✅ QML cache deleted successfully!
)

echo ===============================

REM Start Qt Creator again
start "" "C:\Qt\Tools\QtCreator\bin\qtcreator.exe"

pause
