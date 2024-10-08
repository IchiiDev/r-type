#include "NetworkModuleAsio.hpp"
#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"
#include "BetterNetworkLibrary/NetworkClient.hpp"
#include "BetterNetworkLibrary/NetworkMessage.hpp"
#include "BetterNetworkLibrary/NetworkServer.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "asio/buffer.hpp"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>
#include <chrono>

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

void NetworkModuleAsio::handle_accept(const asio::error_code& error, std::shared_ptr<tcp::socket> socket) {
    if (!error) {
        std::cout << "New client connected!" << std::endl;

        // Add the newly connected socket to the list of active sockets
        m_sockets.push_back(socket);

        // Optionally, you can start async reads/writes on this socket for communication
    } else {
        std::cerr << "Error accepting connection: " << error.message() << std::endl;
    }

    // After handling the accepted connection, immediately set up to accept the next one
    update();
}

void NetworkModuleAsio::update() {
    asio::error_code ec;

    if (m_connectionType == Rte::Network::connectionType::Client) return;

    // Create a new socket for an incoming connection and store it in the vector
    auto new_socket = std::make_shared<tcp::socket>(m_io_context);

    // Start asynchronous accept for the new connection
    m_acceptor.value().async_accept(*new_socket, [this, new_socket](const asio::error_code& error) {
        handle_accept(error, new_socket);
    });

    // Poll the I/O context to process pending events
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

std::vector<char> vBuffer(1 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket) {
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()), 
        [&](std::error_code ec, std::size_t length)
        {
            if (!ec) {
                std::cout << "\n\nRead " << length << " bytes\n\n";

                for (int i = 0; i < length; i++)
                    std::cout << vBuffer[i];

                GrabSomeData(socket);
            }
        }
    );
}

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

class CustomClient : public bnl::net::IClient<CustomMsgTypes> {
    public:
        void pingServer() {
            bnl::net::message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::ServerPing;

            // cette merde est platerforme dependante
            // ne pas utiliser des trucs plaform dependant dans du code de profuction
            std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

            msg << timeNow;
            send(msg);
        }
};

class CustomServer : public bnl::net::IServer<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : bnl::net::IServer<CustomMsgTypes>(nPort)
	{

	}

protected:
	 bool onClientConnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
		return true;
	}

	// Called when a client appears to have disconnected
	 void onClientDisconnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
	}

	// Called when a message arrives
	 void onMessageReceived (std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client, bnl::net::message<CustomMsgTypes>& msg) override {
		switch (msg.header.id) {
		    case CustomMsgTypes::ServerPing:
		    {
		    	std::cout << "[" << client->getId() << "]: Server Ping\n";

		    	// Simply bounce message back to client
		    	client->send(msg);
		    }
		    break;

            /*
		    case CustomMsgTypes::MessageAll:
		    {
		    	std::cout << "[" << client->getID() << "]: Message All\n";

		    	// Construct a new message and send it to all clients
		    	bnl::net::message<CustomMsgTypes> msg;
		    	msg.header.id = CustomMsgTypes::ServerMessage;
		    	msg << client->getId();
		    	messageAllClients(msg, client);

		    }
		    break;
            */
		}
	}
};

