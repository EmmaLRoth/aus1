#ifndef SEMESTRAL_PROJECT_CSV_LOADER_HPP
#define SEMESTRAL_PROJECT_CSV_LOADER_HPP

#include <functional>
#include <libds/mm/system_memory_manager.hpp>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


/**
 * @brief Generická trieda na načítanie CSV súborov.
 *
 * Načíta riadky zo CSV súboru, rozdelí ich podľa zadaného oddeľovača
 * a každý riadok spracuje pomocou užívateľom dodanej parser funkcie.
 * Prvý riadok (hlavička) je automaticky preskočený.
 *
 * @tparam T Typ objektu, na ktorý sa každý riadok CSV parsuje.
 */
template<typename T>
class CSVLoader {
    public:
        /**
         * @brief Konštruktor.
         *
         * @param delimiter Znak používaný ako oddeľovač stĺpcov v CSV súbore (napr. `;`).
         */
        CSVLoader(char delimiter);

        /**
         * @brief Načíta CSV súbor a vráti vektor naparsovaných objektov.
         *
         * Súbor sa hľadá v relatívnej ceste `../semestral_project/data/`.
         * Prvý riadok (hlavička) je preskočený. Každý ďalší riadok je rozdelený
         * podľa oddeľovača a predaný funkcii @p parser, ktorá ho prevedie na objekt typu @p T.
         *
         * @param file_name Názov súboru (bez cesty), napr. `"strany.csv"`.
         * @param parser    Funkcia s podpisom `T(const std::vector<std::string>&)`,
         *                  ktorá z vektora stĺpcov vytvorí objekt typu @p T.
         * @return Vektor naparsovaných objektov typu @p T.
         *
         * @throws std::runtime_error Ak sa súbor nepodarí otvoriť.
         */
        std::vector<T> load(const std::string& file_name, std::function<T(const std::vector<std::string>&)> parser);

    private:
        char delimiter;

};

template<typename T>
CSVLoader<T>::CSVLoader(char delimiter) : delimiter(delimiter) {}

template<typename T>
std::vector<T> CSVLoader<T>::load(const std::string& path_name, 
    std::function<T(const std::vector<std::string>&)> parser) {

    std::string path = std::format("../semestral_project/data/{}", path_name);
    std::ifstream ifst(path);

    if (ifst.fail()) {
        throw std::runtime_error("Failed opening file: " + path_name + "!");
    }
 

    std::vector<T> results;
    std::string line;
    bool first = true;
    while (std::getline(ifst, line)) {
        if (first) {
            first = false;
            continue;
        }

        std::stringstream s(line);
        std::string str;
        std::vector<std::string> cols;

        while (std::getline(s, str, this->delimiter)) {
            cols.push_back(str);
        }

        results.push_back(parser(cols));
    }
    return results;
}

#endif