#include "store/store_data.h"
#include <iostream>

int main() {
    // Initialize store data
    StoreData store("data/data.json");

    // Add sample data
    std::vector<double> emb1 = {1.0, 2.0, 3.0};
    store.insertData("label1", emb1);

    std::vector<double> emb2 = {4.0, 5.0, 6.0};
    store.insertData("label2", emb2);

    // Get embeddings
    std::vector<double> emb1_copy = store.readData("label1");
    std::vector<double> emb2_copy = store.readData("label2");

    // Print embeddings
    std::cout << "Embedding for label1: ";
    for (double val : emb1_copy) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "Embedding for label2: ";
    for (double val : emb2_copy) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Get all data
    std::unordered_map<std::string, std::vector<double>> allData = store.getAllData();
    for (auto& element : allData) {
        std::string label = element.first;
        std::vector<double> embedding = element.second;
        std::cout << "Label: " << label << ", Embedding: ";
        for (double val : embedding) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // Search for similar embeddings
    std::vector<std::string> similar = store.search(std::vector<double>{1.0, 2.0, 3.0}, 0.5, 1);
    for (std::string label : similar) {
        std::cout << "Similar label: " << label << std::endl;
    }

    return 0;
}