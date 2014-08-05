/// HEADER
#include <csapex/msg/input.h>

/// COMPONENT
#include <csapex/msg/output.h>
#include <csapex/command/delete_connection.h>
#include <csapex/command/command.h>
#include <csapex/utility/assert.h>

/// SYSTEM
#include <iostream>

using namespace csapex;

Input::Input(Settings& settings, const UUID &uuid)
    : Connectable(settings, uuid), target(NULL), buffer_(new Buffer(1)), optional_(false)
{
}

Input::Input(Settings &settings, Unique* parent, int sub_id)
    : Connectable(settings, parent, sub_id, TYPE_IN), target(NULL), buffer_(new Buffer(1)), optional_(false)
{
}

Input::~Input()
{
    if(target != NULL) {
        target->removeConnection(this);
    }

    free();
}

bool Input::tryConnect(Connectable* other_side)
{
    if(!other_side->canOutput()) {
        std::cerr << "cannot connect, other side can't output" << std::endl;
        return false;
    }

    return other_side->tryConnect(this);
}

bool Input::acknowledgeConnection(Connectable* other_side)
{
    target = dynamic_cast<Output*>(other_side);
    connect(other_side, SIGNAL(destroyed(QObject*)), this, SLOT(removeConnection(QObject*)));
    connect(other_side, SIGNAL(enabled(bool)), this, SIGNAL(connectionEnabled(bool)));
    return true;
}

void Input::removeConnection(Connectable* other_side)
{
    if(target != NULL) {
        apex_assert_hard(other_side == target);
        target = NULL;

        Q_EMIT connectionRemoved();
    }
}

Command::Ptr Input::removeAllConnectionsCmd()
{
    Command::Ptr cmd(new command::DeleteConnection(target, this));
    return cmd;
}

void Input::setOptional(bool optional)
{
    optional_ = optional;
}

bool Input::isOptional() const
{
    return optional_;
}

void Input::setAsync(bool asynch)
{
    QMutexLocker lock(&sync_mutex);

    Connectable::setAsync(asynch);

    if(!asynch && isConnected()) {
        free();
        setSequenceNumber(getSource()->sequenceNumber());
    }
}

bool Input::hasMessage() const
{
    return buffer_->isFilled();
}

void Input::free()
{
    buffer_->free();

    setBlocked(false);
}

void Input::enable()
{
    Connectable::enable();
//    if(isConnected() && !getSource()->isEnabled()) {
//        getSource()->enable();
//    }
}

void Input::disable()
{
    Connectable::disable();
//    if(isConnected() && getSource()->isEnabled()) {
//        getSource()->disable();
//    }
}

void Input::removeAllConnectionsNotUndoable()
{
    if(target != NULL) {
        target->removeConnection(this);
        target = NULL;
        setError(false);
        Q_EMIT disconnected(this);
    }
}


bool Input::canConnectTo(Connectable* other_side, bool move) const
{
    return Connectable::canConnectTo(other_side, move) && (move || !isConnected());
}

bool Input::targetsCanBeMovedTo(Connectable* other_side) const
{
    return target->canConnectTo(other_side, true) /*&& canConnectTo(getConnected())*/;
}

bool Input::isConnected() const
{
    return target != NULL;
}

void Input::connectionMovePreview(Connectable *other_side)
{
    Q_EMIT(connectionInProgress(getSource(), other_side));
}


void Input::validateConnections()
{
    bool e = false;
    if(isConnected()) {
        if(!target->getType()->canConnectTo(getType().get())) {
            e = true;
        }
    }

    setError(e);
}

Connectable *Input::getSource() const
{
    return target;
}

void Input::inputMessage(ConnectionType::Ptr message)
{
    int s = message->sequenceNumber();
    if(s < sequenceNumber() && !isAsync()) {
        std::cerr << "connector @" << getUUID().getFullName() <<
                     ": dropping old message @ with #" << s <<
                     " < #" << sequenceNumber() << std::endl;
        return;
    }

    setBlocked(true);

    buffer_->write(message);
    setSequenceNumber(s);

    count_++;

    Q_EMIT messageArrived(this);
}