/**
 * NOME: oop_structure.hpp
 * DESCRIÇÃO: Modelo padrão para definição de classes seguindo a Orientação a Objetos.
 * FINALIDADE: Garantir encapsulamento e organização modular do código.
 * PRÁTICA: Utiliza Header Guards (#ifndef) para evitar inclusões múltiplas.
 */
#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

#include <string>

class MyClass {
public:
    MyClass(const std::string& name);
    void sayHello() const;
private:
    std::string m_name;
};

#endif
