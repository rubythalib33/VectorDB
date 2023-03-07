#include "store/store_data.h"
#include <iostream>

int main() {
    // Initialize store data
    StoreData store("data.json");

    // Add sample data
    std::vector<float> emb1 = {1.0, 2.0, 3.0};
    store.updateData("label1", emb1);

    std::vector<float> emb2 = {4.0, 5.0, 6.0};
    store.updateData("label2", emb2);

    // Get embeddings
    std::vector<float> emb1_copy = store.readData("label1");
    std::vector<float> emb2_copy = store.readData("label2");

    // Print embeddings
    std::cout << "Embedding for label1: ";
    for (float val : emb1_copy) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "Embedding for label2: ";
    for (float val : emb2_copy) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Get all data
    std::unordered_map<std::string, std::vector<float>> allData = store.getAllData();
    for (auto& element : allData) {
        std::string label = element.first;
        std::vector<float> embedding = element.second;
        std::cout << "Label: " << label << ", Embedding: ";
        for (float val : embedding) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}