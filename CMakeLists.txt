cmake_minimum_required(VERSION 3.15)
project(FileManager VERSION 1.0.0 LANGUAGES CXX)

# Настройки C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Название исполняемого файла
if(WIN32)
    set(TARGET_NAME "filemanager.exe")
else()
    set(TARGET_NAME "filemanager")
endif()

# Опции сборки
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_TESTING "Enable testing" OFF)

# Пути к исходникам
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# Создание исполняемого файла
add_executable(${TARGET_NAME} ${SOURCES})

# Настройки для разных платформ
if(WIN32)
    # Windows-specific настройки
    target_compile_definitions(${TARGET_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
    )
    
    # Отключаем консольное окно для GUI (если нужно)
    # set_target_properties(${TARGET_NAME} PROPERTIES
    #     WIN32_EXECUTABLE TRUE
    # )
    
    # Статическая линковка для переносимости
    set_target_properties(${TARGET_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    
    # Для MinGW
    if(MINGW)
        target_link_options(${TARGET_NAME} PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
        )
    endif()
    
elseif(UNIX AND NOT APPLE)
    # Linux-specific настройки
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
    
    # Статическая линковка для Linux (опционально)
    # target_link_options(${TARGET_NAME} PRIVATE
    #     -static-libgcc
    #     -static-libstdc++
    # )
    
elseif(APPLE)
    # macOS-specific настройки
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Установка
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES README.md
    DESTINATION share/doc/filemanager
)

# Создание пакетов (для CPack)
include(CPack)

set(CPACK_PACKAGE_NAME "filemanager")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Консольный файловый менеджер на C++")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "FileManager")
    set(CPACK_NSIS_PACKAGE_NAME "FileManager")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Для DEB пакетов
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17)")
    
    # Для RPM пакетов
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/File")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.17")
endif()

# Информация о версии
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
