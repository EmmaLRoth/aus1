#include <functional>
#include <libds/mm/system_memory_manager.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <libds/adt/table.hpp>
#include "models.hpp"
#include "csv_loader.hpp"
#include "predicates.hpp"
#include "processors.hpp"
#include "comparators.hpp"
#include "algorithm.hpp"
#include "hierarchy_manager.hpp"
#include "table_manager.hpp"
#include <libds/amt/explicit_hierarchy.hpp>
#include <heap/heap-monitor.hpp>


using namespace ds::amt;
using namespace ds::adt;

/**
 * @brief Interaktívne vyhľadávanie územných jednotiek pomocou predikátov (Úroveň 1 a 2).
 *
 * Používateľ si vyberie predikát a program na základe neho filtruje a vypisuje
 * územné jednotky z daného rozsahu iterátorov. Pri `uroven1 == true` sa filtrujú
 * len obce (UnitType::OBEC), inak sa filtruje celý rozsah.
 *
 * Dostupné predikáty:
 * - 1 – `predStartsWithStr`: filtrovanie podľa prefixu názvu
 * - 2 – `predPartyHasVotes`: filtrovanie podľa počtu hlasov strany
 * - 3 – `predTrue`: vypíše všetky jednotky bez filtrovania
 * - 4 – `predHasType`: filtrovanie podľa typu územnej jednotky
 * - 5 – návrat do menu
 *
 * @tparam Iterator Typ iterátora pre prechádzanie územných jednotiek.
 *
 * @param begin Iterátor na začiatok rozsahu.
 * @param end Iterátor na koniec rozsahu.
 * @param districts Vektor volebných okrskov.
 * @param parties Vektor politických strán.
 * @param uroven1 Ak `true`, filtruje len obce; inak filtruje celý rozsah.
 * @return Číslo zvolenej možnosti (5 = späť do menu).
 *
 * @throws std::invalid_argument Ak je vstup neplatný alebo mimo rozsahu.
 */

template<typename Iterator>
int program1(Iterator begin, Iterator end, std::vector<District>& districts, std::vector<PoliticalParty>& parties, bool uroven1) {
    Algorithm alg;
    int number; 
    std::cout << "Zvolte predikát: \n.............................. \n";
    std::cout << "1. predStartWithStr \n2. predPartyHasVotes \n3. predTrue \n4. hasType \n5. späťDoMenu\n";
    std::cout << "Voľba: ";
    std::cin >> number;

    if (std::cin.fail()) {
        throw std::invalid_argument("Invalid argument!");
    }

    std::vector<TerritorialUnit<District>> find_results;

    switch (number) {
        case 1: {
            std::cout << "\033[2J\033[1;1H";

            std::string prefix;
            std::cout << "Zadajte prefix: ";
            std::cin >> prefix;

            auto pred_starts_with = predStartsWithStr(prefix);

            auto proc_insert_into = procInsertInto(find_results);

            if (uroven1) {
                alg.algoritmus(proc_insert_into, 
                [&](const TerritorialUnit<District>& unit) {
                    return unit.type == UnitType::OBEC && pred_starts_with(unit);
                }, begin, end);
            } else {
                alg.algoritmus(proc_insert_into, pred_starts_with, begin, end);
            }

            auto proc_print = procPrint();
            auto pred_true = predTrue();

            alg.algoritmus(proc_print, pred_true, find_results.begin(), find_results.end());
            
            break;
        }
        case 2: {
            std::cout << "\033[2J\033[1;1H";

            for (PoliticalParty& p : parties) {
                std::cout << p.id << " - " << p.short_party_name << "\n";
            }

            int id;
            std::cout << "Zadajte číslo politickej strany: ";
            std::cin >> id;

            if (std::cin.fail() || id < 0 || id >= parties.size()) {
                throw std::invalid_argument("Invalid argument!");
            }

            int predict_votes;
            std::cout << "Zadajte predpokladaný počet hlasov tejto strany: ";
            std::cin >> predict_votes;

            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid argument!");
            }

            auto pred_party_has_votes = predPartyHasVotes(id, predict_votes);

            auto proc_insert_into = procInsertInto(find_results);

            if (uroven1) {
                alg.algoritmus(proc_insert_into, 
               [&](const TerritorialUnit<District>& unit) {
                   return unit.type == UnitType::OBEC && pred_party_has_votes(unit);
               }, begin, end);
            } else {
                alg.algoritmus(proc_insert_into, pred_party_has_votes, begin, end);
            }

            auto proc_print = procPrint();
            auto pred_true = predTrue();

            alg.algoritmus(proc_print, pred_true, find_results.begin(), find_results.end());

            std::cout << "pocet vysledkov: " << find_results.size();
            break;
        }
        case 3: {
            std::cout << "\033[2J\033[1;1H";

            auto proc_print = procPrint();
            auto pred_true = predTrue();

            if (uroven1) {
                alg.algoritmus(proc_print, [](const TerritorialUnit<District>& unit) {
                        return unit.type == UnitType::OBEC;
                    }, begin, end);
            } else {
                alg.algoritmus(proc_print, pred_true, begin, end);
            }

            break;
        }

        case 4: {
            std::cout << "\033[2J\033[1;1H";

            std::string type;
            std::cout << "Zadajte typ územnej jednotky (čr/kraj/okres/obec): ";
            std::cin >> type;

            auto pred_has_type = predHasType(type);

            auto proc_insert_into = procInsertInto(find_results);

            if (uroven1) {
                alg.algoritmus(proc_insert_into, [&](const TerritorialUnit<District>& unit) {
                        return unit.type == UnitType::OBEC && pred_has_type(unit);
                    }, begin, end);
            } else {
                alg.algoritmus(proc_insert_into, pred_has_type, begin, end);
            }

            auto proc_print = procPrint();
            auto pred_true = predTrue();

            alg.algoritmus(proc_print, pred_true, find_results.begin(), find_results.end());
            
            break;
        }
        case 5: {
            std::cout << "\033[2J\033[1;1H";

            break;
        }
        default: {
            throw std::invalid_argument("Invalid argument!");
        }
    }

    return number;
}

