#ifndef SEMESTRAL_PROJECT_PREDICATES_HPP
#define SEMESTRAL_PROJECT_PREDICATES_HPP

#include <functional>
#include "models.hpp"

/**
 * @brief Konvertuje reťazec na malé písmená.
 *
 * Každý znak reťazca je prevedený na malé písmeno pomocou `std::tolower`.
 * Používa `unsigned char` cast pre korektné správanie pri znakoch mimo ASCII.
 *
 * @param s Vstupný reťazec.
 * @return Nový reťazec so všetkými znakmi konvertovanými na malé písmená.
 */
inline std::string toLowerString(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }   
    return result;
}

/**
 * @brief Vytvorí predikát, ktorý overuje, či názov územnej jednotky začína zadaným prefixom.
 *
 * Predikát extrahuje podreťazec z názvu (od indexu 1) a porovná ho so zadaným prefixom.
 *
 * @note Podreťazec začína od indexu 1 (nie 0), lebo prvý znak je úvodzovka.
 *
 * @param prefix Hľadaný prefix (porovnanie bez ohľadu na veľkosť písmen).
 * @return Predikát typu `bool(const TerritorialUnit<District>&)`.
 */
inline std::function<bool(const TerritorialUnit<District>&)> predStartsWithStr(const std::string& prefix) {
    return [prefix](const TerritorialUnit<District>& terr_unit) {
        std::string lowerSubstr = toLowerString(terr_unit.name.substr(1, prefix.size()));
        std::string lowerPrefix = toLowerString(prefix);

        if (lowerSubstr == lowerPrefix) {
            return true;
        }

        return false;
    };
}

/**
 * @brief Vytvorí predikát, ktorý overuje, či každý okrsok územnej jednotky
 *        zaznamenal aspoň zadaný počet hlasov pre danú stranu.
 *
 * Predikát prechádza všetky volebné okrsky územnej jednotky a kontroluje,
 * či počet hlasov pre stranu s indexom @p party_id je aspoň @p predict_number_of_votes.
 * Ak je index mimo rozsahu, predikát vráti `false`.
 *
 * @param party_id Id strany.
 * @param predict_number_of_votes Minimálny požadovaný počet hlasov v každom okrsku.
 * @return Predikát typu `bool(const TerritorialUnit<District>&)`.
 */
inline std::function<bool(const TerritorialUnit<District>&)> predPartyHasVotes(int party_id, int predict_number_of_votes) {
    return [party_id, predict_number_of_votes](const TerritorialUnit<District>& terr_unit) {
        for (const District& d : terr_unit.election_results) {
            if (party_id - 1 < 0 || party_id - 1 >=d.number_of_votes.size()) {
                return false;
            }
            
            if (d.number_of_votes[party_id - 1] < predict_number_of_votes) {
                return false;
            }
        }
        return true;
    };
}

/**
 * @brief Vytvorí predikát, ktorý vždy vráti `true`.
 *
 * Slúži ako neutrálny (identity) predikát, napríklad pri filtrovaní
 * bez podmienky alebo ako východisková hodnota.
 *
 * @return Predikát typu `bool(const TerritorialUnit<District>&)`, ktorý vždy vráti `true`.
 */
inline std::function<bool(const TerritorialUnit<District>&)> predTrue() {
    return [](const TerritorialUnit<District>& terr_unit) {
        return true;
    };
}

/**
 * @brief Vytvorí predikát, ktorý overuje, či územná jednotka má zadaný typ.
 *
 * Porovnáva textovú reprezentáciu typu územnej jednotky (pomocou `enumToString`)
 * so zadaným reťazcom @p type.
 *
 * @param type Očakávaný typ územnej jednotky ako reťazec (napr. "obec", "kraj").
 * @return Predikát typu `bool(const TerritorialUnit<District>&)`.
 */
inline std::function<bool(const TerritorialUnit<District>&)> predHasType(std::string type) {
    return [type](const TerritorialUnit<District>& terr_unit) {
        if (type == enumToString(terr_unit.type)) {
            return true;
        }
        return false;
    };
}


#endif