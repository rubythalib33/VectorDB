#include <string>
#include <vector>
#include <../include/nlohmann/json.hpp>

using json = nlohmann::json;

class StoreData {
public:
    StoreData(const std::string& filename);
    bool updateData(const std::string& label, const std::vector<float>& embedding);
    bool deleteData(const std::string& label);
    std::vector<float> readData(const std::string& label) const;
    bool saveData();
    std::unordered_map<std::string, std::vector<float>> getAllData() const;
private:
    json m_data;
    std::string m_filename;
};
