#include "Rte/Network/NetworkModule.hpp"

#include <asio.hpp>
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
        void setUpServer(unsigned int maxClientNbr) override;
    
    protected:
        // general variables
        Rte::Network::connectionType m_connectionType;
        Rte::Network::connectionProtocol m_connectionProtocol;

        std::optional<tcp::acceptor> m_acceptor;
        std::optional<tcp::socket> m_socket;
        asio::io_context m_io_context;
        std::unique_ptr<asio::steady_timer> m_timer;

        unsigned int m_maxClientNbr;

    public:
        // client only methods
        bool connect_as_client(const std::string& host, const std::string& port) override;
        bool send_as_client(const std::string& data) override;
    
    public:
        // server only methods
        bool start_as_server(const std::string& port) override;
        void receive_as_server() override;
    
    private:
        // utils methods
        bool verify_method_validity(Rte::Network::connectionType allowed_type);
        void handle_client(tcp::socket socket);
        void print_local_ip_addresses();
        

    protected:
        std::shared_ptr<Rte::Ecs> m_ecs;
};
