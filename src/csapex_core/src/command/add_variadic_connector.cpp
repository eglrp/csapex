/// HEADER
#include <csapex/command/add_variadic_connector.h>

/// COMPONENT
#include <csapex/command/command_factory.h>
#include <csapex/command/command_serializer.h>
#include <csapex/command/dispatcher.h>
#include <csapex/model/graph_facade_impl.h>
#include <csapex/model/graph/graph_impl.h>
#include <csapex/model/node.h>
#include <csapex/model/node_handle.h>
#include <csapex/model/subgraph_node.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <csapex/serialization/io/std_io.h>
#include <csapex/serialization/io/csapex_io.h>
#include <csapex/signal/event.h>
#include <csapex/signal/slot.h>
#include <csapex/factory/message_factory.h>

using namespace csapex;
using namespace command;

CSAPEX_REGISTER_COMMAND_SERIALIZER(AddVariadicConnector)

AddVariadicConnector::AddVariadicConnector(const AUUID& graph_id, const AUUID& node, const ConnectorType& connector_type, const TokenDataConstPtr& type, const std::string& label)
  : CommandImplementation(graph_id), connector_type(connector_type), token_type(type), label_(label), optional_(false), node_id(node)
{
}

void AddVariadicConnector::setLabel(const std::string& label)
{
    label_ = label;
}

void AddVariadicConnector::setOptional(bool optional)
{
    optional_ = optional;
}

std::string AddVariadicConnector::getDescription() const
{
    return std::string("create forwarding connector with type ") + port_type::name(connector_type);
}

bool AddVariadicConnector::doExecute()
{
    NodeHandle* nh = getRoot()->getLocalGraph()->findNodeHandle(node_id);
    NodePtr node = nh->getNode().lock();

    switch (connector_type) {
        case ConnectorType::INPUT: {
            VariadicInputs* vi = dynamic_cast<VariadicInputs*>(node.get());
            apex_assert_hard(vi);
            Input* input = vi->createVariadicInput(token_type, label_, optional_);
            connector_id = input->getUUID();
        } break;
        case ConnectorType::OUTPUT: {
            VariadicOutputs* vo = dynamic_cast<VariadicOutputs*>(node.get());
            apex_assert_hard(vo);
            Output* output = vo->createVariadicOutput(token_type, label_);
            connector_id = output->getUUID();
        } break;
        case ConnectorType::SLOT_T: {
            VariadicSlots* vs = dynamic_cast<VariadicSlots*>(node.get());
            apex_assert_hard(vs);
            Slot* slot = vs->createVariadicSlot(token_type, label_, [](const TokenPtr&) {});
            connector_id = slot->getUUID();
        } break;
        case ConnectorType::EVENT: {
            VariadicEvents* vt = dynamic_cast<VariadicEvents*>(node.get());
            apex_assert_hard(vt);
            Event* trigger = vt->createVariadicEvent(token_type, label_);
            connector_id = trigger->getUUID();
        } break;
        default:
            throw std::logic_error(std::string("unknown connector type: ") + port_type::name(connector_type));
    }

    map.external = connector_id;

    SubgraphNode* graph = dynamic_cast<SubgraphNode*>(node.get());
    if (graph) {
        switch (connector_type) {
            case ConnectorType::INPUT: {
                auto relay = graph->getRelayForInput(connector_id);
                map.internal = relay->getUUID();
            } break;
            case ConnectorType::OUTPUT: {
                auto relay = graph->getRelayForOutput(connector_id);
                map.internal = relay->getUUID();
            } break;
            case ConnectorType::SLOT_T: {
                auto relay = graph->getRelayForSlot(connector_id);
                map.internal = relay->getUUID();
            } break;
            case ConnectorType::EVENT: {
                auto relay = graph->getRelayForEvent(connector_id);
                map.internal = relay->getUUID();
            } break;
            default:
                throw std::logic_error(std::string("unknown connector type: ") + port_type::name(connector_type));
        }
    }

    return true;
}

bool AddVariadicConnector::doUndo()
{
    NodeHandle* nh = getRoot()->getLocalGraph()->findNodeHandle(node_id);
    NodePtr node = nh->getNode().lock();

    switch (connector_type) {
        case ConnectorType::INPUT: {
            VariadicInputs* vi = dynamic_cast<VariadicInputs*>(node.get());
            apex_assert_hard(vi);
            vi->removeVariadicInputById(connector_id);
        } break;
        case ConnectorType::OUTPUT: {
            VariadicOutputs* vo = dynamic_cast<VariadicOutputs*>(node.get());
            apex_assert_hard(vo);
            vo->removeVariadicOutputById(connector_id);
        } break;
        case ConnectorType::SLOT_T: {
            VariadicSlots* vs = dynamic_cast<VariadicSlots*>(node.get());
            apex_assert_hard(vs);
            vs->removeVariadicSlotById(connector_id);
        } break;
        case ConnectorType::EVENT: {
            VariadicEvents* vt = dynamic_cast<VariadicEvents*>(node.get());
            apex_assert_hard(vt);
            vt->removeVariadicEventById(connector_id);
        } break;
        default:
            throw std::logic_error(std::string("unknown connector type: ") + port_type::name(connector_type));
    }

    return true;
}

bool AddVariadicConnector::doRedo()
{
    return doExecute();
}

RelayMapping AddVariadicConnector::getMap() const
{
    return map;
}

void AddVariadicConnector::serialize(SerializationBuffer& data, SemanticVersion& version) const
{
    Command::serialize(data, version);

    data << connector_type;
    data << token_type->typeName();

    data << label_;
    data << optional_;

    data << node_id;
    data << connector_id;
}

void AddVariadicConnector::deserialize(const SerializationBuffer& data, const SemanticVersion& version)
{
    Command::deserialize(data, version);

    data >> connector_type;

    std::string type_name;
    data >> type_name;

    token_type = MessageFactory::createMessage(type_name);

    data >> label_;
    data >> optional_;

    data >> node_id;
    data >> connector_id;
}
