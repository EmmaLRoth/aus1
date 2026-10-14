#ifndef SEMESTRAL_PROJECT_COMPARATORS_HPP
#define SEMESTRAL_PROJECT_COMPARATORS_HPP

#include "libds/adt/sorts.hpp"
#include "libds/amt/implicit_sequence.hpp"
#include "models.hpp"
#include <functional>
#include <iostream>

/**
 * @class Comparator
 * @brief Zapuzdruje komparačné logiky a algoritmus pre zoraďovanie dát.
 */
class Comparator {
    public:
        /**
        * @brief Vytvorí komparátor, ktorý porovnáva územné jednotky abecedne podľa názvu.
        *
        * @return Komparátor typu `bool(const TerritorialUnit<District>&, const TerritorialUnit<District>&)`,
        *         ktorý vráti `true` ak názov prvej jednotky je lexikograficky menší ako názov druhej.
        */
        inline std::function<bool(const TerritorialUnit<District>&, const TerritorialUnit<District>&)> compareAlphabetical() {
            return [](const TerritorialUnit<District>& first, const TerritorialUnit<District>& second) {
                return first.name < second.name;
            };
        }

        /**
        * @brief Vytvorí komparátor, ktorý porovnáva územné jednotky podľa počtu hlasov zadanej strany.
        *
        * Porovnáva celkové hlasy (`total_votes`) pre stranu s indexom @p partyId.
        * Zoradenie je zostupné – jednotka s vyšším počtom hlasov je považovaná za menšiu.
        *
        * @param partyId Id strany vo vektore `total_votes`.
        * @return Komparátor typu `bool(const TerritorialUnit<District>&, const TerritorialUnit<District>&)`.
        */
        inline std::function<bool(const TerritorialUnit<District>&, const TerritorialUnit<District>&)> comparePartyResult(int partyId) {
            return [partyId](const TerritorialUnit<District>& first, const TerritorialUnit<District>& second) {
                if (partyId - 1 >= 0) {
                    return first.total_votes[partyId - 1] > second.total_votes[partyId - 1];
                }
                return false;
            };
        }

        /**
        * @brief Interaktívne zoradí sekvenciu územných jednotiek podľa zvoleného komparátora.
        *
        * Používateľ si cez štandardný vstup vyberie typ zoradenia:
        *  1. abecedne podľa názvu (`compareAlphabetical`)
        *  2. zostupne podľa počtu hlasov zvolenej strany (`comparePartyResult`)
        *
        * Pri voľbe 2 sa vypíše zoznam strán a používateľ zadá id strany.
        * Na zoradenie sa používa algoritmus Merge Sort.
        *
        * @param sequence Referencia na implicitnú sekvenciu územných jednotiek, ktorá sa zoradí.
        * @param parties  Referencia na vektor politických strán (použitý pri výpise možností).
        *
        * @throws std::invalid_argument Ak je vstup neplatný (nečíslo alebo záporné ID strany).
        */
        inline void startSort(ds::amt::ImplicitSequence<TerritorialUnit<District>>& sequence, std::vector<PoliticalParty>& parties) {
            ds::adt::MergeSort<TerritorialUnit<District>> mergeSort;
            int choice;
            std::cout << "Vyberte typ komparátora:\n 1. compareAlphabetical\n 2. comparePartyResult\n";
            std::cout << "Voľba: ";
            std::cin >> choice;

            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid argument!");
            }

            switch (choice) {
                case 1:
                    mergeSort.sort(sequence, compareAlphabetical());
                    break;
                case 2:
                    for (PoliticalParty& p : parties) {
                        std::cout << p.id << " - " << p.short_party_name << "\n";
                    }

                    int id;
                    std::cout << "Zadajte číslo politickej strany: ";
                    std::cin >> id;

                    if (std::cin.fail() || (id < 0 && id >= parties.size())) {
                        throw std::invalid_argument("Invalid argument!");
                    }

                    mergeSort.sort(sequence, comparePartyResult(id));
                    break;
                    
                default:
                    break;
            }
        }
};

#endif