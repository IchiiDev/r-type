#include "NetworkModuleAsio.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "asio/connect.hpp"

#include <iostream>
#include <memory>
#include <string>

using namespace Rte;

IModule *createModule() {
    return new NetworkModuleAsio(); // NOLINT(cppcoreguidelines-owning-memory)
}

void NetworkModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    std::cout << "NetworkModule::init()\n";
}

void handle_accept(const asio::error_code& error) {
    if (!error) {
        std::cout << "Connection accepted!" << std::endl;
        // Start communication with the client using the `socket`.
    } else {
        std::cout << "Error accepting connection: " << error.message() << std::endl;
    }
}

void NetworkModuleAsio::update() {
    if (m_connectionType == Rte::Network::connectionType::Client) return;

    // Accept a new client connection
    m_socket = tcp::socket(m_io_context);
    m_acceptor.value().async_accept(m_socket.value(), handle_accept);

    m_io_context.poll();
}

bool NetworkModuleAsio::verify_method_validity(Rte::Network::connectionType allowed_type) {
    return (m_connectionType == allowed_type);
}

/*
bool NetworkModuleAsio::Client::Udp::connect(const std::string& host, const std::string& port) {
    try {
        udp::resolver resolver(m_io_context);
        udp::resolver::results_type endpoints = resolver.resolve(host, port);

        this->m_socket = udp::socket(m_io_context, udp::v4());
        asio::connect(m_socket, endpoints.begin());

        std::cout << "Client connected" << std::endl;

        return true;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkModuleAsio::Client::Udp::send(const std::string& data) {
    try {
        asio::write(m_socket, asio::buffer(data, sizeof(std::string)));

        return true;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}
*/

void NetworkModuleAsio::setUpConnection(Rte::Network::connectionType type, Rte::Network::connectionProtocol protocol) {
    m_connectionType = type;
    m_connectionProtocol = protocol;
}

void NetworkModuleAsio::setUpServer(unsigned int maxClientNbr) {
    m_maxClientNbr = maxClientNbr;
}

bool NetworkModuleAsio::connect_as_client(const std::string& host, const std::string& port) {
    try {
        tcp::resolver resolver(m_io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        // Ensure socket is reset before creating a new one
        if (m_socket.has_value()) {
            m_socket->close();
        }

        // Create the socket
        m_socket.emplace(m_io_context);

        // Try to connect to the server
        asio::connect(m_socket.value(), endpoints);

        // Check if the socket is open after attempting connection
        if (m_socket->is_open()) {
            std::cout << "Client successfully connected to " << host << ":" << port << std::endl;
            return true;
        }
        
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
            return false;
       

    } catch (const asio::system_error& e) {
        // Handle ASIO-specific exceptions
        std::cerr << "ASIO connection error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        // Handle general exceptions
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}


bool NetworkModuleAsio::send_as_client(const std::string& data) {
    try {
        asio::write(m_socket.value(), asio::buffer(data, sizeof(std::string)));

        return true;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}


bool NetworkModuleAsio::start_as_server(const std::string& port) {
    try {
        // Resolve the port for the server to listen on
        m_acceptor.emplace(m_io_context, tcp::endpoint(tcp::v4(), std::stoi(port)));

        // Print the server port
        std::cout << "Server listening on port " << port << std::endl;

        // Get and print the local IP addresses
        // print_local_ip_addresses();

        return true;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}
