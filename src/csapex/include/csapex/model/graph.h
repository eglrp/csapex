#ifndef GRAPH_H
#define GRAPH_H

/// COMPONENT
#include <csapex/model/connection.h>
#include <csapex/command/meta.h>
#include <csapex/csapex_fwd.h>
#include <csapex/utility/uuid.h>

/// SYSTEM
#include <QObject>
#include <map>
#include <boost/function.hpp>

namespace csapex {

class Graph : public QObject
{
    Q_OBJECT

    friend class GraphIO;
    friend class GraphWorker;
    friend class command::AddNode;
    friend class command::AddConnection;
    friend class command::DeleteConnection;
    friend class command::DeleteNode;
    friend class DesignerView;
    friend class DesignerScene;
    friend class Template;
    friend class CommandDispatcher;

    friend class WidgetController;
    friend class BoxSelectionModel;
    friend class ConnectionSelectionModel;

public:
    typedef boost::shared_ptr<Graph> Ptr;

public:
    Graph();
    virtual ~Graph();

    /*extract dispatcher*/ void init(CommandDispatcher* dispatcher);

    Node* findNode(const UUID& uuid) const;
    Node* findNodeNoThrow(const UUID& uuid) const;
    Node* findNodeForConnector(const UUID &uuid) const;

    int getComponent(const UUID& node_uuid) const;

    Connectable* findConnector(const UUID &uuid);

    Command::Ptr deleteConnectionByIdCommand(int id);

    Command::Ptr deleteConnectionFulcrumCommand(int connection, int fulcrum);

    Command::Ptr deleteAllConnectionFulcrumsCommand(int connection);
    Command::Ptr deleteAllConnectionFulcrumsCommand(Connection::Ptr connection);

    Command::Ptr deleteConnectionById(int id);

    Connection::Ptr getConnectionWithId(int id);
    Connection::Ptr getConnection(Connection::Ptr);
    int getConnectionId(Connection::Ptr);

    std::string makeUUIDPrefix(const std::string& name);

    Command::Ptr clear();

    int countNodes();

    void foreachNode(boost::function<void (Node*)> f);
    void foreachNode(boost::function<void (Node*)> f, boost::function<bool (Node*)> pred);

private:
   /*rename*/ void verify();

Q_SIGNALS:
    void stateChanged();
    void structureChanged(Graph*);
    /*extract?*/ void dirtyChanged(bool);

    void connectionAdded(Connection*);
    void connectionDeleted(Connection*);

    void nodeAdded(NodePtr);
    void nodeRemoved(NodePtr);

private: /// ONLY COMMANDS / NOT UNDOABLE
    void addNode(NodePtr node);
    void deleteNode(const UUID &uuid);

    bool addConnection(Connection::Ptr connection);
    void deleteConnection(Connection::Ptr connection);

    void buildConnectedComponents();
    void verifyAsync();

protected:
    std::vector<NodePtr> nodes_;
    std::map<Node*, int> node_component_;

    std::map<Node*, std::vector<Node*> > node_parents_;
    std::map<Node*, std::vector<Node*> > node_children_;

    std::vector<Connection::Ptr> connections_;

    /*extract*/ CommandDispatcher* dispatcher_;

    std::map<std::string, int> uuids_;
};

}

#endif // GRAPH_H
