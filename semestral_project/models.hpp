#ifndef SEMESTRAL_PROJECT_MODELS_HPP
#define SEMESTRAL_PROJECT_MODELS_HPP

#include <string>
#include <vector>


/**
 * @brief Typ územnej jednotky.
 *
 * Reprezentuje administratívnu úroveň územnej jednotky
 * v rámci hierarchie (štát → kraj → okres → obec).
 */
enum UnitType {
        CR,                     ///Česká republika
        KRAJ,                   ///Kraj
        OKRES,                  ///Okres
        OBEC,                   ///Obec
        NEZNAME,                ///Neznámy typ
};

/**
 * @brief Konvertuje hodnotu enumu UnitType na reťazec.
 *
 * @param type Typ územnej jednotky.
 * @return Textová reprezentácia typu (napr. "kraj", "obec").
 *         Ak typ nie je rozpoznaný, vráti "nezname".
 */
inline std::string enumToString(UnitType type) {
        switch (type) {
                case UnitType::CR:
                        return "čr";

                case UnitType::KRAJ:
                        return "kraj";

                case UnitType::OKRES:
                        return "okres";

                case UnitType::OBEC:
                        return "obec";
                case UnitType::NEZNAME:
                  return "nezname";
                }
                
        return "nezname";
}

/**
 * @brief Reprezentuje územnú jednotku (obec, okres, kraj alebo ČR).
 *
 * Šablónový struct, kde parameter @p T určuje typ volebného okrsku
 * (napr. District). Dáta sú načítané zo súborov pscoco.csv a cnumnuts.csv.
 *
 * @tparam T Typ záznamu volebného okrsku.
 */
template<typename T>
struct TerritorialUnit {                        //pscoco.csv + cnumnuts.csv
        std::string name;                       ///NAZEVOBCE alebo NAZEVNUTS
        int town_code;                          ///OBEC
        int district_code;                      ///OKRES
        int region_code;                        ///KRAJ
        std::vector<T> election_results;        ///Zoznam volebných okrskov patriacich tejto jednotke
                                            
        std::string numnuts;                    ///NUMNUTS
        std::string nuts;                       ///NUTS

        std::vector<int> total_votes;           ///výskedný súčet hlasov

        UnitType type;                          ///Typ územnej jednotky   
};

/**
 * @brief Reprezentuje volebný okrsok.
 *
 * Obsahuje výsledky hlasovania pre konkrétny okrsok v rámci obce.
 * Dáta sú načítané zo súboru pst4p.csv.
 */
struct District {                               //pst4p.csv
        int district_id;                        ///OKRSEK
        int town_code;                          ///OBEC
        TerritorialUnit<District>* town;        ///ukazovateľ na obec
        std::vector<int> number_of_votes;       ///POC_HLASU

        /**
         * @brief Operátor porovnania na rovnosť.
         *
         * Dva okrsky sú rovnaké, ak majú zhodné ID, kód obce,
         * ukazovateľ na obec aj počty hlasov.
         *
         * @param other Druhý okrsok na porovnanie.
         * @return true ak sú okrsky zhodné, inak false.
         */
        bool operator==(const District& other) const {
                return district_id == other.district_id
                    && town_code == other.town_code
                    && town == other.town
                    && number_of_votes == other.number_of_votes;
        }

        /**
         * @brief Operátor porovnania na nerovnosť.
         *
         * @param other Druhý okrsok na porovnanie.
         * @return true ak sa okrsky líšia, inak false.
         */
        bool operator!=(const District& other) const {
                return !(*this == other);
        }
};

/**
 * @brief Reprezentuje politickú stranu.
 *
 * Obsahuje identifikátor a názvy strany načítané zo súboru strany.csv.
 */
struct PoliticalParty {                         //strany.csv
        int id;                                 ///VYLOS_CIS
        std::string party_name;                 ///NAZEV_STRN
        std::string short_party_name;           ///NAZEV_STRN_SKRT
};

#endif