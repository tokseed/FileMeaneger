#!/bin/bash

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функция для вывода с цветом
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[OK]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Проверка наличия необходимых программ
check_dependencies() {
    print_status "Проверка зависимостей..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake не найден!"
        print_status "Установите CMake:"
        echo "  Ubuntu/Debian: sudo apt install cmake"
        echo "  Fedora: sudo dnf install cmake"
        echo "  Arch: sudo pacman -S cmake"
        exit 1
    fi
    
    if ! command -v g++ &> /dev/null; then
        print_error "G++ не найден!"
        print_status "Установите компилятор:"
        echo "  Ubuntu/Debian: sudo apt install g++"
        echo "  Fedora: sudo dnf install gcc-c++"
        echo "  Arch: sudo pacman -S gcc"
        exit 1
    fi
    
    print_success "Все зависимости найдены"
}

# Сборка проекта
build_project() {
    print_status "Начало сборки проекта..."
    
    # Создание папки для сборки
    mkdir -p build
    cd build
    
    # Конфигурация CMake
    print_status "Конфигурация CMake..."
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DBUILD_SHARED_LIBS=OFF \
        -DENABLE_TESTING=OFF
    
    if [ $? -ne 0 ]; then
        print_error "Ошибка конфигурации CMake"
        exit 1
    fi
    
    # Компиляция
    print_status "Компиляция..."
    make -j$(nproc)
    
    if [ $? -ne 0 ]; then
        print_error "Ошибка компиляции"
        exit 1
    fi
    
    print_success "Сборка завершена успешно!"
    
    # Показываем размер файла
    if [ -f "filemanager" ]; then
        size=$(du -h filemanager | cut -f1)
        echo "  Размер: $size"
    fi
    
    cd ..
}

# Установка в систему
install_project() {
    if [ -f "build/filemanager" ]; then
        print_status "Установка в систему (требуются права sudo)..."
        cd build
        sudo make install
        if [ $? -eq 0 ]; then
            print_success "Установка завершена!"
            echo "  Запускайте: filemanager"
        else
            print_error "Ошибка установки"
        fi
        cd ..
    else
        print_error "Сначала выполните сборку"
    fi
}

# Создание пакета
create_package() {
    print_status "Создание пакета для распространения..."
    cd build
    make package
    if [ $? -eq 0 ]; then
        print_success "Пакет создан:"
        ls -lh *.deb *.rpm *.tar.gz 2>/dev/null || echo "  Пакеты не созданы"
    else
        print_error "Ошибка создания пакета"
    fi
    cd ..
}

# Очистка
clean_project() {
    print_status "Очистка..."
    rm -rf build
    print_success "Очистка завершена"
}

# Тестирование
test_project() {
    if [ -f "build/filemanager" ]; then
        print_status "Запуск тестов..."
        build/filemanager --help
        if [ $? -eq 0 ]; then
            print_success "Тест пройден"
        else
            print_error "Ошибка теста"
        fi
    else
        print_error "Сначала выполните сборку"
    fi
}

# Показать справку
show_help() {
    echo "Использование: $0 [КОМАНДА]"
    echo ""
    echo "Команды:"
    echo "  all         - полная сборка (по умолчанию)"
    echo "  build       - только сборка"
    echo "  install     - сборка и установка"
    echo "  package     - сборка и создание пакета"
    echo "  clean       - очистка"
    echo "  test        - тестирование"
    echo "  help        - показать справку"
    echo ""
    echo "Примеры:"
    echo "  $0          # полная сборка"
    echo "  $0 install  # сборка и установка"
    echo "  $0 clean    # очистка"
}

# Основная логика
case "${1:-all}" in
    all)
        check_dependencies
        clean_project
        build_project
        test_project
        ;;
    build)
        check_dependencies
        build_project
        ;;
    install)
        check_dependencies
        build_project
        install_project
        ;;
    package)
        check_dependencies
        build_project
        create_package
        ;;
    clean)
        clean_project
        ;;
    test)
        test_project
        ;;
    help)
        show_help
        ;;
    *)
        print_error "Неизвестная команда: $1"
        show_help
        exit 1
        ;;
esac
