/// HEADER
#include <csapex/connection_type_manager.h>

using namespace csapex;

ConnectionTypeManager::ConnectionTypeManager()
{
}

ConnectionType::Ptr ConnectionTypeManager::createMessage(const std::string& type)
{
    ConnectionTypeManager& i = instance();

    if(i.classes.empty()) {
        throw std::runtime_error("no connection types registered!");
    }

    if(i.classes.find(type) == i.classes.end()) {
        throw std::runtime_error(std::string("no such type (") + type + ")");
    }

    return i.classes[type]();
}

void ConnectionTypeManager::registerMessage(const std::string &type, Constructor constructor)
{
    ConnectionTypeManager& i = instance();

    std::map<std::string, Constructor>::const_iterator it = i.classes.find(type);
    assert(it == i.classes.end());

    i.classes[type] = constructor;
}
