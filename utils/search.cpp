#include "search.h"
#include <algorithm>

SemanticSearch::SemanticSearch(const std::vector<std::vector<double>>& embeddings, const std::vector<std::string>& labels)
    : embeddings_(embeddings), labels_(labels) {}

std::vector<std::string> SemanticSearch::search(const std::vector<double>& query_embedding, double threshold, int top_k) {
    std::vector<std::pair<double, std::string>> results; // pair of similarity score and label
    for (size_t i = 0; i < embeddings_.size(); ++i) {
        double sim = cosine_similarity(query_embedding, embeddings_[i]);
        if (sim >= threshold) {
            results.push_back(std::make_pair(sim, labels_[i]));
        }
    }
    std::sort(results.begin(), results.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.first > rhs.first; // sort in descending order by similarity
    });
    std::vector<std::string> top_results;
    int count = 0;
    for (const auto& result : results) {
        top_results.push_back(result.second);
        ++count;
        if (count >= top_k) {
            break;
        }
    }
    return top_results;
}
