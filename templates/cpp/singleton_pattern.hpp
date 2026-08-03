/**
 * NOME: singleton_pattern.hpp
 * DESCRIÇÃO: Garante que uma classe tenha apenas uma instância em todo o programa.
 * FINALIDADE: Gerenciadores de configuração, loggers ou pools de conexão.
 * LÓGICA: Construtor privado e deleção dos operadores de cópia/atribuição.
 */
#ifndef SINGLETON_HPP
#define SINGLETON_HPP

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance; // Thread-safe em C++11+
        return instance;
    }
    // Remove possibilidade de cópia
    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;
private:
    Singleton() {} // Construtor privado
};

#endif
