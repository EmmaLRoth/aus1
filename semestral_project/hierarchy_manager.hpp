#ifndef SEMESTRAL_PROJECT_HIERARCHY_MANAGER_HPP
#define SEMESTRAL_PROJECT_HIERARCHY_MANAGER_HPP

#include "libds/adt/table.hpp"
#include "models.hpp"
#include <iostream>
#include <string>
#include <libds/amt/explicit_hierarchy.hpp>

using namespace ds::amt;

/**
 * @brief Operátor porovnania rovnosti pre územné jednotky.
 *
 * Dve územné jednotky sú považované za rovnaké, ak majú zhodný názov,
 * kódy obce/okresu/kraja a NUTS identifikátory.
 *
 * @param a Prvá územná jednotka.
 * @param b Druhá územná jednotka.
 * @return `true` ak sú jednotky zhodné, inak `false`.
 */
inline bool operator==(const TerritorialUnit<District>& a, const TerritorialUnit<District>& b) {
    return a.name == b.name
        && a.town_code == b.town_code
        && a.district_code == b.district_code
        && a.region_code == b.region_code
        && a.numnuts == b.numnuts
        && a.nuts == b.nuts;
}

/**
 * @brief Trieda zodpovedná za správu a zostavenie hierarchie územných jednotiek.
 *
 * Poskytuje metódy na zoradenie dát, načítanie do stromovej hierarchie
 * a rekurzívny výpočet celkových hlasov pre každú jednotku.
 */
class HierarchyManager {
    public:
        /**
         * @brief Vráti NUTS kód rodičovského uzla pre danú územnú jednotku.
         *
         * Rodičovský kód sa určuje podľa dĺžky NUTS kódu uzla:
         * - dĺžka 4 alebo 6 → koreň (prázdny reťazec)
         * - dĺžka 7 → kraj (prvé 2 znaky bez úvodzoviek)
         * - dĺžka 8 → okres (prvých 5 znakov bez úvodzoviek)
         * - dĺžka 0 → obec (kód okresu ako reťazec)
         *
         * @param node Územná jednotka, pre ktorú sa hľadá rodič.
         * @return NUTS kód rodiča, alebo prázdny reťazec ak uzol nemá rodiča.
         */
        std::string getParentNUTS(const TerritorialUnit<District>& node);

         /**
         * @brief Zoradí vektor územných jednotiek podľa dĺžky NUTS kódu.
         *
         * Uzly s kratším NUTS kódom (vyššia úroveň hierarchie) sú zaradené skôr.
         * Uzly s prázdnym NUTS (obce) sú zaradené na koniec.
         *
         * @param hierarchyNodes Referencia na vektor územných jednotiek na zoradenie.
         */
        void sortData(std::vector<TerritorialUnit<District>>& hierarchyNodes);

        /**
         * @brief Zostaví stromovú hierarchiu z vektora územných jednotiek.
         *
         * Najprv zoradí uzly podľa úrovne (od koreňa po listy), potom
         * každý uzol zaradí do hierarchie na správne miesto podľa jeho NUTS kódu.
         * Na vyhľadanie rodičovského uzla používa internú hešovaciu tabuľku.
         *
         * @param hierarchyNodes Vektor územných jednotiek (vstupné dáta).
         * @param hierarchy Referencia na hierarchiu, ktorá sa naplní.
         */
        void loadHierarchy(std::vector<TerritorialUnit<District>>& hierarchyNodes, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy);
        
        /**
         * @brief Rekurzívne vypočíta celkový počet hlasov pre každú stranu v každom uzle.
         *
         * Pre listové uzly (obce) súčet hlasov vypočíta priamo z volebných okrskov.
         * Pre vnútorné uzly (okresy, kraje, ČR) agreguje výsledky zo všetkých synov.
         * Výsledky sa uložia do `total_votes` každého uzla.
         *
         * @param node Aktuálny uzol hierarchie (pri prvom volaní koreň stromu).
         * @param hierarchy Referencia na hierarchiu.
         * @param partyCount Celkový počet politických strán.
         */
        void sumTotalVotes(MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* node, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy, int partyCount);
};

inline std::string HierarchyManager::getParentNUTS(const TerritorialUnit<District>& node) {
    switch(node.nuts.size()) {
        case 4:                                                                 //ČR                            
            return "";

        case 6:
            return "";

        case 7:                                                                //kraj
            return node.nuts.substr(1, 2);

        case 8:                                                                //okres
            return node.nuts.substr(1, 5);

        case 0:                                                                //obce
            return std::to_string(node.district_code);
    }
    return "";
}

inline void HierarchyManager::sortData(std::vector<TerritorialUnit<District>>& hierarchyNodes) {
    std::sort(hierarchyNodes.begin(), hierarchyNodes.end(), 
        [] (TerritorialUnit<District>& a, TerritorialUnit<District>& b) {
            if (a.nuts.size() == 0 && b.nuts.size() != 0) {
                return false;
            }

            if (b.nuts.size() == 0 && a.nuts.size() != 0) {
                return true;
            }

            return a.nuts.size() < b.nuts.size();
        });
}