/**
 * @brief Interaktívne prechádzanie hierarchie územných jednotiek (Úroveň 2).
 *
 * Umožňuje pohyb po stromovej hierarchii nahor/nadol, spúšťanie algoritmov
 * nad aktuálnym podstromom a zobrazenie informácií o aktuálnom uzle.
 *
 * Dostupné akcie:
 * - 1 – posun o úroveň vyššie
 * - 2 – posun na i-tého syna (zobrazí zoznam synov)
 * - 3 – spustenie `program1` nad podstromom aktuálneho uzla
 * - 4 – zobrazenie informácií o aktuálnom uzle (názov, typ, hlasy)
 * - 5 – návrat do menu
 *
 * @param terr_units Vektor územných jednotiek.
 * @param districts Vektor volebných okrskov.
 * @param parties Vektor politických strán.
 * @param hierarchy Referencia na hierarchiu územných jednotiek.
 * @param iterator Referencia na iterátor hierarchie (udržiava aktuálnu pozíciu).
 * @return Číslo zvolenej možnosti (5 = späť do menu).
 *
 * @throws std::invalid_argument Ak je vstup neplatný.
 */
int program2(std::vector<TerritorialUnit<District>>& terr_units, std::vector<District>& districts, std::vector<PoliticalParty>& parties, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy, HierarchyIterator& iterator) {
    int number; 
    std::cout << "Aktuálna pozícia: " << iterator.getCurrentData().name << "\n";

    std::cout << "Zvolte úroveň: \n.............................. \n";
    std::cout << "1. posun o úroveň vyššie \n2. posun na i-teho syna \n3. algoritmus \n4. info \n5. späťDoMenu \n";
    std::cout << "Voľba: ";
    std::cin >> number;

    if (std::cin.fail()) {
        throw std::invalid_argument("Invalid argument!");
    }

    switch(number) {
        case 1: {
            std::cout << "\033[2J\033[1;1H";

            iterator.goUp();
            break;
        }
        case 2: {
            std::cout << "\033[2J\033[1;1H";

            if (iterator.getDegree() == 0) {
                std::cout << "Si v liste. Nedá sa ísť nižšie." << "\n";
                break;
            }

            int numberOfSons = iterator.getDegree();
            std::cout << "----Zoznam synov----\n";

            for (int i = 0; i < numberOfSons; ++i) {
                auto son = hierarchy.accessSon(*iterator.getCurrentBlock(), i);
                if (son != nullptr) {
                    std::cout << "[" << i << "] " << son->data.name << "\n";
                }
            }

            int index; 
            std::cout << "Zadajte index syna: ";
            std::cin >> index;

            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid argument!");
            }

            iterator.goDown(index);

            break;
        }
        case 3: {
            std::cout << "\033[2J\033[1;1H";

            int choice = 0;
            while (choice != 5) {
                choice = program1(iterator.begin(), iterator.end(), districts, parties, false);
                std::cout << "\n";
            }

            break;
        }
        case 4: {
            std::cout << "\033[2J\033[1;1H";

            int sum = 0;
            for (int i = 0; i < iterator.getCurrentData().total_votes.size(); ++i) {
                sum += iterator.getCurrentData().total_votes[i];
            }


            std::cout << iterator.getCurrentData().name <<"\n";
            std::cout << "typ územnej jednotky: " << enumToString(iterator.getCurrentData().type) << "\n";
            std::cout << "celkový počet hlasov: " << sum << "\n";
            std::cout << "počty hlasov jednotlivých strán: " << "\n";
            for (int i = 0; i < iterator.getCurrentData().total_votes.size(); ++i) {
                std::cout << parties[i].short_party_name << " - " << iterator.getCurrentData().total_votes[i] << "\n";
            }
            
            break;
        }
        case 5: {
            std::cout << "\033[2J\033[1;1H";
            break;
        }
        default: {
            throw std::invalid_argument("Invalid argument!");
        }
    }
    return number;
}

