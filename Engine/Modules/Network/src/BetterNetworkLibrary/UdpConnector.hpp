#include <iostream>
#include <asio.hpp>
#include <thread>
#include <atomic>
#include <chrono>

using asio::ip::udp;

class UdpConnector {
public:
    UdpConnector(asio::io_context& io_context, const std::string& server_ip, int server_port)
        : socket_(io_context, udp::endpoint(udp::v4(), 0)),
          server_endpoint_(asio::ip::make_address(server_ip), server_port),
          io_context_(io_context),
          sequence_number_(0), connected_(false) {}

    void start() {
        sendHandshake();
    }

    void send(const std::string& message) {
        if (!connected_) {
            std::cerr << "Not connected yet!" << std::endl;
            return;
        }
        
        // Simulate a reliable send by waiting for an acknowledgment before sending the next packet
        std::string packet = std::to_string(sequence_number_) + ":" + message;
        sendPacket(packet);
        
        // Wait for acknowledgment
        waitForAck();
    }

private:
    udp::socket socket_;
    udp::endpoint server_endpoint_;
    asio::io_context& io_context_;
    std::atomic<int> sequence_number_;
    std::atomic<bool> connected_;
    std::string last_sent_packet_;

    void sendHandshake() {
        std::string handshake_message = "SYN";
        sendPacket(handshake_message);
        waitForAck();
    }

    void sendPacket(const std::string& packet) {
        last_sent_packet_ = packet;
        socket_.send_to(asio::buffer(packet), server_endpoint_);
    }

    void waitForAck() {
        char buffer[1024];
        udp::endpoint sender_endpoint;
        socket_.async_receive_from(asio::buffer(buffer, 1024), sender_endpoint,
            [this, buffer](std::error_code ec, std::size_t bytes_received) {
                if (!ec) {
                    std::string received_data(buffer, bytes_received);
                    if (received_data == "ACK") {
                        if (!connected_) {
                            connected_ = true;
                        } else {
                            sequence_number_++;
                        }
                    } else {
                        std::cerr << "Unexpected response: " << received_data << std::endl;
                    }
                } else {
                    std::cerr << "Receive failed: " << ec.message() << std::endl;
                }
            }
        );

        // Run one operation on the io_context to process async_receive_from
        io_context_.run_one();
    }
};
