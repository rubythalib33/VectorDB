#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include "similarity.h"

class SemanticSearch {
public:
    SemanticSearch(const std::vector<std::vector<double>>& embeddings, const std::vector<std::string>& labels);
    std::vector<std::string> search(const std::vector<double>& query_embedding, double threshold = 0.5, int top_k = 5);

private:
    std::vector<std::vector<double>> embeddings_;
    std::vector<std::string> labels_;
};

#endif // SEARCH_H
