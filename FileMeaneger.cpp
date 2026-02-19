#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace fs = std::filesystem;

// Функция для преобразования file_time_type в строку (кроссплатформенная версия)
std::string timeToString(fs::file_time_type ftime) {
    try {
        // Преобразуем file_time в system_clock time_point
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

        std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

        std::tm tm = {};

#ifdef _WIN32
        // Windows: используем localtime_s
        if (localtime_s(&tm, &tt) != 0) {
            return "Ошибка преобразования времени";
        }
#else
        // Linux/Unix: используем localtime_r
        if (localtime_r(&tt, &tm) == nullptr) {
            return "Ошибка преобразования времени";
        }
#endif

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    catch (const std::exception& e) {
        return std::string("Ошибка времени: ") + e.what();
    }
}

// Базовый класс для работы с файлами и папками
class FileSystemObject {
protected:
    fs::path path;

public:
    FileSystemObject(const std::string& p) : path(p) {}

    virtual ~FileSystemObject() = default;

    bool exists() const {
        return fs::exists(path);
    }

    std::string getPath() const {
        return path.string();
    }

    virtual void showInfo() const = 0;
};

// Класс для работы с файлами
class File : public FileSystemObject {
public:
    File(const std::string& p) : FileSystemObject(p) {}

    void showInfo() const override {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                std::cout << "Файл: " << path.filename().string() << "\n"
                    << "Размер: " << fs::file_size(path) << " байт\n"
                    << "Последнее изменение: " << timeToString(fs::last_write_time(path)) << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при получении информации о файле: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Файл не существует: " << path.filename().string() << "\n";
        }
    }

    void create() {
        setlocale(LC_ALL, "ru");
        try {
            std::ofstream file(path);
            if (file.is_open()) {
                file.close();
                std::cout << "Файл создан: " << path.filename().string() << "\n";
            }
        }
        catch (const std::exception& e) {
            setlocale(LC_ALL, "ru");
            std::cout << "Ошибка при создании файла: " << e.what() << "\n";
        }
    }

    void deleteFile() {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                fs::remove(path);
                std::cout << "Файл удален: " << path.filename().string() << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при удалении файла: " << e.what() << "\n";
            }
        }
        else {
            setlocale(LC_ALL, "ru");
            std::cout << "Файл не существует: " << path.filename().string() << "\n";
        }
    }

    void write(const std::string& content) {
        setlocale(LC_ALL, "ru");
        try {
            std::ofstream file(path);
            if (file.is_open()) {
                file << content;
                file.close();
                std::cout << "Содержимое записано в файл: " << path.filename().string() << "\n";
            }
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка при записи в файл: " << e.what() << "\n";
        }
    }

    std::string read() const {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                std::ifstream file(path);
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
                return content;
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при чтении файла: " << e.what() << "\n";
                return "";
            }
        }
        return "";
    }
};

// Класс для работы с папками
class Directory : public FileSystemObject {
public:
    Directory(const std::string& p) : FileSystemObject(p) {}

    void showInfo() const override {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                int fileCount = 0;
                int dirCount = 0;

                for (const auto& entry : fs::directory_iterator(path)) {
                    if (entry.is_directory()) {
                        dirCount++;
                    }
                    else {
                        fileCount++;
                    }
                }

                std::cout << "Папка: " << path.filename().string() << '\n'
                    << "Файлов: " << fileCount << '\n'
                    << "Папок: " << dirCount << '\n'
                    << "Последнее изменение: " << timeToString(fs::last_write_time(path)) << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при получении информации о папке: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Папка не существует: " << path.filename().string() << "\n";
        }
    }

    void create() {
        setlocale(LC_ALL, "ru");
        if (!exists()) {
            try {
                fs::create_directories(path);
                std::cout << "Папка создана: " << path.filename().string() << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при создании папки: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Папка уже существует: " << path.filename().string() << "\n";
        }
    }

    void deleteDir() {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                fs::remove_all(path);
                std::cout << "Папка удалена: " << path.filename().string() << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при удалении папки: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Папка не существует: " << path.filename().string() << "\n";
        }
    }

    void listContents(bool detailed = false) const {
        setlocale(LC_ALL, "ru");
        if (exists()) {
            try {
                std::vector<fs::directory_entry> entries;
                for (const auto& entry : fs::directory_iterator(path)) {
                    entries.push_back(entry);
                }

                // Сортировка: сначала папки, потом файлы
                std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
                    if (a.is_directory() && !b.is_directory()) return true;
                    if (!a.is_directory() && b.is_directory()) return false;
                    return a.path().filename().string() < b.path().filename().string();
                    });

                for (const auto& entry : entries) {
                    if (detailed) {
                        try {
                            if (entry.is_directory()) {
                                std::cout << "[DIR]  ";
                            }
                            else {
                                std::cout << "[FILE] ";
                                std::cout << std::setw(10) << entry.file_size() << " bytes ";
                            }
                            std::cout << std::setw(20) << std::left << entry.path().filename().string()
                                << " " << timeToString(entry.last_write_time()) << '\n';
                        }
                        catch (const std::exception& e) {
                            std::cout << "[ERROR] " << entry.path().filename().string() << " - ошибка доступа\n";
                        }
                    }
                    else {
                        std::cout << entry.path().filename().string() << '\n';
                    }
                }
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при чтении содержимого папки: " << e.what() << "\n";
            }
        }
        else {
            std::cout << "Папка не существует: " << path.filename().string() << "\n";
        }
    }

    size_t getFileCount() const {
        size_t count = 0;
        if (exists()) {
            try {
                for (const auto& entry : fs::directory_iterator(path)) {
                    if (!entry.is_directory()) {
                        count++;
                    }
                }
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при подсчете файлов: " << e.what() << "\n";
            }
        }
        return count;
    }
};

