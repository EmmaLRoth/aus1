#ifndef COMPLEXITIES_HASH_TABLE_ANALYZER_HPP
#define COMPLEXITIES_HASH_TABLE_ANALYZER_HPP

#include "complexity_analyzer.hpp"

#include <memory>
#include <string>

#include "../libds/adt/table.hpp"

namespace cmx {

    /**
     * @class HashTableAnalyzer
     * @brief Abstranktná bazová trieda pre analyzátory operácií nad hešovacou tabuľkou.
     * * Táto trieda dedí od ComplexityAnalyzer a poskytuje spoločný stav a logiku predbežného
     * napĺňania (preload) hašovacej tabuľky dátami pred spustením samotných meraní.
     */
    class HashTableAnalyzer : public ComplexityAnalyzer<ds::adt::HashTable<std::string, int>> {
        protected:
            /**
            * @struct State
            * @brief Vnútorná štruktúra na uchovanie stavu generátora kľúčov medzi operáciami.
            */
            struct State {
                std::size_t nextPreloadKeyIndex{0};         ///Index, ktorý sa použije pre nasledujúci vygenerovaný kľúč pri preloade.
                std::size_t findIndex{0};                   ///Index kľúča, ktorý bol náhodne vybraný na vyhľadávanie.
            };

            /**
            * @brief Konštruktor pre HashTableAnalyzer.
            * @param name Názov analyzátora (používaný pri výstupoch meraní).
            */
            HashTableAnalyzer(const std::string& name);

            /**
            * @brief Zabezpečí, aby hešovacia tabuľka dosiahla požadovanú veľkosť pred vykonaním operácie.
            * * Metóda naplní tabuľku náhodne poprehadzovanými unikátnymi kľúčmi vo formáte "key_X" 
            * až do požadovanej kapacity.
            * * @param structure Inštancia hešovacej tabuľky, ktorá sa má naplniť.
            * @param size Požadovaná cieľová veľkosť (počet prvkov) tabuľky.
            */
            void growToSize(ds::adt::HashTable<std::string, int>& structure, size_t size) override;

        protected:
            std::unique_ptr<State> m_state;                 ///Inteligentný ukazovateľ na vnútorný stav analyzátora.

    };

    /**
     * @class HashTableInsertAnalyzer
     * @brief Analyzátor zameraný na meranie efektívnosti vkladania (insert) do hešovacej tabuľky.
     */
    class HashTableInsertAnalyzer: public HashTableAnalyzer {
        public:

            /**
            * @brief Konštruktor pre HashTableInsertAnalyzer.
            * @param name Názov analyzátora.
            */
            HashTableInsertAnalyzer(const std::string& name);
        protected:

            /**
            * @brief Vykoná operáciu vloženia nového prvku do hešovacej tabuľky.
            * * Vloží unikátny kľúč vo formáte "new_key_X", kde X je aktuálna veľkosť tabuľky.
            * * @param structure Inštancia hešovacej tabuľky, nad ktorou sa operácia vykonáva.
            */
            void executeOperation(ds::adt::HashTable<std::string, int>& structure) override;
    };

    /**
     * @class HashTableFindAnalyzer
     * @brief Analyzátor zameraný na meranie efektívnosti vyhľadávania (find) v hešovacej tabuľke.
     */
    class HashTableFindAnalyzer: public HashTableAnalyzer {
        public:
            /**
            * @brief Konštruktor pre HashTableFindAnalyzer.
            * @param name Názov analyzátora.
            */
            HashTableFindAnalyzer(const std::string& name);

        protected:
            /**
            * @brief Vykoná operáciu vyhľadania náhodného kľúča v hešovacej tabuľke.
            * * Vyhľadáva kľúč podľa indexu `m_state->findIndex`, ktorý bol náhodne zvolený 
            * v callbacku pred spustením tejto operácie.
            * * @param structure Inštancia hešovacej tabuľky, v ktorej sa vyhľadáva.
            */
            void executeOperation(ds::adt::HashTable<std::string, int>& structure) override;
    };
} // namespace cmx

#endif
