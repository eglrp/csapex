#ifndef NODE_REQUESTS_H
#define NODE_REQUESTS_H

/// PROJECT
#include <csapex/io/request_impl.hpp>
#include <csapex/io/response_impl.hpp>
#include <csapex/param/parameter.h>
#include <csapex/serialization/serialization_fwd.h>

namespace csapex
{

class NodeRequests
{
public:

    enum class NodeRequestType
    {
        AddClient,
        RemoveClient,

        GetDebugDescription
    };

    class NodeRequest : public RequestImplementation<NodeRequest>
    {
    public:
        NodeRequest(uint8_t request_id);
        NodeRequest(NodeRequestType request_type, const AUUID& uuid);

        virtual void serialize(SerializationBuffer &data) const override;
        virtual void deserialize(SerializationBuffer& data) override;

        virtual ResponsePtr execute(const SessionPtr& session, CsApexCore& core) const override;

        std::string getType() const override
        {
            return "NodeRequests";
        }

    private:
        NodeRequestType request_type_;
        AUUID uuid_;
    };


    class NodeResponse : public ResponseImplementation<NodeResponse>
    {
    public:
        NodeResponse(uint8_t request_id);
        NodeResponse(NodeRequestType request_type, const AUUID& uuid, uint8_t request_id);
        NodeResponse(NodeRequestType request_type, const AUUID& uuid, boost::any result, uint8_t request_id);

        virtual void serialize(SerializationBuffer &data) const override;
        virtual void deserialize(SerializationBuffer& data) override;

        std::string getType() const override
        {
            return "NodeRequests";
        }        

        template <typename R>
        R getResult() const
        {
            return boost::any_cast<R>(result_);
        }


    private:
        NodeRequestType request_type_;
        AUUID uuid_;

        boost::any result_;
    };


public:
    using RequestT = NodeRequest;
    using ResponseT = NodeResponse;
};

}
#endif // NODE_REQUESTS_H