// Класс файлового менеджера
class FileManager {
private:
    fs::path currentPath;

public:
    FileManager() : currentPath(fs::current_path()) {}

    void showCurrentDirectory() const {
        setlocale(LC_ALL, "ru");
        std::cout << "Текущая директория: " << currentPath.string() << '\n';
    }

    void changeDirectory(const std::string& path) {
        setlocale(LC_ALL, "ru");
        try {
            fs::path newPath;
            if (fs::path(path).is_absolute()) {
                newPath = path;
            }
            else {
                newPath = currentPath / path;
            }

            if (fs::exists(newPath) && fs::is_directory(newPath)) {
                currentPath = fs::canonical(newPath);
                std::cout << "Текущая директория: " << currentPath.string() << '\n';
            }
            else {
                std::cout << "Директория не существует: " << newPath.string() << '\n';
            }
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка при смене директории: " << e.what() << '\n';
        }
    }

    void goToParent() {
        setlocale(LC_ALL, "ru");
        if (currentPath.has_parent_path()) {
            try {
                currentPath = currentPath.parent_path();
                std::cout << "Текущая директория: " << currentPath.string() << '\n';
            }
            catch (const std::exception& e) {
                std::cout << "Ошибка при переходе в родительскую директорию: " << e.what() << '\n';
            }
        }
        else {
            std::cout << "У текущей директории нет родительской\n";
        }
    }

    void createFile(const std::string& name) {
        File file((currentPath / name).string());
        file.create();
    }

    void createDirectory(const std::string& name) {
        Directory dir((currentPath / name).string());
        dir.create();
    }

    void deleteItem(const std::string& name) {
        fs::path itemPath = currentPath / name;

        if (!fs::exists(itemPath)) {
            setlocale(LC_ALL, "ru");
            std::cout << "Объект не существует: " << name << '\n';
            return;
        }

        if (fs::is_directory(itemPath)) {
            Directory dir(itemPath.string());
            dir.deleteDir();
        }
        else {
            File file(itemPath.string());
            file.deleteFile();
        }
    }

    void renameItem(const std::string& oldName, const std::string& newName) {
        try {
            fs::path oldPath = currentPath / oldName;
            fs::path newPath = currentPath / newName;

            if (!fs::exists(oldPath)) {
                setlocale(LC_ALL, "ru");
                std::cout << "Объект не существует: " << oldName << '\n';
                return;
            }

            if (fs::exists(newPath)) {
                std::cout << "Объект с таким именем уже существует: " << newName << '\n';
                return;
            }

            fs::rename(oldPath, newPath);
            std::cout << "Переименовано: " << oldName << " -> " << newName << '\n';
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка при переименовании: " << e.what() << '\n';
        }
    }

    void copyItem(const std::string& source, const std::string& destination) {
        setlocale(LC_ALL, "ru");
        try {
            fs::path sourcePath = currentPath / source;
            fs::path destPath = currentPath / destination;

            if (!fs::exists(sourcePath)) {
                std::cout << "Источник не существует: " << source << '\n';
                return;
            }

            if (fs::is_directory(sourcePath)) {
                fs::copy(sourcePath, destPath, fs::copy_options::recursive);
            }
            else {
                fs::copy(sourcePath, destPath);
            }
            std::cout << "Скопировано: " << source << " -> " << destination << '\n';
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка копирования: " << e.what() << '\n';
        }
    }

    void moveItem(const std::string& source, const std::string& destination) {
        setlocale(LC_ALL, "ru");
        try {
            fs::path sourcePath = currentPath / source;
            fs::path destPath = currentPath / destination;

            if (!fs::exists(sourcePath)) {
                std::cout << "Источник не существует: " << source << '\n';
                return;
            }

            fs::rename(sourcePath, destPath);
            std::cout << "Перемещено: " << source << " -> " << destination << '\n';
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка перемещения: " << e.what() << '\n';
        }
    }

    void showItemInfo(const std::string& name) {
        setlocale(LC_ALL, "ru");
        fs::path itemPath = currentPath / name;

        if (!fs::exists(itemPath)) {
            std::cout << "Объект не существует: " << name << '\n';
            return;
        }

        if (fs::is_directory(itemPath)) {
            Directory dir(itemPath.string());
            dir.showInfo();
        }
        else {
            File file(itemPath.string());
            file.showInfo();
        }
    }

