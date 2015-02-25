#ifndef OUTPUT_TRANSITION_H
#define OUTPUT_TRANSITION_H

/// COMPONENT
#include <csapex/msg/transition.h>

namespace csapex
{
class OutputTransition : public Transition
{
public:
    OutputTransition(NodeWorker* node);

    bool isSink() const;
    bool canSendMessages() const;
    void sendMessages();
    void notifyMessageProcessed();

    void setConnectionsReadyToReceive();

private:
    bool areOutputsDone() const;

    void fillConnections();
};
}

#endif // OUTPUT_TRANSITION_H

