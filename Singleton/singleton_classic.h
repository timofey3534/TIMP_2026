#pragma once

// ── Классическая реализация Singleton ────────────────────────────────────────
// Проблема: память освобождается только тем, кто использует класс (извне),
// а не автоматически внутри класса.
class SingletonClassic
{
private:
    static SingletonClassic* p_instance;

    SingletonClassic() {}
    SingletonClassic(const SingletonClassic&);
    SingletonClassic& operator=(SingletonClassic&);

public:
    static SingletonClassic* getInstance() {
        if (!p_instance)
            p_instance = new SingletonClassic();
        return p_instance;
    }
};

// Определение статического члена (в .cpp):
// SingletonClassic* SingletonClassic::p_instance = nullptr;
