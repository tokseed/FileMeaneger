<div align="center">
  
  # 📁 FileManager Pro
  
  ### Элегантный консольный файловый менеджер на C++

  [![Linux Build](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)]()
  [![Windows Build](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)]()
  [![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)]()
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)]()

  <img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/rainbow.png" width="100%">

</div>

## ✨ О проекте

**FileManager Pro** — это мощный и интуитивно понятный консольный файловый менеджер, написанный на современном C++17. Он предоставляет полный набор инструментов для управления файловой системой прямо из командной строки.

<div align="center">
  
  ### 💫 Многострочный код? Легко!
  
</div>

Проект отличается **чистой архитектурой** и **многострочной организацией кода**, что делает его не только функциональным, но и удобным для изучения и модификации.

```cpp
// 👇 Вот так выглядит наш элегантный многострочный код
class FileManager {
private:
    fs::path currentPath;
    
public:
    FileManager() : currentPath(fs::current_path()) {
        // Инициализация менеджера
    }
    
    void showCurrentDirectory() const {
        std::cout << "📂 Текущая директория: " 
                  << currentPath.string() 
                  << '\n';
    }
    
    // И ещё много-много строк прекрасного кода...
    // ... 
    // ...
    // ....
};
