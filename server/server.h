#include "../store/store_data.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, StoreData& store) : socket_(std::move(socket)), store_(store) {}

    void start() {
        authenticate();
    }

private:
    void authenticate(){
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string data(boost::asio::buffer_cast<const char*>(buffer_.data()), length);
                    buffer_.consume(length);
                    std::istringstream iss(data);
                    std::string method, username, password;
                    iss >> method >> username >> password;
                    std::string expected_username = std::getenv("USERNAME") != nullptr ? std::getenv("USERNAME") : "root";
                    std::string expected_password = std::getenv("PASSWORD") != nullptr ? std::getenv("PASSWORD") : "root";
                    if (username == expected_username && password == expected_password) {
                        generateToken();
                        doWrite(generated_tokens_.begin()->c_str());
                        doRead();
                    } else {
                        doWrite("ERROR: Authentication failed\n");
                        socket_.close();
                    }
                } else if (ec != boost::asio::error::eof) {
                    std::cerr << "Error: " << ec.message() << std::endl;
                }
            });
    }

    void generateToken() {
        token_ = "token_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        generated_tokens_.insert(token_);
        std::cout << "Generated token: " << token_ << std::endl;
    }

    void doRead() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string data(boost::asio::buffer_cast<const char*>(buffer_.data()), length);
                    buffer_.consume(length);
                    std::istringstream iss(data);
                    std::string method, tokens;
                    iss >> method >> tokens;
                    if (method == "CREATE") {
                        if (generated_tokens_.find(tokens) != generated_tokens_.end()) {
                            generated_tokens_.erase(tokens);
                            std::string label;
                            std::vector<double> embedding;
                            iss >> label;
                            double val;
                            while (iss >> val) {
                                embedding.push_back(val);
                            }
                            store_.insertData(label, embedding);
                            doWrite("OK\n");
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                        
                    } else if (method == "READ") {
                        if (generated_tokens_.find(tokens) != generated_tokens_.end()) {
                            std::string label;
                            iss >> label;
                            std::vector<double> embedding = store_.readData(label);
                            std::ostringstream oss;
                            for (double val : embedding) {
                                oss << val << " ";
                            }
                            oss << "\n";
                            doWrite(oss.str());
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "UPDATE") {
                        if (generated_tokens_.find(tokens) != generated_tokens_.end()) {
                            std::string label;
                            std::vector<double> embedding;
                            iss >> label;
                            double val;
                            while (iss >> val) {
                                embedding.push_back(val);
                            }
                            store_.updateData(label, embedding);
                            doWrite("OK\n");
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "DELETE") {
                        if (generated_tokens_.find(tokens) != generated_tokens_.end()) {
                            std::string label;
                            iss >> label;
                            store_.deleteData(label);
                            doWrite("OK\n");
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "SEARCH") {
                        if (generated_tokens_.find(tokens) != generated_tokens_.end()) {
                            std::vector<double> query;
                            double val;
                            while (iss >> val) {
                                query.push_back(val);
                            }
                            double threshold;
                            int limit;
                            iss >> threshold >> limit;
                            std::vector<std::string> similar = store_.search(query, threshold, limit);
                            std::ostringstream oss;
                            for (std::string label : similar) {
                                oss << label << " ";
                            }
                            oss << "\n";
                            doWrite(oss.str());
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                        std::vector<double> query;
                        double val;
                        while (iss >> val) {
                            query.push_back(val);
                        }
                    } else {
                        doWrite("ERROR: Invalid method\n");
                    }
                } else if (ec != boost::asio::error::eof) {
                    std::cerr << "Error: " << ec.message() << std::endl;
                }
            });
    }

    void doWrite(const std::string& data) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data),
            [this, self](boost::system::error_code ec, std::size_t /* length */) {
                if (ec) {
                    std::cerr << "Error: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    StoreData& store_;
    boost::asio::streambuf buffer_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint, StoreData& store) : acceptor_(io_context, endpoint), store_(store) {
        doAccept();
    }

private:
    void doAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), store_)->start();
                }
                doAccept();
            });
    }

    tcp::acceptor acceptor_;
    StoreData& store_;
};