/**
 * @brief Vyhľadávanie územných jednotiek podľa názvu v hešovacích tabuľkách (Úroveň 3).
 *
 * Používateľ zadá typ územnej jednotky (obec/okres/kraj) a jej názov.
 * Program vyhľadá zhodu v príslušnej hešovacej tabuľke a vypíše
 * detailné informácie o nájdených jednotkách vrátane hlasov pre každú stranu.
 *
 * @param towns Hešovacia tabuľka obcí indexovaná názvom.
 * @param municipalities Hešovacia tabuľka okresov indexovaná názvom.
 * @param regions Hešovacia tabuľka krajov indexovaná názvom.
 * @param parties Vektor politických strán.
 *
 * @throws std::invalid_argument Ak zadaný typ územnej jednotky nie je platný.
 */
void program3(HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& towns, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& municipalities, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& regions,
    std::vector<PoliticalParty>& parties) {
    std::string type; 
    std::cout << "Zadajte typ územnej jednotky(obec, okres, kraj): ";
    std::cin >> type;

    if (type != "kraj" && type != "okres" && type != "obec") {
        throw std::invalid_argument("Invalid argument!");
    }

    std::string unitName; 
    std::cout << "Zadajte názov územnej jednotky: ";
    std::cin.ignore();
    std::getline(std::cin, unitName);

    auto printInfo = [](ImplicitSequence<TerritorialUnit<District>*> r, std::vector<PoliticalParty>& p) {
        for (auto& item : r) {
            int sum = 0;
            for (int i = 0; i < item->total_votes.size(); ++i) {
                sum += item->total_votes[i];
            }


            std::cout << item->name <<"\n";
            std::cout << "typ územnej jednotky: " << enumToString(item->type) << "\n";
            std::cout << "celkový počet hlasov: " << sum << "\n";
            std::cout << "počty hlasov jednotlivých strán: " << "\n";
            for (int i = 0; i < item->total_votes.size(); ++i) {
                std::cout << p[i].short_party_name << " - " << item->total_votes[i] << "\n";
            }
        }
    };

    ImplicitSequence<TerritorialUnit<District>*>* found;

    if (type == "kraj") {
        if(regions.tryFind(unitName, found)) {
            printInfo(*found, parties);
        } else {
            std::cout << "Daná územná jednotka neexistuje. \n";
        }
        return;
        
    } else if (type == "okres") {
        if(municipalities.tryFind(unitName, found)) {
            printInfo(*found, parties);
        } else {
            std::cout << "Daná územná jednotka neexistuje. \n";
        }
        return;

    } else if (type == "obec") {
        if(towns.tryFind(unitName, found)) {
            printInfo(*found, parties);
        } else {
            std::cout << "Daná územná jednotka neexistuje. \n";
        }
        return;

    }
}

