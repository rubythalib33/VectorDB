#include "../store/store_data.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, StoreData& store) : socket_(std::move(socket)), store_(store) {
        generateToken();
        setExpectedUsernamePassword();
    }

    void start() {
        authenticate();
    }

private:
    std::string token_;
    std::string expected_username_;
    std::string expected_password_;
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
                    if (method != "AUTH") {
                        doWrite("ERROR: Invalid method\n");
                        socket_.close();
                        return;
                    } else {
                        if (username == expected_username_ && password == expected_password_) {
                            doWrite(token_);
                            doRead();
                        } else {
                            doWrite("ERROR: Authentication failed\n");
                            socket_.close();
                        }
                    }
                } else if (ec != boost::asio::error::eof) {
                    std::cerr << "Error: " << ec.message() << std::endl;
                }
            });
    }

    void generateToken() {
        token_ = "token_" + std::to_string(std::rand());
    }

    void setExpectedUsernamePassword(){
        expected_username_ = std::getenv("VDB_USERNAME") != nullptr ? std::getenv("VDB_USERNAME") : "root";
        expected_password_ = std::getenv("VDB_PASSWORD") != nullptr ? std::getenv("VDB_PASSWORD") : "root";
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
                        if (token_ == tokens) {
                            std::string label;
                            std::vector<double> embedding;
                            iss >> label;
                            double val;
                            while (iss >> val) {
                                embedding.push_back(val);
                            }
                            bool status = store_.insertData(label, embedding);
                            if (status) {
                                doWrite("OK\n");
                            } else {
                                doWrite("ERROR: Failed to insert data, There's no such a label or embedding size mismatch\n");
                            }
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                        
                    } else if (method == "READ") {
                        if (token_ == tokens) {
                            std::string label;
                            iss >> label;
                            std::vector<double> embedding = store_.readData(label);
                            std::ostringstream oss;
                            for (double val : embedding) {
                                oss << val << " ";
                            }
                            oss << "\n";
                            if (embedding.size() > 0) {
                                doWrite(oss.str());
                            } else {
                                doWrite("ERROR: Failed to read data, There's no such a label\n");
                            }
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "UPDATE") {
                        if (token_ == tokens) {
                            std::string label;
                            std::vector<double> embedding;
                            iss >> label;
                            double val;
                            while (iss >> val) {
                                embedding.push_back(val);
                            }
                            bool status = store_.updateData(label, embedding);
                            if (status) {
                                doWrite("OK\n");
                            } else {
                                doWrite("ERROR: Failed to update data, There's no such a label or embedding size mismatch\n");
                            }
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "DELETE") {
                        if (token_ == tokens) {
                            std::string label;
                            iss >> label;
                            bool status = store_.deleteData(label);
                            if (status) {
                                doWrite("OK\n");
                            } else {
                                doWrite("ERROR: Failed to delete data, There's no such a label\n");
                            }
                        } else {
                            doWrite("ERROR: Invalid token\n");
                        }
                    } else if (method == "SEARCH") {
                        if (token_ == tokens) {
                            std::vector<double> query;
                            double val;
                            int embedding_size = store_.getEmbeddingSize();
                            for (int i = 0; i < embedding_size; i++) {
                                iss >> val;
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
                            if (similar.size() > 0) {
                                doWrite(oss.str());
                            } else {
                                doWrite("There's no similiar data\n");
                            }
                        } else {
                            doWrite("ERROR: Invalid token\n");
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
                } else {
                    doRead(); // Add this line to keep the connection open for further communication
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