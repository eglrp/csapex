/// HEADER
#include <csapex/model/fulcrum.h>

/// COMPONENT
#include <csapex/model/connection.h>

using namespace csapex;


Fulcrum::Fulcrum(Connection* parent, const QPointF& p, int type, const QPointF &handle_in, const QPointF &handle_out)
    : parent_(parent), type_(type), pos_(p), handle_in_(handle_in), handle_out_(handle_out)
{}

void Fulcrum::move(const QPointF& pos, bool dropped)
{
    pos_ = pos;
    moved(this, dropped);
}

QPointF Fulcrum::pos() const
{
    return pos_;
}

void Fulcrum::moveHandles(const QPointF& in, const QPointF& out, bool dropped)
{
    if(in != handle_in_ || out != handle_out_ || dropped) {
        handle_in_ = in;
        handle_out_ = out;
        movedHandle(this, dropped, Fulcrum::HANDLE_BOTH);
    }
}


void Fulcrum::moveHandleIn(const QPointF& pos, bool dropped)
{
    if(pos != handle_in_ || dropped) {
        handle_in_ = pos;
        movedHandle(this, dropped, Fulcrum::HANDLE_IN);
    }
}

QPointF Fulcrum::handleIn() const
{
    return handle_in_;
}

void Fulcrum::moveHandleOut(const QPointF& pos, bool dropped)
{
    if(pos != handle_out_ || dropped) {
        handle_out_ = pos;
        movedHandle(this, dropped, Fulcrum::HANDLE_OUT);
    }
}

QPointF Fulcrum::handleOut() const
{
    return handle_out_;
}

int Fulcrum::id() const
{
    return id_;
}

int Fulcrum::connectionId() const
{
    return parent_->id();
}

void Fulcrum::setId(int id)
{
    id_ = id;
}

int Fulcrum::type() const
{
    return type_;
}

void Fulcrum::setType(int type)
{
    type_ = type;
    typeChanged(this, type);
}

Connection* Fulcrum::connection() const
{
    return parent_;
}
/// MOC
#include "../../include/csapex/model/moc_fulcrum.cpp"
