#ifndef SLOT_H
#define SLOT_H

/// COMPONENT
#include <csapex/model/connectable.h>
#include <csapex/csapex_fwd.h>

/// SYSTEM
#include <QMutex>
#include <QWaitCondition>

namespace csapex
{

class Slot : public Connectable
{
    Q_OBJECT

    friend class Trigger;
    friend class command::AddConnection;
    friend class command::MoveConnection;
    friend class command::DeleteConnection;

public:
    Slot(boost::function<void()> callback, Settings &settings, const UUID &uuid);
    Slot(boost::function<void()> callback, Settings& settings, Unique *parent, int sub_id);
    virtual ~Slot();

    virtual void trigger();

    virtual bool canInput() const {
        return true;
    }
    virtual bool isInput() const {
        return true;
    }

    virtual bool canConnectTo(Connectable* other_side, bool move) const;


    virtual bool targetsCanBeMovedTo(Connectable* other_side) const;
    virtual bool isConnected() const;

    virtual void connectionMovePreview(Connectable* other_side);
    virtual void validateConnections();

    Connectable* getSource() const;

    virtual CommandPtr removeAllConnectionsCmd();

    virtual void enable();
    virtual void disable();

    virtual void notifyMessageProcessed();

    void reset();

protected:
    virtual bool tryConnect(Connectable* other_side);
    virtual bool acknowledgeConnection(Connectable* other_side);
    virtual void removeConnection(Connectable* other_side);
    virtual void removeAllConnectionsNotUndoable();

Q_SIGNALS:
    void triggered();

private Q_SLOTS:
    void handleTrigger();

protected:
    Connectable* target;

    QMutex trigger_exec_mutex_;
    QWaitCondition exec_finished_;

    boost::function<void()> callback_;
};

}
#endif // SLOT_H