#include "search.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

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

bool SemanticSearch::add(const std::string& label, const std::vector<double>& embedding) {
    labels_.push_back(label);
    embeddings_.push_back(embedding);
    return true;
}

bool SemanticSearch::remove(const std::string& label) {
    auto it = std::find(labels_.begin(), labels_.end(), label);
    if (it == labels_.end()) {
        return false;
    }
    size_t index = std::distance(labels_.begin(), it);
    labels_.erase(labels_.begin() + index);
    embeddings_.erase(embeddings_.begin() + index);
    return true;
}

bool SemanticSearch::check(const std::string& label, const std::vector<double>& query_embedding, double threshold) {
    auto it = std::find(labels_.begin(), labels_.end(), label);
    if (it == labels_.end()) {
        return false;
    }
    size_t index = std::distance(labels_.begin(), it);
    double sim = cosine_similarity(query_embedding, embeddings_[index]);
    return sim >= threshold;
}

std::vector<double> SemanticSearch::get_embedding(const std::string& label) const {
    auto it = std::find(labels_.begin(), labels_.end(), label);
    if (it == labels_.end()) {
        return std::vector<double>();
    }
    size_t index = std::distance(labels_.begin(), it);
    return embeddings_[index];
}

bool SemanticSearch::make_cluster(int k, int max_iter) {
    if (k > embeddings_.size()) {
        // Fail the process, number of clusters is more than the length of embeddings
        return false;
    }

    // Perform k-means clustering
    // 1. Randomly select k initial centroids
    std::vector<std::vector<double>> centroids(k);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, embeddings_.size() - 1);

    for (int i = 0; i < k; ++i) {
        centroids[i] = embeddings_[dis(gen)];
    }

    bool converged = false;
    int iter = 0;
    while (!converged && iter < max_iter) {
        // 2. Assign each point to the nearest centroid
        std::vector<std::vector<size_t>> clusters(k);
        for (size_t i = 0; i < embeddings_.size(); ++i) {
            double min_distance = std::numeric_limits<double>::max();
            int nearest_centroid = -1;
            for (int j = 0; j < k; ++j) {
                double distance = euclidean_distance(embeddings_[i], centroids[j]);
                if (distance < min_distance) {
                    min_distance = distance;
                    nearest_centroid = j;
                }
            }
            clusters[nearest_centroid].push_back(i);
        }

        // 3. Update centroids
        converged = true;
        for (int i = 0; i < k; ++i) {
            std::vector<double> new_centroid(embeddings_[0].size(), 0);
            for (size_t j = 0; j < clusters[i].size(); ++j) {
                for (size_t d = 0; d < embeddings_[0].size(); ++d) {
                    new_centroid[d] += embeddings_[clusters[i][j]][d];
                }
            }
            for (size_t d = 0; d < embeddings_[0].size(); ++d) {
                new_centroid[d] /= clusters[i].size();
            }

            if (euclidean_distance(centroids[i], new_centroid) > 1e-9) {
                converged = false;
            }
            centroids[i] = new_centroid;
        }

        if (converged){
            // Save the labels and centroids as the parameters of the search.cpp
            cluster_labels_.clear();
            cluster_labels_.resize(k);
            cluster_centroids_.clear();

            for (int i = 0; i < k; ++i) {
                for (size_t j = 0; j < clusters[i].size(); ++j) {
                    cluster_labels_[i].push_back(labels_[clusters[i][j]]);
                }
                cluster_centroids_.push_back(centroids[i]);
            }
        }

        ++iter;
    }

    return true;
}

std::vector<std::string> SemanticSearch::search_cluster(const std::vector<double>& query_embedding, double threshold, int top_k) {
    std::vector<std::pair<double, std::string>> results; // pair of similarity score and label
    std::vector<std::string> labels_to_search;
    double min_distance = std::numeric_limits<double>::max();
    for (size_t i = 0; i < cluster_centroids_.size(); ++i) {
        double distance = euclidean_distance(query_embedding, cluster_centroids_[i]);
        if (distance < min_distance) {
            min_distance = distance;
            labels_to_search = cluster_labels_[i];
        }
    }
    for (size_t i = 0; i < labels_to_search.size(); ++i) {
        bool sim = cosine_similarity(query_embedding, this->get_embedding(labels_to_search[i]));
        if (sim >= threshold) {
            results.push_back(std::make_pair(sim, labels_to_search[i]));
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