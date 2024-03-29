#include "store_data.h"
#include <fstream>
#include <iostream>
#include <../utils/search.h>

StoreData::StoreData(const std::string& filename) : m_filename(filename) {
    // Read data from file if it exists
    std::ifstream file(m_filename);
    if (file.good()) {
        file >> m_data;
    }
    file.close();

    // Build semantic search index
    std::vector<std::vector<double>> embeddings;
    std::vector<std::string> labels;
    for (auto& element : m_data.items()) {
        std::string label = element.key();
        auto embedding = element.value()["embedding"].get<std::vector<double>>();
        embeddings.push_back(embedding);
        labels.push_back(label);
    }
    m_search = SemanticSearch(embeddings, labels);
}

bool StoreData::insertData(const std::string& label, const std::vector<double>& embedding) {
    // Check if label already exists
    if (m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " already exists.\n";
        return false;
    }

    // Check if embedding size is consistent with the existing data embeddings
    if (!m_data.empty()) {
        size_t embedding_size = m_data.begin().value()["embedding"].size();
        if (embedding_size != embedding.size()) {
            std::cerr << "Error: Embedding size " << embedding.size() << " is inconsistent with existing data embeddings of size " << embedding_size << ".\n";
            return false;
        }
    }

    // Add label and embedding to data
    json data;
    data["embedding"] = embedding;
    m_data[label] = data;

    m_search.add(label, embedding);

    // Save data to file
    return saveData();
}

bool StoreData::deleteData(const std::string& label) {
    // Check if label exists
    if (!m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " does not exist.\n";
        return false;
    }

    // Delete label from data
    m_data.erase(label);

    m_search.remove(label);

    // Save data to file
    return saveData();
}

bool StoreData::updateData(const std::string& label, const std::vector<double>& embedding) {
    // Check if label exists
    if (!m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " does not exist.\n";
        return false;
    }

    // Check if embedding size is consistent with the existing data embeddings
    if (!m_data.empty()) {
        size_t embedding_size = m_data.begin().value()["embedding"].size();
        if (embedding_size != embedding.size()) {
            std::cerr << "Error: Embedding size " << embedding.size() << " is inconsistent with existing data embeddings of size " << embedding_size << ".\n";
            return false;
        }
    }

    // Update embedding
    m_data[label]["embedding"] = embedding;

    m_search.remove(label);
    m_search.add(label, embedding);

    // Save data to file
    return saveData();
}

std::vector<double> StoreData::readData(const std::string& label) const {
    // Check if label exists
    if (!m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " does not exist.\n";
        return {};
    }

    // Return embedding
    return m_data[label]["embedding"].get<std::vector<double>>();
}

int StoreData::getEmbeddingSize() {
    if (m_data.empty()) {
        return 0;
    }
    return m_data.begin().value()["embedding"].size();
}

bool StoreData::saveData() {
    // Write data to file
    std::ofstream file(m_filename);
    if (!file.good()) {
        std::cerr << "Error: Unable to open file " << m_filename << " for writing.\n";
        return false;
    }
    file << m_data.dump(4);
    file.close();
    return true;
}

std::unordered_map<std::string, std::vector<double>> StoreData::getAllData() const {
    std::unordered_map<std::string, std::vector<double>> allData;
    for (auto& element : m_data.items()) {
        std::string label = element.key();
        std::vector<double> embedding = element.value()["embedding"].get<std::vector<double>>();
        allData[label] = embedding;
    }
    return allData;
}

std::vector<std::string> StoreData::search(const std::vector<double>& query_embedding, double threshold, int top_k) {
    // check if query embedding size is consistent with the existing data embeddings
    if (!m_data.empty()) {
        size_t embedding_size = m_data.begin().value()["embedding"].size();
        if (embedding_size != query_embedding.size()) {
            std::cerr << "Error: Query embedding size " << query_embedding.size() << " is inconsistent with existing data embeddings of size " << embedding_size << ".\n";
            return {};
        }
    }
    return m_search.search(query_embedding, threshold, top_k);
}

bool StoreData::check(const std::string& label, const std::vector<double>& query_embedding, double threshold){
    // check if query embedding size is consistent with the existing data embeddings
    if (!m_data.empty()) {
        size_t embedding_size = m_data.begin().value()["embedding"].size();
        if (embedding_size != query_embedding.size()) {
            std::cerr << "Error: Query embedding size " << query_embedding.size() << " is inconsistent with existing data embeddings of size " << embedding_size << ".\n";
            return {};
        }
    }
    return m_search.check(label, query_embedding, threshold);
}

bool StoreData::makeCluster(int k, int max_iter){
    return m_search.make_cluster(k, max_iter);
}

std::vector<std::string> StoreData::searchCluster(const std::vector<double>& query_embedding, double threshold, int top_k){
    // check if query embedding size is consistent with the existing data embeddings
    if (!m_data.empty()) {
        size_t embedding_size = m_data.begin().value()["embedding"].size();
        if (embedding_size != query_embedding.size()) {
            std::cerr << "Error: Query embedding size " << query_embedding.size() << " is inconsistent with existing data embeddings of size " << embedding_size << ".\n";
            return {};
        }
    }
    return m_search.search_cluster(query_embedding, threshold, top_k);
}