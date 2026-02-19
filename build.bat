@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: Цвета (не работают в обычном cmd, но оставим для PowerShell)
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

echo ========================================
echo    Сборка файлового менеджера для Windows
echo ========================================
echo.

:: Проверка разрядности системы
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set "ARCH=x64"
) else (
    set "ARCH=x86"
)
echo Архитектура: %ARCH%
echo.

:: Проверка наличия компилятора
echo Проверка компилятора...

:: Проверка Visual Studio
if defined VisualStudioVersion (
    echo [OK] Visual Studio найден
    set "COMPILER=VS"
    goto :check_cmake
)

:: Проверка MinGW
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    echo [OK] MinGW найден
    set "COMPILER=MINGW"
    goto :check_cmake
)

:: Проверка Clang
where clang++ >nul 2>nul
if %errorlevel% equ 0 (
    echo [OK] Clang найден
    set "COMPILER=CLANG"
    goto :check_cmake
)

:: Если ничего не найдено
echo [ERROR] Компилятор C++ не найден!
echo.
echo Установите один из компиляторов:
echo   - Visual Studio 2019/2022 (рекомендуется)
echo   - MinGW-w64 (https://www.mingw-w64.org/)
echo   - LLVM/Clang (https://clang.llvm.org/)
echo.
pause
exit /b 1

:check_cmake
:: Проверка CMake
echo Проверка CMake...
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] CMake не найден!
    echo Скачайте CMake: https://cmake.org/download/
    pause
    exit /b 1
)
echo [OK] CMake найден
echo.

:: Создание папки для сборки
if not exist build mkdir build
cd build

:: Конфигурация CMake в зависимости от компилятора
echo Конфигурация CMake...
if "%COMPILER%"=="VS" (
    :: Visual Studio
    cmake .. -G "Visual Studio 17 2022" -A %ARCH% -DCMAKE_BUILD_TYPE=Release
) else if "%COMPILER%"=="MINGW" (
    :: MinGW
    cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
) else if "%COMPILER%"=="CLANG" (
    :: Clang
    cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
)

if %errorlevel% neq 0 (
    echo [ERROR] Ошибка конфигурации CMake
    pause
    exit /b 1
)

:: Компиляция
echo.
echo Компиляция...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo [ERROR] Ошибка компиляции
    pause
    exit /b 1
)

echo.
echo [OK] Сборка завершена успешно!

:: Проверка результата
if exist "Release\filemanager.exe" (
    set "EXE_PATH=Release\filemanager.exe"
) else if exist "filemanager.exe" (
    set "EXE_PATH=filemanager.exe"
) else (
    echo [ERROR] Исполняемый файл не найден
    pause
    exit /b 1
)

:: Показываем размер
for %%I in ("!EXE_PATH!") do set "FILESIZE=%%~zI"
set /a "FILESIZE_KB=!FILESIZE! / 1024"
echo Размер файла: !FILESIZE_KB! KB
echo Путь: %cd%\!EXE_PATH!
echo.

:: Спросить про установку
set /p "INSTALL=Установить в систему? (y/n): "
if /i "!INSTALL!"=="y" (
    :: Копирование в System32 (требует администратора)
    echo Копирование в C:\Windows\System32\...
    copy "!EXE_PATH!" C:\Windows\System32\ >nul
    if !errorlevel! equ 0 (
        echo [OK] Установка завершена!
        echo Теперь можно запускать: filemanager
    ) else (
        echo [ERROR] Ошибка установки. Запустите от имени администратора.
    )
)

:: Спросить про создание пакета
set /p "PACKAGE=Создать ZIP-архив для распространения? (y/n): "
if /i "!PACKAGE!"=="y" (
    echo Создание архива...
    cd ..
    powershell Compress-Archive -Path build\!EXE_PATH! -DestinationPath "filemanager-windows-%ARCH%.zip" -Force
    if !errorlevel! equ 0 (
        echo [OK] Архив создан: filemanager-windows-%ARCH%.zip
    ) else (
        echo [ERROR] Ошибка создания архива
    )
)

echo.
pause
