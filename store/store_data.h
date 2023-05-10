#include <string>
#include <vector>
#include <../include/nlohmann/json.hpp>
#include <../utils/search.h>

using json = nlohmann::json;

class StoreData {
public:
    StoreData(const std::string& filename);
    bool insertData(const std::string& label, const std::vector<double>& embedding);
    bool deleteData(const std::string& label);
    bool updateData(const std::string& label, const std::vector<double>& embedding);
    std::vector<double> readData(const std::string& label) const;
    bool saveData();
    std::unordered_map<std::string, std::vector<double>> getAllData() const;
    std::vector<std::string> search(const std::vector<double>& query_embedding, double threshold = 0.5, int top_k = 10);
    bool check(const std::string& label, const std::vector<double>& query_embedding, double threshold = 0.5);
    bool makeCluster(int k, int max_iter = 100);
    std::vector<std::string> searchCluster(const std::vector<double>& query_embedding, double threshold = 0.5, int top_k = 10);
    int getEmbeddingSize();
private:
    json m_data;
    std::string m_filename;
    SemanticSearch m_search;
};
