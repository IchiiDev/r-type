#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <string>


namespace Rte::Network {
    enum connectionType { Client, Server };
    enum connectionProtocol { UDP, TCP };

    struct InputPackage {
        // id
        unsigned short player_id;

        // general directions
        bool jump;
        bool right;
        bool left;

        // shooting infos
        bool shot;
        float shoot_angle;
    };

    class NetworkModule : public IModule {
    // module methods ---------------------------------------------------------------------
    public:
        void init(const std::shared_ptr<Ecs>& ecs) override = 0;
        void update() override = 0;
    
    // general methods --------------------------------------------------------------------
    public:
        virtual void setUpConnection(connectionType type, connectionProtocol protocol) = 0;
        virtual bool send_data(const std::string& data) = 0;
        virtual bool send_data(const Rte::Network::InputPackage& data) = 0;

    // client only methods ----------------------------------------------------------------
    public:
        virtual bool connect_as_client(const std::string& host,  const unsigned int& port) = 0;
    
    // server only methods ----------------------------------------------------------------
    public:
        virtual void setUpServer(unsigned int maxClientNbr) = 0;
        virtual bool start_as_server(const std::string& port) = 0;
        virtual std::vector<InputPackage> receive_as_server() = 0;
    };
} // namespace Rte::Network


extern "C" EXPORT Rte::IModule* createModule();