/**
 * @brief Interaktívne prechádzanie hierarchie s možnosťou filtrovania a zoradenia výsledkov (Úroveň 4).
 *
 * Rozšírená verzia `program2`. Navyše umožňuje po aplikovaní predikátu
 * výsledky zoradiť pomocou `startSort` a vypísať ich.
 *
 * Dostupné akcie:
 * - 1 – posun o úroveň vyššie
 * - 2 – posun na i-tého syna
 * - 3 – filtrovanie podstromu predikátom + zoradenie výsledkov
 * - 4 – zobrazenie informácií o aktuálnom uzle
 * - 5 – návrat do menu
 *
 * @param terr_units Vektor územných jednotiek.
 * @param districts Vektor volebných okrskov.
 * @param parties Vektor politických strán.
 * @param hierarchy Referencia na hierarchiu územných jednotiek.
 * @param iterator Referencia na iterátor hierarchie (udržiava aktuálnu pozíciu).
 * @return Číslo zvolenej možnosti (5 = späť do menu).
 *
 * @throws std::invalid_argument Ak je vstup neplatný.
 */
int program4(std::vector<TerritorialUnit<District>>& terr_units, std::vector<District>& districts, std::vector<PoliticalParty>& parties, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy, HierarchyIterator& iterator) {
    int number; 
    std::cout << "Aktuálna pozícia: " << iterator.getCurrentData().name << "\n";

    std::cout << "Zvolte úroveň: \n.............................. \n";
    std::cout << "1. posun o úroveň vyššie \n2. posun na i-teho syna \n3. algoritmus \n4. info \n5. späťDoMenu \n";
    std::cout << "Voľba: ";
    std::cin >> number;

    if (std::cin.fail()) {
        throw std::invalid_argument("Invalid argument!");
    }

    switch(number) {
        case 1: {
            std::cout << "\033[2J\033[1;1H";
            iterator.goUp();
            break;
        }
        case 2: {
            std::cout << "\033[2J\033[1;1H";

            if (iterator.getDegree() == 0) {
                std::cout << "Si v liste. Nedá sa ísť nižšie." << "\n";
                break;
            }

            int numberOfSons = iterator.getDegree();
            std::cout << "----Zoznam synov----\n";

            for (int i = 0; i < numberOfSons; ++i) {
                auto son = hierarchy.accessSon(*iterator.getCurrentBlock(), i);
                if (son != nullptr) {
                    std::cout << "[" << i << "] " << son->data.name << "\n";
                }
            }

            int index; 
            std::cout << "Zadajte index syna: ";
            std::cin >> index;

            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid argument!");
            }

            iterator.goDown(index);
            break;
        }
        case 3: {
            std::cout << "\033[2J\033[1;1H";

            Algorithm alg;
            Comparator comp;

            int predChoice; 
            std::cout << "Zvolte predikát: \n.............................. \n";
            std::cout << "1. predStartWithStr \n2. predPartyHasVotes \n3. predTrue \n4. hasType \n5. späťDoMenu\n";
            std::cout << "Voľba: ";
            std::cin >> predChoice;

            if (std::cin.fail()) {
                throw std::invalid_argument("Invalid argument!");
            }

            ImplicitSequence<TerritorialUnit<District>> find_results;
            auto proc_insert_into = procInsertIntoSeq(find_results);

            switch (predChoice) {
                case 1: {
                    std::cout << "\033[2J\033[1;1H";
                    std::string prefix;
                    std::cout << "Zadajte prefix: ";
                    std::cin >> prefix;

                    auto pred_starts_with = predStartsWithStr(prefix);
                    alg.algoritmus(proc_insert_into, pred_starts_with, iterator.begin(), iterator.end());
                    break;
                }
                case 2: {
                    std::cout << "\033[2J\033[1;1H";
                    for (PoliticalParty& p : parties) {
                        std::cout << p.id << " - " << p.party_name << "\n";
                    }

                    int id;
                    std::cout << "Zadajte číslo politickej strany: ";
                    std::cin >> id;

                    if (std::cin.fail() || id < 0 || id >= parties.size()) {
                        throw std::invalid_argument("Invalid argument!");
                    }

                    int predict_votes;
                    std::cout << "Zadajte predpokladaný počet hlasov tejto strany: ";
                    std::cin >> predict_votes;

                    if (std::cin.fail()) {
                        throw std::invalid_argument("Invalid argument!");
                    }

                    auto pred_party_has_votes = predPartyHasVotes(id, predict_votes);
                    alg.algoritmus(proc_insert_into, pred_party_has_votes, iterator.begin(), iterator.end());
                    break;
                }
                case 3: {
                    std::cout << "\033[2J\033[1;1H";
                    auto pred_true = predTrue();
                    alg.algoritmus(proc_insert_into, pred_true, iterator.begin(), iterator.end());
                    break;
                }
                case 4: {
                    std::cout << "\033[2J\033[1;1H";
                    std::string type;
                    std::cout << "Zadajte typ územnej jednotky (čr/kraj/okres/obec): ";
                    std::cin >> type;

                    auto pred_has_type = predHasType(type);
                    alg.algoritmus(proc_insert_into, pred_has_type, iterator.begin(), iterator.end());
                    break;
                }

                case 5: {
                    break;
                }

                default:
                    break;
            }

            if (find_results.size() > 0) {
                comp.startSort(find_results, parties);

                std::cout << "\n-Utriedená štruktúra: \n";
                auto proc_print = procPrint();
                auto pred_true = predTrue();

                alg.algoritmus(proc_print, pred_true, find_results.begin(), find_results.end());
            } else {
                std::cout << "Žiadne výsledky nevyhovovali filtru. Netriedim.\n";
            }
            break;
        }
        case 4: {
            std::cout << "\033[2J\033[1;1H";

            int sum = 0;
            for (size_t i = 0; i < iterator.getCurrentData().total_votes.size(); ++i) {
                sum += iterator.getCurrentData().total_votes[i];
            }

            std::cout << iterator.getCurrentData().name << "\n";
            std::cout << "typ územnej jednotky: " << enumToString(iterator.getCurrentData().type) << "\n";
            std::cout << "celkový počet hlasov: " << sum << "\n";
            std::cout << "počty hlasov jednotlivých strán: " << "\n";
            for (size_t i = 0; i < iterator.getCurrentData().total_votes.size(); ++i) {
                std::cout << parties[i].short_party_name << " - " << iterator.getCurrentData().total_votes[i] << "\n";
            }
            break;
        }
        case 5: {
            std::cout << "\033[2J\033[1;1H";
            break;
        }
        default: {
            throw std::invalid_argument("Invalid argument!");
        }
    }
    return number;
}

