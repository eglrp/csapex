#ifndef MESSAGE_H
#define MESSAGE_H

/// COMPONENT
#include <csapex/model/connection_type.h>
#include <csapex/utility/type.h>

///
/// FOR OPTIONAL ROS SUPPORT
///
namespace ros
{
namespace message_traits
{

template<typename M>
struct IsMessage;

template<typename M>
const char* md5sum();

template<typename M>
const char* datatype();

template<typename M>
const char* definition();

template<typename M>
bool hasHeader();

}
}

///
///
///



namespace csapex {
namespace connection_types {

struct Message : public ConnectionType
{
public:
    typedef boost::shared_ptr<Message> Ptr;

protected:
    Message(const std::string& name);
    virtual ~Message();

public:
    void writeYaml(YAML::Emitter& yaml) const;
    void readYaml(const YAML::Node& node);

public:
    std::string frame_id;
};

struct NoMessage : public Message
{
public:
    typedef boost::shared_ptr<NoMessage> Ptr;

protected:
    NoMessage();

public:
    virtual ConnectionType::Ptr clone() ;
    virtual ConnectionType::Ptr toType();

    static ConnectionType::Ptr make();

    bool canConnectTo(const ConnectionType* other_side) const;
    bool acceptsConnectionFrom(const ConnectionType* other_side) const;
};

struct AnyMessage : public Message
{
public:
    typedef boost::shared_ptr<AnyMessage> Ptr;

protected:
    AnyMessage();

public:
    virtual ConnectionType::Ptr clone() ;
    virtual ConnectionType::Ptr toType();

    static ConnectionType::Ptr make();

    bool canConnectTo(const ConnectionType* other_side) const;
    bool acceptsConnectionFrom(const ConnectionType* other_side) const;
};


template <typename Type>
struct GenericMessage : public Message {
    typedef boost::shared_ptr<GenericMessage<Type> > Ptr;

    GenericMessage()
        : Message(type2nameWithoutNamespace(typeid(Type)))
    {}

    virtual ConnectionType::Ptr clone() {
        Ptr new_msg(new GenericMessage<Type>);
        new_msg->value = value;
        return new_msg;
    }

    virtual ConnectionType::Ptr toType() {
        Ptr new_msg(new GenericMessage<Type>);
        return new_msg;
    }

    static ConnectionType::Ptr make(){
        Ptr new_msg(new GenericMessage<Type>);
        return new_msg;
    }

    bool acceptsConnectionFrom(const ConnectionType* other_side) const {
        return name() == other_side->name();
    }

    void writeYaml(YAML::Emitter& yaml) const {
        yaml << YAML::Key << "value" << YAML::Value << "not implemented";
    }
    void readYaml(YAML::Node& node) {
    }

    typename boost::shared_ptr<Type> value;
};

template <typename Type>
struct DirectMessage : public Message {
    typedef boost::shared_ptr<DirectMessage<Type> > Ptr;

    DirectMessage()
        : Message(type2name(typeid(Type)))
    {}

    virtual bool isRosMessage() const
    {
        return false;
    }

    virtual ConnectionType::Ptr clone() {
        Ptr new_msg(new DirectMessage<Type>);
        new_msg->value = value;
        return new_msg;
    }

    virtual ConnectionType::Ptr toType() {
        Ptr new_msg(new DirectMessage<Type>);
        return new_msg;
    }

    static ConnectionType::Ptr make(){
        Ptr new_msg(new DirectMessage<Type>);
        return new_msg;
    }

    bool acceptsConnectionFrom(const ConnectionType* other_side) const {
        return name() == other_side->name();
    }

    void writeYaml(YAML::Emitter& yaml) const {
        yaml << YAML::Key << "value" << YAML::Value << value;
    }
    void readYaml(YAML::Node& node) {
    }

    Type getValue() {
        return value;
    }

    Type value;
};

template <typename Type, class Instance>
struct MessageTemplate : public Message {
    typedef boost::shared_ptr<Instance> Ptr;

    MessageTemplate(const std::string& name)
        : Message(name)
    {}

    virtual ConnectionType::Ptr clone() {
        Ptr new_msg(new Instance);
        new_msg->value = value;
        return new_msg;
    }

    virtual ConnectionType::Ptr toType() {
            Ptr new_msg(new Instance);
        return new_msg;
    }

    static ConnectionType::Ptr make(){
        Ptr new_msg(new Instance);
        return new_msg;
    }

    bool acceptsConnectionFrom(const ConnectionType* other_side) const {
        return name() == other_side->name();
    }

    void writeYaml(YAML::Emitter& yaml) const {
        yaml << YAML::Key << "value" << YAML::Value << "not implemented";
    }
    void readYaml(YAML::Node& node) {
    }

    Type value;
};


}
}

#endif // MESSAGE_H
