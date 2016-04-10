#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H

/// PROJECT
#include <csapex/model/model_fwd.h>
#include <csapex/msg/msg_fwd.h>
#include <csapex/signal/signal_fwd.h>
#include <csapex/command/command_fwd.h>
#include <csapex/utility/uuid.h>
#include <csapex/model/connector_type.h>

namespace csapex
{

class CommandFactory
{
public:
    CommandFactory(GraphFacade *root, const AUUID& graph_id);
    CommandFactory(GraphFacade *root);

public:
    CommandPtr addConnection(const UUID& from, const UUID& to);

    CommandPtr removeAllConnectionsCmd(ConnectablePtr input);
    CommandPtr removeAllConnectionsCmd(Connectable* input);

    CommandPtr removeAllConnectionsCmd(Input* input);
    CommandPtr removeAllConnectionsCmd(Output* output);
    CommandPtr removeAllConnectionsCmd(Slot* slot);
    CommandPtr removeAllConnectionsCmd(Trigger* trigger);

    CommandPtr removeConnectionCmd(Output* output, Connection* connection);
    CommandPtr removeConnectionCmd(Trigger* trigger, Slot* other_side);


    CommandPtr moveConnections(const UUID& from, const UUID& to);
    CommandPtr moveConnections(Connectable* from, Connectable* to);


    CommandPtr deleteConnectionFulcrumCommand(int connection, int fulcrum);
    CommandPtr deleteAllConnectionFulcrumsCommand(int connection);
    CommandPtr deleteAllConnectionFulcrumsCommand(ConnectionPtr connection);
    CommandPtr deleteConnectionByIdCommand(int id);
    CommandPtr clearCommand();


    CommandPtr createVariadicInput(ConnectionTypeConstPtr connection_type, const std::string& label, bool optional);
    CommandPtr createVariadicOutput(ConnectionTypeConstPtr connection_type, const std::string& label);
    CommandPtr createVariadicTrigger(const std::string& label);
    CommandPtr createVariadicSlot(const std::string& label);

    CommandPtr createVariadicPort(ConnectorType port_type, ConnectionTypeConstPtr connection_type);
    CommandPtr createVariadicPort(ConnectorType port_type, ConnectionTypeConstPtr connection_type, const std::string& label, bool optional);

private:
    GraphFacade* getGraphFacade() const;

private:
    GraphFacade* root_;
    AUUID graph_uuid;
};

}

#endif // COMMAND_FACTORY_H

