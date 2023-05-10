#include "similarity.h"
#include <cmath>
#include <stdexcept>
#include <vector>

double cosine_similarity(const std::vector<double>& v1, const std::vector<double>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same dimension");
    }
    double dot_product = 0.0;
    double norm_v1 = 0.0;
    double norm_v2 = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        dot_product += v1[i] * v2[i];
        norm_v1 += v1[i] * v1[i];
        norm_v2 += v2[i] * v2[i];
    }
    return dot_product / (std::sqrt(norm_v1) * std::sqrt(norm_v2));
}

double euclidean_distance(const std::vector<double>& v1, const std::vector<double>& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vectors must be of the same dimension");
    }
    double distance = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return std::sqrt(distance);
}