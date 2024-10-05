#include "Rte/Network/NetworkModule.hpp"
#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <memory>
#include <optional>
#include <string>

using asio::ip::tcp;
using asio::ip::udp;

class NetworkModuleAsio : public Rte::Network::NetworkModule {
    public:
        void init(const std::shared_ptr<Rte::Ecs>& ecs) override;
        void update() override;

    public:
        // general methods
        void setUpConnection(Rte::Network::connectionType type, Rte::Network::connectionProtocol protocol) override;
        bool send_data(const std::string& data) override;
        bool send_data(const Rte::Network::InputPackage& data) override;
    
    protected:
        // general variables
        Rte::Network::connectionType m_connectionType;
        Rte::Network::connectionProtocol m_connectionProtocol;

        std::optional<tcp::acceptor> m_acceptor;
        std::vector<std::shared_ptr<tcp::socket>> m_sockets;
        asio::io_context m_io_context;
        std::unique_ptr<asio::steady_timer> m_timer;

        unsigned int m_maxClientNbr;

    public:
        // client only methods
        bool connect_as_client(const std::string& host, const unsigned int& port) override;
    
    public:
        // server only methods
        void setUpServer(unsigned int maxClientNbr) override;
        bool start_as_server(const std::string& port) override;
        std::vector<Rte::Network::InputPackage> receive_as_server() override;
    
    private:
        // utils methods
        bool verify_method_validity(Rte::Network::connectionType allowed_type);
        void handle_client(tcp::socket socket);
        void print_local_ip_addresses();
        void handle_accept(const asio::error_code& error, std::shared_ptr<tcp::socket> socket);
        

    protected:
        std::shared_ptr<Rte::Ecs> m_ecs;
};
