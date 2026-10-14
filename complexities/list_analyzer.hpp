#ifndef COMPLEXITIES_LIST_ANALYZER_HPP
#define COMPLEXITIES_LIST_ANALYZER_HPP

#include "complexity_analyzer.hpp"

#include <random>
#include <vector>

namespace cmx {

class VectorAnalyzer : public ComplexityAnalyzer<std::vector<int>> {
protected:
    VectorAnalyzer(const std::string& name);
    void growToSize(std::vector<int>& structure, size_t size) override;
protected:
    size_t m_indexMiddle;

};

class VectorInsertAnalyzer: public VectorAnalyzer {
    public:
    VectorInsertAnalyzer(const std::string& name);
    protected:
    void executeOperation(std::vector<int>& structure) override;
};
} // namespace cmx

#endif
 