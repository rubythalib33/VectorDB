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

bool StoreData::updateData(const std::string& label, const std::vector<double>& embedding) {
    // Check if label already exists
    if (m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " already exists.\n";
        return false;
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

std::vector<double> StoreData::readData(const std::string& label) const {
    // Check if label exists
    if (!m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " does not exist.\n";
        return {};
    }

    // Return embedding
    return m_data[label]["embedding"].get<std::vector<double>>();
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
    return m_search.search(query_embedding, threshold, top_k);
}