/**
 * @brief Hlavné menu programu – výber úrovne funkcionality.
 *
 * Zobrazí menu s výberom úrovne a deleguje riadenie na príslušnú funkciu:
 * - 1 – `program1`: zoznam obcí s filtráciou predikátmi
 * - 2 – `program2`: interaktívne prechádzanie hierarchie
 * - 3 – `program3`: vyhľadávanie podľa názvu v hešovacích tabuľkách
 * - 4 – `program4`: prechádzanie hierarchie s filtráciou a zoradením
 * - 5 – ukončenie programu
 *
 * @param terr_units Vektor územných jednotiek.
 * @param districts Vektor volebných okrskov.
 * @param parties Vektor politických strán.
 * @param hierarchy Referencia na hierarchiu územných jednotiek.
 * @param iterator Referencia na iterátor hierarchie.
 * @param towns Hešovacia tabuľka obcí.
 * @param municipalities Hešovacia tabuľka okresov.
 * @param regions Hešovacia tabuľka krajov.
 * @return Číslo zvolenej možnosti (5 = ukončenie).
 *
 * @throws std::invalid_argument Ak je vstup neplatný.
 */
int program(std::vector<TerritorialUnit<District>>& terr_units, 
    std::vector<District>& districts, 
    std::vector<PoliticalParty>& parties, 
    MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy, 
    HierarchyIterator& iterator, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& towns, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& municipalities, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& regions) {
    int number; 
    std::cout << "Zvolte úroveň: \n.............................. \n";
    std::cout << "1. úroveň1 \n2. úroveň2 \n3. úroveň3 \n4. úroveň4 \n5. koniecProgramu \n";
    std::cout << "Voľba: ";
    std::cin >> number;

    if (std::cin.fail()) {
        throw std::invalid_argument("Invalid argument!");
    }

    switch(number) {
        case 1: {
            std::cout << "\033[2J\033[1;1H";

            int choice = 0;
            while (choice != 5) {
                choice = program1(terr_units.begin(), terr_units.end(), districts, parties, true);
                std::cout << "\n";
            }

            break;
        }
        case 2: {
            std::cout << "\033[2J\033[1;1H";

            int choice = 0;
            while (choice != 5) {
                choice = program2(terr_units, districts, parties, hierarchy, iterator);
                std::cout << "\n";
            }
            break;
        }
        case 3: {
            program3(towns, municipalities, regions, parties);
            break;
        }

        case 4: {
            std::cout << "\033[2J\033[1;1H";
            
            int choice = 0;
            while (choice != 5) {
                choice = program4(terr_units, districts, parties, hierarchy, iterator);
            }

            break;
        }

        case 5: {
            std::cout << "\033[2J\033[1;1H";
            break;
        }
        default: {
            throw std::invalid_argument("Invalid argument!");
        }
    }
    return number;
}

