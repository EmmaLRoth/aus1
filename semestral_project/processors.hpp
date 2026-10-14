#ifndef SEMESTRAL_PROJECT_PROCESSORS_HPP
#define SEMESTRAL_PROJECT_PROCESSORS_HPP

#include <functional>
#include <iostream>
#include "libds/amt/implicit_sequence.hpp"
#include "models.hpp"

/**
 * @brief Vytvorí procesor, ktorý vkladá územné jednotky do vektora.
 *
 * Vrátená funkcia pri každom zavolaní pridá územnú jednotku
 * na koniec zadaného vektora @p results.
 *
 * @param results Referencia na vektor, do ktorého sa budú jednotky vkladať.
 * @return Funkcia typu `void(TerritorialUnit<District>&)`.
 */
inline std::function<void(TerritorialUnit<District>&)> procInsertInto(std::vector<TerritorialUnit<District>>& results) {
    return [&](TerritorialUnit<District>& t) {
        results.push_back(t);
    };
}

/**
 * @brief Vytvorí procesor, ktorý vkladá územné jednotky do implicitnej sekvencie.
 *
 * Vrátená funkcia pri každom zavolaní pridá územnú jednotku
 * na koniec zadanej implicitnej sekvencie @p results.
 *
 * @param results Referencia na implicitnú sekvenciu, do ktorej sa budú jednotky vkladať.
 * @return Funkcia typu `void(TerritorialUnit<District>&)`.
 */
inline std::function<void(TerritorialUnit<District>&)> procInsertIntoSeq(ds::amt::ImplicitSequence<TerritorialUnit<District>>& results) {
    return [&](TerritorialUnit<District>& t) {
        auto& block = results.emplaceLast();
        block.data = t;
    };
}

/**
 * @brief Vytvorí procesor, ktorý vypíše základné informácie o územnej jednotke.
 *
 * Vrátená funkcia pri každom zavolaní vypíše názov a typ územnej jednotky
 * na štandardný výstup vo formáte: `názov [typ]`.
 *
 * @return Funkcia typu `void(TerritorialUnit<District>&)`.
 */
inline std::function<void(TerritorialUnit<District>&)> procPrint() {
    return [&](TerritorialUnit<District>& t) {
        std::cout << t.name << " [" << enumToString(t.type) << "]" << '\n';
    };
}

#endif