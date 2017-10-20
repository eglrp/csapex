/// HEADER
#include <csapex/io/protcol/graph_requests.h>

/// PROJECT
#include <csapex/command/command.h>
#include <csapex/io/feedback.h>
#include <csapex/io/raw_message.h>
#include <csapex/io/session.h>
#include <csapex/model/graph_facade_local.h>
#include <csapex/model/graph.h>
#include <csapex/serialization/parameter_serializer.h>
#include <csapex/serialization/request_serializer.h>
#include <csapex/serialization/serialization_buffer.h>
#include <csapex/utility/uuid_provider.h>

/// SYSTEM
#include <iostream>

CSAPEX_REGISTER_REQUEST_SERIALIZER(GraphRequests)

using namespace csapex;

///
/// REQUEST
///
GraphRequests::GraphRequest::GraphRequest(GraphRequestType request_type, const AUUID &uuid)
    : RequestImplementation(0),
      request_type_(request_type),
      uuid_(uuid)
{

}

GraphRequests::GraphRequest::GraphRequest(uint8_t request_id)
    : RequestImplementation(request_id)
{

}

ResponsePtr GraphRequests::GraphRequest::execute(const SessionPtr &session, CsApexCore &core) const
{
    GraphFacade* gf = core.getRoot()->getSubGraph(uuid_);
    GraphFacadeLocal* gf_local = dynamic_cast<GraphFacadeLocal*>(gf);
    apex_assert_hard(gf_local);

    switch(request_type_)
    {
        /**
         * begin: generate cases
         **/
#define HANDLE_ACCESSOR(_enum, type, function) \
    case GraphRequests::GraphRequestType::_enum:\
        return std::make_shared<GraphResponse>(request_type_, uuid_, nf->function(), getRequestID());
#define HANDLE_STATIC_ACCESSOR(_enum, type, function) HANDLE_ACCESSOR(_enum, type, function)
#define HANDLE_DYNAMIC_ACCESSOR(_enum, signal, type, function) HANDLE_ACCESSOR(_enum, type, function)
#define HANDLE_SIGNAL(_enum, signal)

    #include <csapex/model/graph_facade_remote_accessors.hpp>
        /**
         * end: generate cases
         **/


    default:
        return std::make_shared<Feedback>(std::string("unknown graph request type ") + std::to_string((int)request_type_),
                                          getRequestID());
    }

    return std::make_shared<GraphResponse>(request_type_, uuid_, getRequestID());
}

void GraphRequests::GraphRequest::serialize(SerializationBuffer &data) const
{
    data << request_type_;
    data << uuid_;
    data << arguments_;
}

void GraphRequests::GraphRequest::deserialize(const SerializationBuffer& data)
{
    data >> request_type_;
    data >> uuid_;
    data >> arguments_;
}

///
/// RESPONSE
///

GraphRequests::GraphResponse::GraphResponse(GraphRequestType request_type, const AUUID& uuid, uint8_t request_id)
    : ResponseImplementation(request_id),
      request_type_(request_type),
      uuid_(uuid)
{

}
GraphRequests::GraphResponse::GraphResponse(GraphRequestType request_type, const AUUID& uuid, boost::any result, uint8_t request_id)
    : ResponseImplementation(request_id),
      request_type_(request_type),
      uuid_(uuid),
      result_(result)
{

}

GraphRequests::GraphResponse::GraphResponse(uint8_t request_id)
    : ResponseImplementation(request_id)
{

}

void GraphRequests::GraphResponse::serialize(SerializationBuffer &data) const
{
    data << request_type_;
    data << uuid_;
    data << result_;
}

void GraphRequests::GraphResponse::deserialize(const SerializationBuffer& data)
{
    data >> request_type_;
    data >> uuid_;
    data >> result_;
}
