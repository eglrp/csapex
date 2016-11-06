#ifndef GRAPH_H
#define GRAPH_H

/// COMPONENT
#include <csapex/model/model_fwd.h>
#include <csapex/utility/uuid_provider.h>
#include <csapex/csapex_export.h>

/// SYSTEM
#include <csapex/utility/slim_signal.hpp>
#include <map>
#include <functional>
#include <set>

namespace csapex {

class CSAPEX_EXPORT Graph : public UUIDProvider
{
    friend class GraphIO;
    friend class GraphFacade;

public:
    typedef std::shared_ptr<Graph> Ptr;

    struct NodeNotFoundException : public std::logic_error
    {
        NodeNotFoundException(const std::string& name)
            : std::logic_error("node " + name + " cannot be found")
        {}
    };

    struct NodeHandleNotFoundException : public std::logic_error
    {
        NodeHandleNotFoundException(const std::string& name)
            : std::logic_error("node handle for node " + name + " cannot be found")
        {}
    };

    typedef std::vector<graph::VertexPtr>::iterator vertex_iterator;
    typedef std::vector<graph::VertexPtr>::const_iterator vertex_const_iterator;

public:
    Graph();
    virtual ~Graph();

    int getComponent(const UUID& node_uuid) const;
    int getDepth(const UUID& node_uuid) const;

    void resetActivity();

    void clear();

    Node* findNode(const UUID& uuid) const;
    Node* findNodeNoThrow(const UUID& uuid) const noexcept;
    Node* findNodeForConnector(const UUID &uuid) const;

    virtual NodeHandle* findNodeHandle(const UUID& uuid) const;
    virtual NodeHandle* findNodeHandleNoThrow(const UUID& uuid) const noexcept;
    NodeHandle* findNodeHandleForConnector(const UUID &uuid) const;
    NodeHandle* findNodeHandleForConnectorNoThrow(const UUID &uuid) const noexcept;
    NodeHandle* findNodeHandleWithLabel(const std::string& label) const;

    std::vector<NodeHandle*> getAllNodeHandles();


    virtual Connectable* findConnector(const UUID &uuid);
    virtual Connectable* findConnectorNoThrow(const UUID &uuid) noexcept;

    template <typename T>
    T *findConnector(const UUID &uuid)
    {
        return dynamic_cast<T*>(findConnector(uuid));
    }
    template <typename T>
    T *findConnectorNoThrow(const UUID &uuid) noexcept
    {
        return dynamic_cast<T*>(findConnectorNoThrow(uuid));
    }

    ConnectionPtr getConnectionWithId(int id);
    ConnectionPtr getConnection(const UUID& from, const UUID& to);
    ConnectionPtr getConnection(Connectable* from, Connectable* to);
    int getConnectionId(ConnectionPtr);

    std::vector<ConnectionPtr> getConnections();

    int countNodes();

    void addNode(NodeHandlePtr node);
    void deleteNode(const UUID &uuid, bool quiet = false);

    bool addConnection(ConnectionPtr connection, bool quiet = false);
    void deleteConnection(ConnectionPtr connection, bool quiet = false);

    void triggerConnectionsAdded();

    void analyzeGraph();

    // iterators
    vertex_iterator beginVertices();
    const vertex_const_iterator beginVertices() const;

    vertex_iterator endVertices();
    const vertex_const_iterator endVertices() const;


private:
    void checkNodeState(NodeHandle* nh);

    void buildConnectedComponents();
    void calculateDepths();

public:
    csapex::slim_signal::Signal<void()> state_changed;
    csapex::slim_signal::Signal<void(Graph*)> structureChanged;

    csapex::slim_signal::Signal<void(Connection*)> connectionAdded;
    csapex::slim_signal::Signal<void(Connection*)> connectionDeleted;

    csapex::slim_signal::Signal<void(graph::VertexPtr)> vertex_added;
    csapex::slim_signal::Signal<void(graph::VertexPtr)> vertex_removed;

protected:
//    std::vector<NodeHandlePtr> nodes_;
    std::vector<graph::VertexPtr> vertecies_;
    std::vector<ConnectionPtr> connections_;

    std::map<const graph::Vertex*, int> node_component_;
    std::map<const graph::Vertex*, int> node_depth_;

    std::map<const graph::Vertex*, std::vector<graph::Vertex*> > node_parents_;
    std::map<const graph::Vertex*, std::vector<graph::Vertex*> > node_children_;


    std::set<const graph::Vertex*> sources_;
    std::set<const graph::Vertex*> sinks_;
};

}

#endif // GRAPH_H
