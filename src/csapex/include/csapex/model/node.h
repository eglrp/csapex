#ifndef NODE_H_
#define NODE_H_

/// COMPONENT
#include <csapex/msg/msg_fwd.h>
#include <csapex/signal/signal_fwd.h>
#include <csapex/model/parameterizable.h>
#include <csapex/utility/stream_relay.h>
#include <csapex/utility/assert.h>
#include <csapex/utility/timable.h>

namespace csapex {

class Node : public Parameterizable, public Timable
{
public:
    typedef std::shared_ptr<Node> Ptr;

public:
    Node();
    virtual ~Node();

    void initialize(const UUID &uuid, NodeModifier *node_modifier);

    void doSetup();
    bool isSetup() const;


public: /* API */
    virtual void setup(csapex::NodeModifier& node_modifier) = 0;

    virtual void setupParameters(Parameterizable& parameters);

    virtual void process(csapex::Parameterizable& parameters);
    virtual void process(csapex::Parameterizable& parameters, std::function<void(std::function<void ()>)> continuation);

    virtual void abort();

    virtual bool isAsynchronous() const;
    virtual void stateChanged();

protected:
    virtual void process(); /*deprecated*/

public:
    StreamRelay adebug;
    StreamRelay ainfo;
    StreamRelay awarn;
    StreamRelay aerr;

protected:
    NodeModifier* modifier_;
    bool setup_;

    long guard_;
};

}

#endif // NODE_H_
