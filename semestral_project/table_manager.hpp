#ifndef SEMESTRAL_PROJECT_TABLE_MANAGER_HPP
#define SEMESTRAL_PROJECT_TABLE_MANAGER_HPP

#include <semestral_project/models.hpp>
#include <libds/amt/explicit_hierarchy.hpp>
#include <libds/adt/table.hpp>

using namespace ds::amt;
using namespace ds::adt;

/**
 * @brief Trieda zodpovedná za naplnenie hashovacích tabuliek z hierarchie územných jednotiek.
 *
 * Prechádza stromovú hierarchiu územných jednotiek a zaraďuje každú jednotku
 * do príslušnej hešovacej tabuľky podľa jej typu (kraj, okres, obec).
 */
class TableManager {
    public:

        /**
         * @brief Rekurzívne naplní hešovacie tabuľky z hierarchie územných jednotiek.
         *
         * Prechádza hierarchiu do hĺbky a podľa typu každého uzla
         * vloží ukazovateľ na jeho dáta do zodpovedajúcej tabuľky:
         * - `KRAJ` -> @p tableRegions
         * - `OKRES` -> @p tableMunicipalities
         * - `OBEC` -> @p tableTowns
         *
         * Kľúčom v tabuľke je názov jednotky bez úvodných a koncových úvodzoviek.
         * Ak záznam pre daný kľúč už existuje, ukazovateľ sa pridá do existujúcej sekvencie.
         * Ak nie, vytvorí sa nová sekvencia s týmto ukazovateľom.
         *
         * @param hierarchy Ukazovateľ na hierarchiu územných jednotiek.
         * @param node Aktuálny uzol hierarchie (pri prvom volaní koreň stromu).
         * @param tableTowns Tabuľka obcí indexovaná názvom.
         * @param tableMunicipalities Tabuľka okresov indexovaná názvom.
         * @param tableRegions Tabuľka krajov indexovaná názvom.
         */
        void fillTables(MultiWayExplicitHierarchy<TerritorialUnit<District>>* hierarchy, 
            MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* node, 
            HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableTowns, 
            HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableMunicipalities, 
            HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableRegions);
};

inline void TableManager::fillTables(MultiWayExplicitHierarchy<TerritorialUnit<District>>* hierarchy, 
    MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* node, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableTowns, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableMunicipalities, 
    HashTable<std::string, ImplicitSequence<TerritorialUnit<District>*>>& tableRegions) {
        
   if (node == nullptr) {
        return;
    }

    std::string nodeName = node->data.name.substr(1, node->data.name.size() - 2);

    ImplicitSequence<TerritorialUnit<District>*>* existing = nullptr;

    if (node->data.type == UnitType::KRAJ) {
        if (tableRegions.tryFind(nodeName, existing)) { 
            auto& block = existing->emplaceLast();
            block.data = &node->data;
        } else {
            ImplicitSequence<TerritorialUnit<District>*> v;
            auto& block = v.emplaceLast();
            block.data = &node->data;
            tableRegions.insert(nodeName, v);
        }

    } else if (node->data.type == UnitType::OKRES) {
        if (tableMunicipalities.tryFind(nodeName, existing)) { 
            auto& block = existing->emplaceLast();
            block.data = &node->data;
        } else {
            ImplicitSequence<TerritorialUnit<District>*> v;
            auto& block = v.emplaceLast();         
            block.data = &node->data;
            tableMunicipalities.insert(nodeName, v);
        }

    } else if (node->data.type == UnitType::OBEC) {
        if (tableTowns.tryFind(nodeName, existing)) { 
            auto& block = existing->emplaceLast();  
            block.data = &node->data;
        } else {
            ImplicitSequence<TerritorialUnit<District>*> v;
            auto& block = v.emplaceLast();         
            block.data = &node->data;
            tableTowns.insert(nodeName, v);
        }
    }

    for (size_t i = 0; i < hierarchy->degree(*node); ++i) {
        fillTables(hierarchy, hierarchy->accessSon(*node, i), tableTowns, tableMunicipalities, tableRegions);
    }
}

/**
 * @brief Operátor porovnania identity pre implicitné sekvencie ukazovateľov.
 *
 * Dve sekvencie sú považované za rovnaké, ak sú to ten istý objekt v pamäti
 * (porovnanie adries). Tento operátor je potrebný pre kompatibilitu
 * s hešovacou tabuľkou.
 *
 * @param a Prvá sekvencia.
 * @param b Druhá sekvencia.
 * @return `true` ak @p a a @p b sú ten istý objekt, inak `false`.
 */
inline bool operator==(const ImplicitSequence<TerritorialUnit<District>*>& a, const ImplicitSequence<TerritorialUnit<District>*>& b) {
    return &a == &b;
}

#endif