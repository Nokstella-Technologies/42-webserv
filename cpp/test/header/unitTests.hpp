#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include "Configuration.hpp"
#include <cassert>
#include <string>
#include <cstring>
#include <iostream>



void assertEq(bool exp, const char *msg);

template <typename Func, typename Err>
bool doesThrow(Func func, Err err) {
    try {
        func();  // Chama a função que estamos verificando
        return false;  // Se a função não lançar exceção, retorna false
    } catch (const Err& e) {
        return !std::strcmp(err.what(), e.what());  // If the function throws an exception, return true
    } catch (...) {
        std::cerr << "Erro inesperado" << std::endl;
        return false; // a função lançar exceção de um tipo diferente, retorna false
    }
}

void testFileName();
void testStartsWith();
void testEndsWith();
void testTrim();
#endif // UNITTESTS_HPP