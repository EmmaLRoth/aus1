#include "hashtable_analyzer.hpp"

#include <algorithm>

namespace cmx {
    HashTableAnalyzer::HashTableAnalyzer(const std::string& name)
        : ComplexityAnalyzer<ds::adt::HashTable<std::string, int>>(name),
          m_state(std::make_unique<State>())
    {
        this->registerBeforeOperation(
            [&](const ds::adt::HashTable<std::string, int>&) {
                if (m_state->nextPreloadKeyIndex == 0) {
                    m_state->findIndex = 0;
                    return;
                }

                std::random_device rd;
                std::uniform_int_distribution<std::size_t> dist(0, m_state->nextPreloadKeyIndex - 1);
                m_state->findIndex = dist(rd);
            });
    }

    void HashTableAnalyzer::growToSize(ds::adt::HashTable<std::string, int>& structure, size_t size) {
        if (structure.isEmpty()) {
            m_state->nextPreloadKeyIndex = 0;
        }

        if (structure.size() >= size) {
            return;
        }

        const std::size_t insertCount = size - structure.size();
        std::vector<std::size_t> offsets(insertCount);
        for (std::size_t i = 0; i < insertCount; ++i) {
            offsets[i] = i;
        }

        std::random_device rd;
        std::shuffle(offsets.begin(), offsets.end(), rd);

        for (const std::size_t offset : offsets) {
            const std::size_t keyIndex = m_state->nextPreloadKeyIndex + offset;
            const std::string key = "key_" + std::to_string(keyIndex);
            structure.insert(key, static_cast<int>(keyIndex));
        }

        m_state->nextPreloadKeyIndex += insertCount;
    }

    HashTableInsertAnalyzer::HashTableInsertAnalyzer(const std::string& name)
        : HashTableAnalyzer(name)
    {
    }

    void HashTableInsertAnalyzer::executeOperation(ds::adt::HashTable<std::string, int>& structure) {
        std::string key = "new_key_" + std::to_string(structure.size());
        structure.insert(key, static_cast<int>(structure.size()));
    }

    HashTableFindAnalyzer::HashTableFindAnalyzer(const std::string& name)
        : HashTableAnalyzer(name)
    {
    }

    void HashTableFindAnalyzer::executeOperation(ds::adt::HashTable<std::string, int>& structure) {
        const std::string keyToFind = "key_" + std::to_string(m_state->findIndex);
        int* result = nullptr;
        structure.tryFind(keyToFind, result);
    }
}
