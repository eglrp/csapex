/// HEADER
#include <csapex/serialization/message_serializer.h>

/// COMPONENT
#include <csapex/utility/assert.h>
#include <csapex/utility/yaml_node_builder.h>
#include <csapex/msg/message_factory.h>

/// SYSTEM
#include <fstream>

using namespace csapex;

MessageSerializer::MessageSerializer()
{
}

ConnectionType::Ptr MessageSerializer::deserializeMessage(const YAML::Node &node)
{
    MessageSerializer& i = instance();

    std::string type = node["type"].as<std::string>();

    if(i.type_to_converter.empty()) {
        throw DeserializationError("no connection types registered!");
    }

    if(i.type_to_converter.find(type) == i.type_to_converter.end()) {
        throw DeserializationError(std::string("no such type (") + type + ")");
    }

    ConnectionType::Ptr msg = MessageFactory::createMessage(type);
    try {
        i.type_to_converter.at(type).decoder(node["data"], *msg);
    } catch(const YAML::Exception& e) {
        throw DeserializationError(std::string("error while deserializing: ") + e.msg);
    }

    return msg;
}

YAML::Node MessageSerializer::serializeMessage(const ConnectionType &msg)
{
    try {
        MessageSerializer& i = instance();

        std::string type = msg.typeName();

        Converter& converter = i.type_to_converter.at(type);
        Converter::Encoder& encoder = converter.encoder;

        YAML::Node node;
        node["type"] = type;
        node["data"] = encoder(msg);

        return node;

    } catch(const std::out_of_range& e) {
        throw SerializationError(std::string("cannot serialize message of type ")
                                 + msg.descriptiveName() + ", no YAML converter registered for " + msg.typeName());
    }
}


ConnectionType::Ptr MessageSerializer::readYaml(const YAML::Node &node)
{
    ConnectionType::Ptr msg = MessageSerializer::deserializeMessage(node);
    if(!msg) {
        std::string type = node["type"].as<std::string>();
        throw DeserializationError(std::string("message type '") + type + "' unknown");
    }

    return msg;
}

void MessageSerializer::registerMessage(std::string type, Converter converter)
{
    MessageSerializer& i = instance();

    std::map<std::string, Converter>::const_iterator it = i.type_to_converter.find(type);

    if(it != i.type_to_converter.end()) {
        return;
    }

    apex_assert_hard(it == i.type_to_converter.end());

    i.type_to_converter.insert(std::make_pair(type, converter));
}
