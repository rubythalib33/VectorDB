#include <string>
#include <fstream>
#include <iostream>

class Auth {
public:
    Auth(std::string username, std::string password) :
        m_username(username), m_password(password)
    {}

    bool checkCredentials(std::string username, std::string password) {
        return (username == m_username && password == m_password);
    }

    static std::string getUsernameFromEnv() {
        std::ifstream file(".env");
        if (!file) {
            std::cerr << "Error opening .env file" << std::endl;
            return "";
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 9) == "USERNAME=") {
                return line.substr(9);
            }
        }
        std::cerr << "Error reading USERNAME from .env file" << std::endl;
        return "";
    }

    static std::string getPasswordFromEnv() {
        std::ifstream file(".env");
        if (!file) {
            std::cerr << "Error opening .env file" << std::endl;
            return "";
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 9) == "PASSWORD=") {
                return line.substr(9);
            }
        }
        std::cerr << "Error reading PASSWORD from .env file" << std::endl;
        return "";
    }

private:
    std::string m_username;
    std::string m_password;
};