/**
 * @brief Vstupný bod programu.
 *
 * Načíta CSV súbory zadané ako argumenty príkazového riadku, zostaví
 * dátové štruktúry (vektor územných jednotiek, hierarchiu, hashovacie tabuľky)
 * a spustí hlavnú slučku programu.
 *
 * Očakávané argumenty (v tomto poradí):
 * 1. `pscoco.csv`   – zoznam obcí
 * 2. `pst4p.csv`    – výsledky hlasovania po okrskoch
 * 3. `strany.csv`   – zoznam politických strán
 * 4. `cnumnuts.csv` – číselník NUTS kódov (kraje a okresy)
 *
 * Postup inicializácie:
 * 1. Načítanie obcí z `pscoco.csv`
 * 2. Načítanie strán z `strany.csv`
 * 3. Načítanie okrskov z `pst4p.csv` a zostavenie `District` objektov
 * 4. Priradenie okrskov k obciam
 * 5. Načítanie NUTS kódov z `cnumnuts.csv` a doplnenie do vektora jednotiek
 * 6. Zostavenie stromovej hierarchie
 * 7. Výpočet celkových hlasov pre každý uzol hierarchie
 * 8. Naplnenie hashovacích tabuliek
 * 9. Spustenie hlavnej slučky menu
 *
 * @param argc Počet argumentov príkazového riadku.
 * @param argv Pole argumentov príkazového riadku.
 * @return 0 pri úspešnom ukončení.
 *
 * @throws std::invalid_argument Ak počet alebo názvy argumentov nie sú správne.
 */
