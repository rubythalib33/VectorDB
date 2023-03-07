#include "utils/search.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::vector<double>> embeddings = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.5, 0.5, 0.0},
        {0.0, 0.5, 0.5},
        {0.5, 0.0, 0.5},
    };

    std::vector<std::string> labels = {"red", "green", "blue", "yellow", "cyan", "magenta"};

    SemanticSearch search_engine(embeddings, labels);

    std::vector<double> query_embedding = {0.8, 0.1, 0.1};
    double threshold = 0.6;
    int top_k = 2;
    std::vector<std::string> results = search_engine.search(query_embedding, threshold, top_k);

    std::cout << "Results:" << std::endl;
    for (const auto& label : results) {
        std::cout << "- " << label << std::endl;
    }

    return 0;
}
