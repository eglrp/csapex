#ifndef UUID_PROVIDER_H
#define UUID_PROVIDER_H

/// COMPONENT
#include <csapex/utility/uuid.h>

namespace csapex
{

class UUIDProvider
{
    friend class UUID;
    friend class GraphIO; // TODO: remove

public:
    UUIDProvider();
    virtual ~UUIDProvider();

    UUID makeUUID(const std::string& name);
    UUID generateUUID(const std::string& prefix);

    UUID makeDerivedUUID(const UUID& parent, const std::string& name);
    UUID makeConnectableUUID(const UUID &parent, const std::string &type, int sub_id);

    static UUID makeUUID_forced(const std::string& representation);
    static UUID makeDerivedUUID_forced(const UUID& parent, const std::string& name);
    static UUID makeConnectableUUID_forced(const UUID &parent, const std::string &type, int sub_id);



protected:
    void reset();
    void free(const UUID& uuid);
    std::string generateUniquePrefix(const std::string& name);

private:
    std::mutex hash_mutex_;
    std::map<std::string, int> hash_;

    std::map<std::string, int> uuids_;
};

}

#endif // UUID_PROVIDER_H