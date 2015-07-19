#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

/// PROJECT
#include <csapex/model/connection_type.h>
#include <csapex/msg/message_traits.h>
#include <csapex/utility/singleton.hpp>
#include <csapex/utility/tmp.hpp>
#include <csapex/utility/type.h>

/// SYSTEM
#include <yaml-cpp/yaml.h>
#include <iostream>

HAS_MEM_FUNC(encode, has_yaml_implementation);

namespace csapex
{
namespace serial {

template <typename Message, typename Selector = void>
struct Serializer
{
static YAML::Node encode(const Message& msg) {
    return YAML::convert<Message>::encode(msg);
}
static bool decode(const YAML::Node& node, Message& msg) {
    return YAML::convert<Message>::decode(node, msg);
}
};


template <typename Message>
YAML::Node encodeMessage(const csapex::ConnectionType& msg) {
    return serial::Serializer<Message>::encode(dynamic_cast<const Message&>(msg));
}
template <typename Message>
bool decodeMessage(const YAML::Node& node, csapex::ConnectionType& msg) {
    return serial::Serializer<Message>::decode(node, dynamic_cast<Message&>(msg));
}

}

class MessageSerializer : public Singleton<MessageSerializer>
{
    friend class Singleton<MessageSerializer>;

public:
    struct Converter
    {
        typedef std::function<YAML::Node(const csapex::ConnectionType&)> Encoder;
        typedef std::function<bool(const YAML::Node&, csapex::ConnectionType&)> Decoder;

        Converter(Encoder encode, Decoder decode)
            : encoder(encode), decoder(decode)
        {}

        Encoder encoder;
        Decoder decoder;
    };

    typedef std::runtime_error SerializationError;
    typedef std::runtime_error DeserializationError;

public:
    static ConnectionType::Ptr deserializeMessage(const YAML::Node &node);
    static YAML::Node serializeMessage(const ConnectionType& msg);

    static ConnectionType::Ptr readYaml(const YAML::Node& node);

public:
    template <template <typename> class Wrapper, typename M>
    static void registerDirectMessage()
    {
        registerDirectMessageImpl<Wrapper, M>();
    }

    template <typename M>
    static void registerMessage() {
        MessageSerializer::instance().registerMessage(connection_types::serializationName<M>(),
                                                   Converter(std::bind(&serial::encodeMessage<M>, std::placeholders::_1),
                                                             std::bind(&serial::decodeMessage<M>, std::placeholders::_1, std::placeholders::_2)));
    }

private:
    template <template <typename> class Wrapper, typename M>
    struct HasYaml {
        typedef typename boost::type_traits::ice_and<
        boost::is_class<M>::value,
        has_yaml_implementation< YAML::convert<M>, YAML::Node(YAML::convert<M>::*)(const M&) >::value
        > type;
    };

    template <template <typename> class Wrapper, typename M>
    static void registerDirectMessageImpl(typename std::enable_if< !HasYaml<Wrapper,M>::type::value >::type* = 0)
    {
        MessageSerializer::instance().registerMessage(connection_types::serializationName< Wrapper<M> >(),
                                                   Converter(std::bind(&serial::encodeMessage<Wrapper<M>>, std::placeholders::_1),
                                                             std::bind(&serial::decodeMessage<Wrapper<M>>, std::placeholders::_1, std::placeholders::_2)));

    }
    template <template <typename> class Wrapper, typename M>
    static void registerDirectMessageImpl(typename std::enable_if< HasYaml<Wrapper,M>::type::value >::type* = 0)
    {
        MessageSerializer::instance().registerMessage(connection_types::serializationName< Wrapper<M> >(),
                                                   Converter(std::bind(&MessageSerializer::encodeDirectMessage<Wrapper, M>, std::placeholders::_1),
                                                             std::bind(&MessageSerializer::decodeDirectMessage<Wrapper, M>, std::placeholders::_1, std::placeholders::_2)));
    }


    template <template <typename> class Wrapper, typename Message>
    static YAML::Node encodeDirectMessage(const csapex::ConnectionType& msg) {
        typedef Wrapper<Message> Implementation;
        const Implementation& impl = dynamic_cast<const Implementation&>(msg);

        return YAML::convert<Message>::encode(impl);
    }
    template <template <typename> class Wrapper, typename Message>
    static bool decodeDirectMessage(const YAML::Node& node, csapex::ConnectionType& msg) {
        typedef Wrapper<Message> Implementation;
        Implementation& impl = dynamic_cast<Implementation&>(msg);

        return YAML::convert<Message>::decode(node, impl);
    }

private:
    MessageSerializer();

    static void registerMessage(std::string type, Converter converter);

private:
    std::map<std::string, Converter> type_to_converter;
};


template <typename T>
struct MessageSerializerRegistered
{
    MessageSerializerRegistered() {
        csapex::MessageSerializer::registerMessage<T>();
    }
};

}

#endif // MESSAGE_SERIALIZER_H