inline void HierarchyManager::loadHierarchy(std::vector<TerritorialUnit<District>>& hierarchyNodes, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy) {
    if (hierarchyNodes.empty()) {
        std::cerr << "CHYBA: hierarchyNodes je prazdne.\n";
        return;
    }
 
    sortData(hierarchyNodes);
 
    using Block = MultiWayExplicitHierarchy<TerritorialUnit<District>>::BlockType;
 
    ds::adt::HashTable<std::string, Block*> nodes;
 
    auto& root = hierarchy.emplaceRoot();
    root.data = hierarchyNodes[0];
 
    Block* rootPtr = &root;
    nodes.insert(root.data.nuts.substr(1, 2), rootPtr);
 
    if (!root.data.numnuts.empty()) {
        std::string nutsKey = root.data.nuts.substr(1, 2);
        if (root.data.numnuts != nutsKey) {
            nodes.insert(root.data.numnuts, rootPtr);
        }
    }
 
    for (size_t i = 1; i < hierarchyNodes.size(); ++i) {
        if (hierarchyNodes[i].nuts.size() == 6) {
            continue;
        }
 
        std::string parent_nuts = getParentNUTS(hierarchyNodes[i]);
 
        Block** parent_ptr = nullptr;
        if (!parent_nuts.empty() && !nodes.tryFind(parent_nuts, parent_ptr)) {
            std::cerr << "CHYBA: Parent " << parent_nuts << " neexistuje pre " << hierarchyNodes[i].nuts << "\n";
            continue;
        }
 
        //Ak viem nájsť rodiča v tabuľke, použijem ho. Inak použijem koreň.
        Block* parent_node = (!parent_nuts.empty() && parent_ptr != nullptr) ? *parent_ptr : &root;
 
        auto& child = hierarchy.emplaceSon(*parent_node, hierarchy.degree(*parent_node));
        child.data = hierarchyNodes[i];
 
        Block* childPtr = &child;
 
        if (!child.data.nuts.empty()) {
            std::string childNutsKey = hierarchyNodes[i].nuts.substr(1, hierarchyNodes[i].nuts.size() - 2);
            Block** existing = nullptr;
            if (!nodes.tryFind(childNutsKey, existing)) {
                nodes.insert(childNutsKey, childPtr);
            }
        }

        if (!child.data.numnuts.empty()) {
            Block** existing = nullptr;
            if (!nodes.tryFind(child.data.numnuts, existing)) {
                nodes.insert(child.data.numnuts, childPtr);
            }
        }
    }
}

inline void HierarchyManager::sumTotalVotes(MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* node, MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy, int partyCount) {
    if (node == nullptr) {
        return;
    }

    auto& unit = node->data;
    unit.total_votes.assign(partyCount, 0);

    if (hierarchy.degree(*node) == 0) {                                             //list - obec
        for (const auto& district : unit.election_results) {
            for (size_t i = 0; i < partyCount; ++i) { 
                unit.total_votes[i] += district.number_of_votes[i];
            }
        }

    } else {                                                                        //vrchol - okres, kraj, cr
        for (size_t i = 0; i < hierarchy.degree(*node); i++) {
            auto* child = hierarchy.accessSon(*node, i);
            sumTotalVotes(child, hierarchy, partyCount);

            for (size_t j = 0; j < partyCount; ++j) {
                unit.total_votes[j] += child->data.total_votes[j];
            }
        }
    }
}

/**
 * @brief Iterátor pre prehľadávanie stromovej hierarchie územných jednotiek.
 *
 * Umožňuje manuálny pohyb po hierarchii (nahor/nadol) aj sekvenčné
 * prechádzanie podstromu od aktuálneho uzla do hĺbky.
 */
class HierarchyIterator {
    public:
        /**
         * @brief Konštruktor. Nastaví iterátor na koreň hierarchie.
         *
         * @param h Referencia na hierarchiu, ktorú bude iterátor prechádzať.
         */
        HierarchyIterator(MultiWayExplicitHierarchy<TerritorialUnit<District>>& h);

        /**
         * @brief Presunie iterátor na rodičovský uzol.
         *
         * Ak je aktuálny uzol koreň, vypíše hlásenie a zostane na mieste.
         */
        void goUp();

        /**
         * @brief Presunie iterátor na syna s daným indexom.
         *
         * @param sonIndex Index syna aktuálneho uzla.
         *                 Ak je index mimo rozsahu alebo je uzol list, vypíše hlásenie.
         */
        void goDown(int sonIndex);

        /**
         * @brief Vráti iterátor ukazujúci na začiatok podstromu od aktuálneho uzla.
         *
         * @return Kópia iterátora s nastaveným začiatočným bodom prehľadávania.
         */
        HierarchyIterator begin();

        /**
         * @brief Vráti iterátor reprezentujúci koniec prehľadávania.
         *
         * @return Kópia iterátora s `currentBlock == nullptr`.
         */
        HierarchyIterator end();

