#include "list_analyzer.hpp"

namespace cmx {


// TODO 01

    VectorAnalyzer::VectorAnalyzer(const std::string& name)
        : ComplexityAnalyzer<std::vector<int>>(name)
    {
        this->registerBeforeOperation(
            [&](const std::vector<int>& v) {
                m_indexMiddle = v.size() / 2;
            });
    }



    void VectorAnalyzer::growToSize(std::vector<int>& structure, size_t size) {
        while (structure.size() < size) {
            structure.push_back(0);
        }
    }

    VectorInsertAnalyzer::VectorInsertAnalyzer(const std::string &name): cmx::VectorAnalyzer(name) {

    }

    void VectorInsertAnalyzer::executeOperation(std::vector<int> &structure) {
        structure.insert(structure.begin() + this->m_indexMiddle, 5);
    }
} // namespace cmx

 