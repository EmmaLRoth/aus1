#include "list_analyzer.hpp"


int main() {

    cmx::VectorInsertAnalyzer via("VectorInsert");
    via.setStepCount(10);
    via.setStepSize(100);
    via.setReplicationCount(30);
    via.setOutputDirectory(".");
    via.analyze();

}

 