int main(int argc, char** argv) {
    if (argc <= 4) {
        throw std::invalid_argument("Invalid arguments!");
    }
 
    std::string towns_file = argv[1];
    std::string pst4p_file = argv[2];
    std::string parties_file = argv[3];
    std::string code_list_file = argv[4];
 
    if (towns_file.find("pscoco") == std::string::npos) {
        throw std::invalid_argument("Argument 1 must be pscoco.csv");
    }
 
    if (pst4p_file.find("pst4p") == std::string::npos) {
        throw std::invalid_argument("Argument 2 must be pst4p.csv");
    }
 
    if (parties_file.find("strany") == std::string::npos) {
        throw std::invalid_argument("Argument 3 must be strany.csv");
    }
 
    if (code_list_file.find("cnumnuts") == std::string::npos) {
        throw std::invalid_argument("Argument 4 must be cnumnuts.csv");
    }
 
    // pscoco.csv
    CSVLoader<TerritorialUnit<District>> loader1(';');
    auto territorial_units = loader1.load(argv[1], 
        [](const std::vector<std::string>& cols) {
            TerritorialUnit<District> t;
            t.name = cols[5];
            t.town_code = std::stoi(cols[4]);
            t.district_code = std::stoi(cols[1]);
            t.region_code = std::stoi(cols[0]);
            t.type = UnitType::OBEC;
            return t;
        }
    );
 
    // strany.csv
    CSVLoader<PoliticalParty> loader3(';');
    auto parties = loader3.load(argv[3],
        [](const std::vector<std::string>& cols) {
            PoliticalParty p;
            p.id = std::stoi(cols[0]);
            p.party_name = cols[1];
            p.short_party_name = cols[2];
            return p;
        }
    );
 
    //pst4p.csv
    HashTable<std::string, District> map;
 
    CSVLoader<int> loader2(';');
    loader2.load(argv[2], 
        [&](const std::vector<std::string>& cols) {
 
        int town_code = std::stoi(cols[5]);
        int district_id = std::stoi(cols[6]);
        int party = std::stoi(cols[8]);
        int number_of_votes = std::stoi(cols[9]);
 
        std::string key = cols[5] + "-" + cols[6];

        District* d_ptr = nullptr;
        if (!map.tryFind(key, d_ptr)) {
            District empty{};
            map.insert(key, empty);
            map.tryFind(key, d_ptr);
        }
        District& d = *d_ptr;
 
        if (d.number_of_votes.empty()) {
            d.town_code = town_code;
            d.district_id = district_id;
            d.town = nullptr;
            d.number_of_votes.assign(parties.size(), 0);
        }
 
        d.number_of_votes[party - 1] = number_of_votes;
 
        return 0;
    });
 
    std::vector<District> districts;
    districts.reserve(map.size());
    for (auto& item : map) {
        districts.push_back(item.data);
    }
 
    HashTable<int, TerritorialUnit<District>*> towns_by_code;
 
    for (TerritorialUnit<District>& t : territorial_units) {
        towns_by_code.insert(t.town_code, &t);
    }
 
    for (District& d : districts) {
        TerritorialUnit<District>** found = nullptr;
        if (towns_by_code.tryFind(d.town_code, found)) {
            d.town = *found;
        }
    }
 
    for (District& d : districts) {
        if (d.town != nullptr) {
            d.town->election_results.push_back(d);
        }
    }
 
    // cnumnuts.csv
    CSVLoader<TerritorialUnit<District>> loader4(';');
    auto territorial_division = loader4.load(argv[4],
        [](const std::vector<std::string>& cols) {
            TerritorialUnit<District> n;
            n.name   = cols[2];
            n.numnuts = cols[0];
            n.nuts   = cols[1];
 
            switch(n.nuts.size()) {
                case 4:
                    n.type = UnitType::CR;
                    break;
                case 7:
                    n.type = UnitType::KRAJ;
                    break;
                case 8:
                    n.type = UnitType::OKRES;
                    break;
                default:
                    n.type = UnitType::NEZNAME;
            }
 
            return n;
        }
    );
 
    for (auto& unit : territorial_division) {
        territorial_units.push_back(unit);
    }
 
    MultiWayExplicitHierarchy<TerritorialUnit<District>> hierarchy;
    HierarchyManager hierarchy_manager;
    hierarchy_manager.loadHierarchy(territorial_units, hierarchy);
 
    hierarchy_manager.sumTotalVotes(hierarchy.accessRoot(), hierarchy, parties.size());
 
    HierarchyIterator iterator(hierarchy);

    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>> tableTowns;              //obec
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>> tableMunicipalities;     //okres
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>> tableRegions;            //kraj

    TableManager tableManager;
    tableManager.fillTables(&hierarchy, hierarchy.accessRoot(), tableTowns, tableMunicipalities, tableRegions);
 
    int choice = 0;
    while (choice != 5) {
        choice = program(territorial_units, districts, parties, hierarchy, iterator, tableTowns, tableMunicipalities, tableRegions);
        std::cout << "\n";
    }

    return 0;
}