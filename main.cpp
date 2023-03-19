#include <iostream>
#include <boost/asio.hpp>
#include "server/server.h"

using boost::asio::ip::tcp;

int main() {
    try {
        // Create an io_context object
        boost::asio::io_context io_context;

        // Set up an endpoint to listen on
        tcp::endpoint endpoint(tcp::v4(), 3737);

        // Create a StoreData object
        std::string data_file_path = "data/data.json";
        StoreData store(data_file_path);

        // Create a server object and start accepting connections
        Server server(io_context, endpoint, store);

        // Run the io_context object
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}