    void listContents(bool detailed = false) {
        setlocale(LC_ALL, "ru");
        Directory dir(currentPath.string());
        if (detailed) {
            std::cout << "Содержимое директории " << currentPath.string() << ":\n";
        }
        dir.listContents(detailed);
    }

    void searchFiles(const std::string& pattern) {
        setlocale(LC_ALL, "ru");
        std::cout << "Поиск файлов с шаблоном: " << pattern << "\n";

        try {
            for (const auto& entry : fs::recursive_directory_iterator(currentPath)) {
                if (!entry.is_directory()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find(pattern) != std::string::npos) {
                        std::cout << entry.path().string() << "\n";
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка при поиске: " << e.what() << "\n";
        }
    }

    std::string getCurrentPath() const {
        return currentPath.string();
    }

    bool itemExists(const std::string& name) const {
        return fs::exists(currentPath / name);
    }

    uintmax_t getItemSize(const std::string& name) const {
        try {
            return fs::file_size(currentPath / name);
        }
        catch (...) {
            return 0;
        }
    }
};

// Функция для отображения помощи
void showHelp() {
    setlocale(LC_ALL, "ru");
    std::cout << "Команды файлового менеджера:\n"
        << "  cd <path>      - сменить директорию\n"
        << "  ls             - список файлов\n"
        << "  ls -l          - подробный список\n"
        << "  mkdir <name>   - создать папку\n"
        << "  touch <name>   - создать файл\n"
        << "  rm <name>      - удалить\n"
        << "  mv <old> <new> - переименовать/переместить\n"
        << "  cp <src> <dst> - скопировать\n"
        << "  info <name>    - информация об объекте\n"
        << "  search <pattern> - поиск файлов\n"
        << "  pwd           - текущая директория\n"
        << "  up            - на уровень выше\n"
        << "  help          - помощь\n"
        << "  exit          - выход\n";
}

int main() {
    setlocale(LC_ALL, "ru");
    FileManager fm;
    std::string command;

    std::cout << "Добро пожаловать в файловый менеджер!\n";
    fm.showCurrentDirectory();
    showHelp();

    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, command);

        if (command.empty()) continue;

        if (command == "exit") {
            break;
        }
        else if (command == "help") {
            showHelp();
        }
        else if (command == "pwd") {
            fm.showCurrentDirectory();
        }
        else if (command == "up") {
            fm.goToParent();
        }
        else if (command == "ls") {
            fm.listContents();
        }
        else if (command == "ls -l") {
            fm.listContents(true);
        }
        else if (command.find("cd ") == 0) {
            if (command.length() > 3) {
                fm.changeDirectory(command.substr(3));
            }
            else {
                std::cout << "Укажите путь для перехода\n";
            }
        }
        else if (command.find("mkdir ") == 0) {
            if (command.length() > 6) {
                fm.createDirectory(command.substr(6));
            }
            else {
                std::cout << "Укажите имя папки\n";
            }
        }
        else if (command.find("touch ") == 0) {
            if (command.length() > 6) {
                fm.createFile(command.substr(6));
            }
            else {
                std::cout << "Укажите имя файла\n";
            }
        }
        else if (command.find("rm ") == 0) {
            if (command.length() > 3) {
                fm.deleteItem(command.substr(3));
            }
            else {
                std::cout << "Укажите имя объекта для удаления\n";
            }
        }
        else if (command.find("info ") == 0) {
            if (command.length() > 5) {
                fm.showItemInfo(command.substr(5));
            }
            else {
                std::cout << "Укажите имя объекта\n";
            }
        }
        else if (command.find("search ") == 0) {
            if (command.length() > 7) {
                fm.searchFiles(command.substr(7));
            }
            else {
                std::cout << "Укажите шаблон для поиска\n";
            }
        }
        else {
            // Обработка команд с несколькими аргументами
            size_t space1 = command.find(' ');
            if (space1 != std::string::npos) {
                std::string cmd = command.substr(0, space1);
                std::string args = command.substr(space1 + 1);

                size_t space2 = args.find(' ');
                if (space2 != std::string::npos) {
                    std::string arg1 = args.substr(0, space2);
                    std::string arg2 = args.substr(space2 + 1);

                    if (cmd == "mv") {
                        fm.moveItem(arg1, arg2);
                    }
                    else if (cmd == "cp") {
                        fm.copyItem(arg1, arg2);
                    }
                    else if (cmd == "rename") {
                        fm.renameItem(arg1, arg2);
                    }
                    else {
                        std::cout << "Неизвестная команда: " << cmd << "\n";
                    }
                }
                else {
                    std::cout << "Недостаточно аргументов для команды " << cmd << "\n";
                }
            }
            else {
                std::cout << "Неизвестная команда: " << command << "\n";
            }
        }
    }

    std::cout << "До свидания!\n";
    return 0;
}
