#include <iostream>
#include "ConcurrentHashMap.hpp"
#include <assert.h>

#define ASSERT_EQ(a,b) assert(a == b)

using namespace std;

void test_member(ConcurrentHashMap &h) {
    // una letra que no se agrego, no esta
    ASSERT_EQ(h.member("a"), false);
    ASSERT_EQ(h.member("b"), false);
    ASSERT_EQ(h.member("c"), false);
    ASSERT_EQ(h.member("z"), false);

    // una palabra de 2 letras que no se agrego, no esta
    ASSERT_EQ(h.member("ab"), false);
    ASSERT_EQ(h.member("ag"), false);
    ASSERT_EQ(h.member("az"), false);

    // Agrego una palabra y solo esa palabra esta
    h.add("palabra");
    h.inc("palabra");
    ASSERT_EQ(h.member("palabra"), true);
    ASSERT_EQ(h.member("ab"), false);
    ASSERT_EQ(h.member("ag"), false);
    ASSERT_EQ(h.member("az"), false);

    // Agrego una palabra de una sola letra y estan solo esa y la palabra anterior
    h.add("p");
    h.inc("p");
    ASSERT_EQ(h.member("palabra"), true);
    ASSERT_EQ(h.member("p"), true);
    ASSERT_EQ(h.member("ab"), false);
    ASSERT_EQ(h.member("ag"), false);
    ASSERT_EQ(h.member("az"), false);
}

void test_addAndInc(ConcurrentHashMap &h) {
    ConcurrentHashMap nuevoH;
    ASSERT_EQ(nuevoH.count_word("a"), 0);
    nuevoH.addAndInc("a");
    ASSERT_EQ(nuevoH.count_word("a"), 1);
    nuevoH.inc("a"); // incremento a desde afuera
    nuevoH.addAndInc("a");
    ASSERT_EQ(nuevoH.count_word("a"), 3);

    // Me aseguro que las palabras que existian no hayan cambiado
    ASSERT_EQ(h.count_word("p"), 1);
    ASSERT_EQ(h.count_word("palabra"), 1);
    ASSERT_EQ(h.count_word("a"), 0);
    ASSERT_EQ(h.count_word("b"), 0);
    ASSERT_EQ(h.count_word("c"), 0);
    ASSERT_EQ(h.count_word("d"), 0);
    ASSERT_EQ(h.count_word("ab"), 0);
    ASSERT_EQ(h.count_word("ag"), 0);
    ASSERT_EQ(h.count_word("az"), 0);

    // funciona para cuando hay mas de una palabra en la misma posicion de la tabla de hash
    h.addAndInc("a");
    h.addAndInc("azucar");
    ASSERT_EQ(h.count_word("azucar"), 1);
    h.addAndInc("papa");
    ASSERT_EQ(h.count_word("papa"), 1);

    // Incrementa el valor de las palabras existentes
    h.addAndInc("p");
    ASSERT_EQ(h.count_word("p"), 2);
    h.addAndInc("palabra");
    ASSERT_EQ(h.count_word("palabra"), 2);
}

void test_maximum() {
    ConcurrentHashMap h;

    h.addAndInc("p");
    h.addAndInc("p");
    h.addAndInc("papa");
    h.addAndInc("palabra");
    h.addAndInc("palabra");
    h.addAndInc("palabra");
    h.addAndInc("azucar");
    h.addAndInc("a");

    // funciona para 1 thread
    pair<string, unsigned int> maxActual = h.maximum(1);
    ASSERT_EQ(maxActual.second, 3);
    ASSERT_EQ(maxActual.first, "palabra");

    // funciona para 2 thread
    maxActual = h.maximum(2);
    ASSERT_EQ(maxActual.second, 3);
    ASSERT_EQ(maxActual.first, "palabra");

    // funciona para 3 thread
    maxActual = h.maximum(3);
    ASSERT_EQ(maxActual.second, 3);
    ASSERT_EQ(maxActual.first, "palabra");

    // funciona para 4 threads incluso despues de que el maximon cambie
    h.addAndInc("azucar");
    h.addAndInc("azucar");
    h.addAndInc("azucar");
    maxActual = h.maximum(4);
    ASSERT_EQ(maxActual.second, 4);
    ASSERT_EQ(maxActual.first, "azucar");
}

int main(void) {
    ConcurrentHashMap h;

    // modifica h y ahora tiene p y palabra
    test_member(h);

    // modifica h y ahora tiene a 1 vez, azucar 1 vez, papa 1 vez, palabra 2 veces y p 2 veces.
    test_addAndInc(h);

    test_maximum();

    return 0;
}