        /**
         * @brief Posunie iterátor na ďalší uzol.
         *
         * Najprv sa pokúsi ísť do prvého syna. Ak uzol nemá synov,
         * hľadá ďalšieho súrodenca nahor po strome. Ak sa dostane
         * späť na začiatok prehľadávania, nastaví `currentBlock` na `nullptr`.
         */
        void operator++();

        /**
         * @brief Vráti referenciu na dáta aktuálneho uzla.
         *
         * @return Referencia na `TerritorialUnit<District>` aktuálneho uzla.
         */
        TerritorialUnit<District>& operator*();

        /**
         * @brief Porovná dva iterátory podľa aktuálneho bloku.
         *
         * @param other Druhý iterátor.
         * @return `true` ak iterátory neukazujú na ten istý uzol.
         */
        bool operator!=(const HierarchyIterator& other) const;

        /**
         * @brief Vráti ukazovateľ na aktuálny blok hierarchie.
         *
         * @return Ukazovateľ na `MultiWayExplicitHierarchyBlock` aktuálneho uzla.
         */
        MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* getCurrentBlock();

        /**
         * @brief Vráti referenciu na dáta aktuálneho uzla.
         *
         * @return Referencia na `TerritorialUnit<District>` aktuálneho uzla.
         */
        TerritorialUnit<District>& getCurrentData();

        /**
         * @brief Vráti počet synov aktuálneho uzla.
         *
         * @return Počet synov, alebo 0 ak je `currentBlock` `nullptr`.
         */
        int getDegree();

    private:
        MultiWayExplicitHierarchy<TerritorialUnit<District>>& hierarchy;                            ///Referencia na prechádzanú hierarchiu.
        MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* currentBlock;                    ///Aktuálny uzol iterátora.       
        MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* beginningOfSearch;               ///Uzol, od ktorého sa začalo prehľadávanie podstromu.
};

inline HierarchyIterator::HierarchyIterator(MultiWayExplicitHierarchy<TerritorialUnit<District>>& h) : 
    hierarchy(h), 
    currentBlock(h.accessRoot()),
    beginningOfSearch(h.accessRoot()) {
}

inline void HierarchyIterator::goUp() {
    if(currentBlock != nullptr && !hierarchy.isRoot(*currentBlock)) {
        currentBlock = hierarchy.accessParent(*currentBlock);
        std::cout << "Posun na: " << currentBlock->data.name << "\n";
    } else {
        std::cout << "Si v koreni. Nedá sa ísť vyššie." << "\n";
    }
}

inline void HierarchyIterator::goDown(int sonIndex) {
    if (currentBlock != nullptr && !hierarchy.isLeaf(*currentBlock)) {
        if (sonIndex < 0 || static_cast<std::size_t>(sonIndex) >= hierarchy.degree(*currentBlock)) {
            std::cout << "Neplatný index syna" << "\n";
            return;
        }
        currentBlock = hierarchy.accessSon(*currentBlock, sonIndex);
        std::cout << "Posun na: " << currentBlock->data.name << "\n";
    } else {
        std::cout << "Si v liste. Nedá sa ísť nižšie." << "\n";
    }
}

inline HierarchyIterator HierarchyIterator::begin() {
    HierarchyIterator it = *this;
    it.beginningOfSearch = this->currentBlock;
    return it;
}

inline HierarchyIterator HierarchyIterator::end() {
    HierarchyIterator it = *this;
    it.currentBlock = nullptr;
    return it;
}

inline void HierarchyIterator::operator++() {
    if (currentBlock == nullptr) {
        return;
    }

    if (hierarchy.degree(*currentBlock) > 0) {
        currentBlock = hierarchy.accessSon(*currentBlock, 0);

    } else {
        while (currentBlock != beginningOfSearch) {
            auto parent = hierarchy.accessParent(*currentBlock);

            if (parent == nullptr) {
                break;
            }

            int myIndex = -1;
            for (int i = 0; i < hierarchy.degree(*parent); ++i) {
                if (hierarchy.accessSon(*parent, i) == currentBlock) {
                    myIndex = i;
                    break;
                }
            }

            if (myIndex != -1 && (myIndex + 1) < hierarchy.degree(*parent)) {
                currentBlock = hierarchy.accessSon(*parent, myIndex + 1);
                return;
            }

            currentBlock = parent;
        }

        currentBlock = nullptr;
    }
}


inline bool HierarchyIterator::operator!=(const HierarchyIterator& other) const {
    return this->currentBlock != other.currentBlock;
}

inline TerritorialUnit<District>& HierarchyIterator::operator*() {
    return currentBlock->data;
}

inline MultiWayExplicitHierarchyBlock<TerritorialUnit<District>>* HierarchyIterator::getCurrentBlock() {
    return currentBlock;
}

inline TerritorialUnit<District>& HierarchyIterator::getCurrentData() {
    return currentBlock->data;
}

inline int HierarchyIterator::getDegree() {
    if (currentBlock == nullptr) {
        return 0;
    }
    return hierarchy.degree(*currentBlock);
}


#endif