bool NetworkModuleAsio::connect_as_client(const std::string& host, const unsigned int& port) {
	CustomClient c;
	c.connect("127.0.0.1", 60000);

	bool bQuit = false;
    bool responseNeeded = false;
	while (!bQuit) {

		if (c.isConnected()) {
            if (!responseNeeded) {
                std::string input;
                std::getline(std::cin, input);
                responseNeeded = true;

                if (input == "PING") c.pingServer();
                else {
                    responseNeeded = false;
                    std:: cout << "Invalid Input" << std::endl;
                }
            }

            if (!c.getReiceiveQueue().empty()) {
                auto msg = c.getReiceiveQueue().popFront().msg;
                responseNeeded = false;

                switch (msg.header.id) {
                    case CustomMsgTypes::ServerPing: {
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					    std::chrono::system_clock::time_point timeThen;
					    msg >> timeThen;
					    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                    }
                    case CustomMsgTypes::ServerAccept:
                    case CustomMsgTypes::ServerDeny:
                    case CustomMsgTypes::MessageAll:
                    case CustomMsgTypes::ServerMessage:
                      break;
                    }
            }
        } else {
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}

	return false;
}
    /*
    bnl::net::message<CustomMsgType> msg;

    msg.header.id = CustomMsgType::FireBullet;

    int a = 1;
    bool b = true;
    float c = 3.14159265358979323846F;

    struct {
        float x;
        float y;
    } d[5];

    msg << a << b << c << d;

    a = 99;
    b = false;
    c = 99.0F;

    msg >> d >> c >> b >> a;

    std::cout << "Received data: " << a << " " << b << " " << c << std::endl;

    return true;
    */

    // try {
        /*
        // tcp::resolver resolver(m_io_context);
        asio::error_code ec;

        // Fake work wtf is this fucking library im literraly going insane
        asio::io_context::work idleWork(m_io_context);

        std::thread thrContext = std::thread([&]() { m_io_context.run(); });

        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), port);

        m_sockets.push_back(std::make_shared<asio::ip::tcp::socket>(m_io_context));

        m_sockets[0]->connect(endpoint, ec);

        if (!ec) {
            std::cout << "Connected" << std::endl;
        } else {
            std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
        }

        if (m_sockets[0]->is_open()) {
            GrabSomeData(*m_sockets[0]);

            std::string sRequest = 
                "GET /index.html HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Connection: close\r\n\r\n";

            m_sockets[0]->write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(20000ms);

            m_io_context.stop();
            if (thrContext.joinable()) thrContext.join();
        }

        return true;
        */
        /*
        tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        // Create a new socket for each connection
        auto socket = std::make_shared<tcp::socket>(m_io_context);

        // Try to connect to the server
        asio::connect(*socket, endpoints);

        // Check if the socket is open after attempting connection
        if (socket->is_open()) {
            std::cout << "Client successfully connected to " << host << ":" << port << std::endl;

            // Store the socket in the vector
            m_sockets.push_back(socket);

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
*/

bool NetworkModuleAsio::send_data(const Network::InputPackage& data) {
    bool all_successful = true;

    // Iterate over all sockets in the m_sockets vector
    for (auto it = m_sockets.begin(); it!= m_sockets.end();) {
        try {
            auto& socket = *it;

            if (!socket->is_open()) {
                std::cerr << "Error: One of the sockets is not open." << std::endl;
                all_successful = false;
                continue; // Go to the next socket
            }

            asio::write(*socket, asio::buffer(&data, sizeof(Network::InputPackage)));

        } catch (const asio::system_error& e) {
            if (e.code() == asio::error::broken_pipe) {
                std::cerr << "Error: Broken pipe on one socket, removing it..." << std::endl;
                it = m_sockets.erase(it);  // Remove the socket with the broken pipe
                all_successful = false;
            }
            continue;
        }

        ++it;
    }

    return all_successful;
}

bool NetworkModuleAsio::send_data(const std::string& data) {
    bool all_successful = true;

    // Iterate over all sockets in the m_sockets vector
    for (auto it = m_sockets.begin(); it != m_sockets.end(); /* no increment here */) {
        try {
            auto& socket = *it;

            if (!socket->is_open()) {
                std::cerr << "Error: One of the sockets is not open." << std::endl;
                it = m_sockets.erase(it);  // Remove closed sockets
                all_successful = false;
                continue; // Go to the next socket
            }

            asio::write(*socket, asio::buffer(data));

        } catch (const asio::system_error& e) {
            if (e.code() == asio::error::broken_pipe) {
                std::cerr << "Error: Broken pipe on one socket, removing it..." << std::endl;
                it = m_sockets.erase(it);  // Remove the socket with the broken pipe
                all_successful = false;
            } else {
                std::cerr << "Exception: " << e.what() << std::endl;
                it = m_sockets.erase(it);  // Remove socket with error
                all_successful = false;
            }
            continue;  // Move to the next socket
        }

        ++it; // Move to the next socket
    }

    return all_successful;
}

std::vector<Network::InputPackage> NetworkModuleAsio::receive_as_server() {
    for (auto& socket : m_sockets) {
        if (socket && socket->is_open()) {
            // Create a buffer for receiving data that matches the size of the struct
            auto buffer = std::make_shared<std::array<char, sizeof(Network::InputPackage)>>();

            // Asynchronously receive data on each connected socket
            socket->async_receive(asio::buffer(*buffer),
                [this, buffer, socket](std::error_code ec, std::size_t length) {
                    if (!ec) {
                        if (length == sizeof(Network::InputPackage)) {
                            // Cast the buffer to the struct type
                            Network::InputPackage received_data{};
                            std::memcpy(&received_data, buffer->data(), sizeof(Network::InputPackage));
                            
                            // Use the received struct data
                            std::cout << "Received struct: jump=" << received_data.jump
                                      << ", right=" << received_data.right
                                      << ", shoot angle=" << received_data.shoot_angle << std::endl;

                            // Optionally, start another receive operation for continuous data
                            receive_as_server();
                        } else {
                            std::cerr << "Received data size mismatch." << std::endl;
                        }
                    } else {
                        std::cerr << "Receive error: " << ec.message() << std::endl;
                    }
                });
        }
    }

    return {};
}

bool NetworkModuleAsio::start_as_server(const std::string& port) {
    CustomServer server(60000); 
	server.start();

	while (true) {
		server.update();
	}

	return false;
    /*
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
    */
}
