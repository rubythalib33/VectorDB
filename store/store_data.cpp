#include "store_data.h"
#include <fstream>
#include <iostream>

StoreData::StoreData(const std::string& filename) : m_filename(filename) {
    // Read data from file if it exists
    std::ifstream file(m_filename);
    if (file.good()) {
        file >> m_data;
    }
    file.close();
}

bool StoreData::updateData(const std::string& label, const std::vector<float>& embedding) {
    // Check if label already exists
    if (m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " already exists.\n";
        return false;
    }

    // Add label and embedding to data
    json data;
    data["embedding"] = embedding;
    m_data[label] = data;

    // Save data to file
    return saveData();
}

std::vector<float> StoreData::readData(const std::string& label) const {
    // Check if label exists
    if (!m_data.contains(label)) {
        std::cerr << "Error: Label " << label << " does not exist.\n";
        return {};
    }

    // Return embedding
    return m_data[label]["embedding"].get<std::vector<float>>();
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

std::unordered_map<std::string, std::vector<float>> StoreData::getAllData() const {
    std::unordered_map<std::string, std::vector<float>> allData;
    for (auto& element : m_data.items()) {
        std::string label = element.key();
        std::vector<float> embedding = element.value()["embedding"].get<std::vector<float>>();
        allData[label] = embedding;
    }
    return allData;
}
