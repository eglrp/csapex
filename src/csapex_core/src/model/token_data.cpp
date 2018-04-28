/// HEADER
#include <csapex/model/token.h>

/// COMPONENT
#include <csapex/msg/message.h>
#include <csapex/msg/token_traits.h>
#include <csapex/utility/assert.h>
#include <csapex/serialization/io/std_io.h>

/// SYSTEM
#include <iostream>

using namespace csapex;

TokenData::TokenData()
{
}

TokenData::TokenData(const std::string& type_name)
    : type_name_(type_name)
{
    setDescriptiveName(type_name);
}

TokenData::TokenData(const std::string& type_name, const std::string& descriptive_name)
    : type_name_(type_name), descriptive_name_(descriptive_name)
{
}

TokenData::~TokenData()
{
}

void TokenData::setDescriptiveName(const std::string &name)
{
    descriptive_name_ = name;
}

bool TokenData::canConnectTo(const TokenData *other_side) const
{
    return other_side->acceptsConnectionFrom(this);
}

bool TokenData::acceptsConnectionFrom(const TokenData *other_side) const
{
    return type_name_ == other_side->typeName();
}

std::string TokenData::descriptiveName() const
{
    return descriptive_name_;
}

std::string TokenData::typeName() const
{
    return type_name_;
}

void TokenData::serialize(SerializationBuffer &data) const
{
    data << type_name_;
    data << descriptive_name_;
}
void TokenData::deserialize(const SerializationBuffer& data)
{
    data >> type_name_;
    data >> descriptive_name_;
}

TokenData::Ptr TokenData::toType() const
{
    return cloneAs<TokenData>();
}

bool TokenData::isValid() const
{
    return true;
}

bool TokenData::isContainer() const
{
    return false;
}

TokenData::Ptr TokenData::nestedType() const
{
    throw std::logic_error("cannot get nested type for non-container messages");
}
TokenData::ConstPtr TokenData::nestedValue(std::size_t index) const
{
    throw std::logic_error("cannot get nested value for non-container messages");
}
std::size_t TokenData::nestedValueCount() const
{
    throw std::logic_error("cannot get nested count for non-container messages");
}
void TokenData::addNestedValue(const ConstPtr &msg)
{
    throw std::logic_error("cannot add nested value to non-container messages");
}

void TokenData::writeNative(const std::string &/*file*/, const std::string &/*base*/, const std::string& /*suffix*/) const
{
    std::cerr << "error: writeRaw not implemented for message type " << descriptiveName() << std::endl;
}

uint8_t TokenData::getPacketType() const
{
    return PACKET_TYPE_ID;
}
