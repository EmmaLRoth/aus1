#ifndef SEMESTRAL_PROJECT_ALGORITHM_HPP
#define SEMESTRAL_PROJECT_ALGORITHM_HPP

/**
 * @class Algorithm
 * @brief Zapuzdruje všeobecné algoritmické operácie pre prácu s kolekciami dát.
 */
class Algorithm {
    public:
        /**
        * @brief Prechádza rozsah iterátorov a aplikuje procesor na každý prvok spĺňajúci predikát.
        *
        * Statická inline metóda, ktorá prechádza prvkami v rozsahu [begin, end). 
        * Najprv vyhodnotí podmienku špecifikovanú v @p predicate a ak je splnená (vráti `true`), 
        * odovzdá prvok na spracovanie objektu @p processor.
        *
        * @tparam Processor Typ volateľného objektu (napr. lambda výraz, funkcia alebo funktor).
        * @tparam Predicate Typ volateľného objektu vracajúceho boolovskú hodnotu.
        * @tparam Iterator Typ iterátora podporujúci operácie inkrementácie (++) a dereferencie (*).
        *
        * @param processor Funkcia alebo objekt aplikovaný na prvky, ktoré úspešne prejdú filtrom.
        * @param predicate Filtračná funkcia určujúca, či sa má prvok spracovať.
        * @param begin Iterátor ukazujúci na začiatok spracovávaného rozsahu (vrátane).
        * @param end Iterátor ukazujúci za koniec spracovávaného rozsahu (vylúčene).
        */
        template<typename Processor, typename Predicate, typename Iterator>
        static inline void algoritmus(Processor processor, Predicate predicate, const Iterator begin, const Iterator end) {
            for (auto i = begin; i != end; ++i) {
                if (predicate(*i)) {
                    processor(*i);
                }
            }
        }
};

#endif // SEMESTRAL_PROJECT_ALGORITHM_HPP