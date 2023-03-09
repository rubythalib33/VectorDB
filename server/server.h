#include "../store/store_data.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, StoreData& store) : socket_(std::move(socket)), store_(store) {}

    void start() {
        doRead();
    }

private:
    void doRead() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string data(boost::asio::buffer_cast<const char*>(buffer_.data()), length);
                    buffer_.consume(length);
                    std::istringstream iss(data);
                    std::string method;
                    iss >> method;
                    if (method == "CREATE") {
                        std::string label;
                        std::vector<double> embedding;
                        iss >> label;
                        double val;
                        while (iss >> val) {
                            embedding.push_back(val);
                        }
                        store_.insertData(label, embedding);
                        doWrite("OK\n");
                    } else if (method == "READ") {
                        std::string label;
                        iss >> label;
                        std::vector<double> embedding = store_.readData(label);
                        std::ostringstream oss;
                        for (double val : embedding) {
                            oss << val << " ";
                        }
                        oss << "\n";
                        doWrite(oss.str());
                    } else if (method == "UPDATE") {
                        std::string label;
                        std::vector<double> embedding;
                        iss >> label;
                        double val;
                        while (iss >> val) {
                            embedding.push_back(val);
                        }
                        store_.updateData(label, embedding);
                        doWrite("OK\n");
                    } else if (method == "DELETE") {
                        std::string label;
                        iss >> label;
                        store_.deleteData(label);
                        doWrite("OK\n");
                    } else if (method == "SEARCH") {
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