/// HEADER
#include <csapex/overlay.h>

/// COMPONENT
#include <csapex/boxed_object.h>
#include <csapex/connection.h>
#include <csapex/graph.h>
#include <csapex/connector_out.h>
#include <csapex/connector_in.h>
#include <csapex/command_meta.h>
#include <csapex/command_delete_connection.h>
#include <csapex/box.h>

/// SYSTEM
#include <boost/foreach.hpp>
#include <iostream>
#include <cmath>
#include <QApplication>
#include <QTimer>
#include <QPainter>
#include <QFontMetrics>
#include <QResizeEvent>

using namespace csapex;

namespace {
QRgb id2rgb(int id)
{
    return qRgb(qRed(id), qGreen(id), qBlue(id));
}

int rgb2id(QRgb rgb)
{
    int raw = (rgb & 0xFFFFFF);
    if(raw >= 0xFFFFFF) {
        return -1;
    }
    return raw;
}
}

Overlay::Overlay(QWidget* parent)
    : QWidget(parent), highlight_connection_id_(-1), schema_dirty_(true)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);

    repainter = new QTimer();
    repainter->setInterval(125);
    repainter->start();

    activity_marker_min_width_ = 6;
    activity_marker_max_width_ = 10;
    activity_marker_min_opacity_ = 25;
    activity_marker_max_opacity_ = 75;

    connector_radius_ = 7;

    color_connected = QColor(0x33, 0x33, 0x33, 0xAA);
    color_disconnected = color_in_connected.darker();

    color_in_connected = QColor(0x33, 0x33, 0xFF, 0xAA);
    color_in_disconnected = color_in_connected.darker();

    color_out_connected = QColor(0x33, 0xFF, 0x33, 0xAA);
    color_out_disconnected = color_out_connected.darker();

    setMouseTracking(true);

    QObject::connect(repainter, SIGNAL(timeout()), this, SLOT(repaint()));
    QObject::connect(repainter, SIGNAL(timeout()), this, SLOT(tick()));
}

void Overlay::addTemporaryConnection(Connector *from, const QPoint& end)
{
    TempConnection temp;
    temp.from = from;
    temp.to = end;

    temp_.push_back(temp);
}

void Overlay::addTemporaryConnection(Connector *from, Connector *to)
{
    TempConnection temp;
    temp.from = from;
    temp.to = to->centerPoint();

    temp_.push_back(temp);
}

void Overlay::deleteTemporaryConnections()
{
    temp_.clear();
}

void Overlay::deleteTemporaryConnectionsAndRepaint()
{
    deleteTemporaryConnections();
    repaint();
}

void Overlay::drawConnection(Connection& connection)
{
    if(!connection.from()->isOutput() || !connection.to()->isInput()) {
        return;
    }

    ConnectorOut* from = dynamic_cast<ConnectorOut*> (connection.from());
    ConnectorIn* to = dynamic_cast<ConnectorIn*> (connection.to());

    if(from->getBox()->isHidden() || to->getBox()->isHidden()) {
        return;
    }

    QPoint p1 = from->centerPoint();
    QPoint p2 = to->centerPoint();

    int id = connection.id();

    int flags = FLAG_NONE;

    if(highlight_connection_id_ == id) {
        flags |= FLAG_HIGHLIGHT;
    }
    if(to->isError() || from->isError()) {
        flags |= FLAG_ERROR;
    }
    if(connection.isSelected()) {
        flags |= FLAG_SELECTED;
    }
    if(!from->isEnabled() || !to->isEnabled()) {
        flags |= FLAG_DISABLED;
    }
    if(from->isMinimizedSize()) {
        flags |= FLAG_MINIMIZED_FROM;
    }
    if(to->isMinimizedSize()) {
        flags |= FLAG_MINIMIZED_TO;
    }

    drawConnection(p1, p2, id, flags);

    int f = connection.activity();

    drawActivity(f, from);
    drawActivity(f, to);
}


void Overlay::drawConnection(const QPoint& from, const QPoint& to, int id, unsigned flags)
{
    bool selected = (flags & FLAG_SELECTED) != 0;
    bool highlighted = (flags & FLAG_HIGHLIGHT) != 0;
    bool error = (flags & FLAG_ERROR) != 0;
    bool disabled = (flags & FLAG_DISABLED) != 0;
    bool minimized_from = (flags & FLAG_MINIMIZED_FROM) != 0;
    bool minimized_to = (flags & FLAG_MINIMIZED_TO) != 0;

    bool minimized = minimized_from || minimized_to;

    double r = minimized ? 3 : connector_radius_;

    double max_slack_height = 40.0;
    double mindist_for_slack = 60.0;
    double slack_smooth_distance = 300.0;

    QPoint diff = (to - from);

    double direct_length = hypot(diff.x(), diff.y());

    QPoint offset;
    QPoint delta;
    if(direct_length > mindist_for_slack) {
        double offset_factor = std::min(1.0, (direct_length - mindist_for_slack) / slack_smooth_distance);

        delta = QPoint(std::max(offset_factor * mindist_for_slack, std::abs(0.45 * (diff).x())), 0);
        offset = QPoint(0, offset_factor * max_slack_height);
    }

    QPoint cp1 = from + delta + offset;
    QPoint cp2 = to - delta + offset;

    QPainterPath path;
    path.moveTo(from);
    path.cubicTo(cp1, cp2, to);


    if(highlighted) {
        painter->setPen(QPen(Qt::black, r * 1.75, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(path);

        painter->setPen(QPen(Qt::white, r * 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(path);
    } else  if(selected) {
        painter->setPen(QPen(Qt::black, r * 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(path);

        QLinearGradient lg(from, to);
        if(error) {
            lg.setColorAt(0,Qt::darkRed);
            lg.setColorAt(1,Qt::red);
        } else {
            lg.setColorAt(0,color_out_connected.lighter(175));
            lg.setColorAt(1,color_in_connected.lighter(175));
        }

        painter->setPen(QPen(QBrush(lg), r * 1.3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(path);
    }

    Qt::PenStyle penstyle = from.x() > to.x() ? Qt::DotLine : Qt::SolidLine;
    QLinearGradient lg(from, to);
    if(error) {
        lg.setColorAt(0,Qt::darkRed);
        lg.setColorAt(1,Qt::red);
    } else if(disabled) {
        lg.setColorAt(0,Qt::darkGray);
        lg.setColorAt(1,Qt::gray);
    } else {
        lg.setColorAt(0,color_out_connected.lighter());
        lg.setColorAt(1,color_in_connected.lighter());
    }

    QPen gp = QPen(QBrush(lg), r * 0.75, penstyle, Qt::RoundCap, Qt::RoundJoin);

    painter->setPen(gp);
    painter->drawPath(path);

    if(id < 0) {
        return;
    }

    if(schema_dirty_) {
        QPen schema_pen = QPen(QColor(id2rgb(id)), connector_radius_ * 2, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);
        schematics_painter->setPen(schema_pen);
        schematics_painter->drawPath(path);
    }
}

void Overlay::drawConnector(Connector *c)
{
    bool output = c->isOutput();
    bool input = c->isInput();

    QColor color;

    if(c->isError()) {
        color= Qt::red;
    } else if(!c->isEnabled()) {
        color= Qt::gray;
    } else {
        if(c->isConnected()){
            color = output ? (input ? color_connected : color_out_connected) : color_in_connected;
        } else {
            color = output ? (input ? color_disconnected : color_out_disconnected) : color_in_disconnected;
        }
    }

    painter->setBrush(QBrush(color, Qt::SolidPattern));
    painter->setPen(QPen(color.darker(), 2));

    int font_size = 10;
    int lines = 3;
    if(c->isForwarding()) {
        painter->setPen(QPen(QBrush(color.darker()), 2, Qt::DotLine));
    }
    int r = c->isMinimizedSize() ? 4 : connector_radius_;
    painter->drawEllipse(c->centerPoint(), r, r);


    if(!c->isMinimizedSize()) {
        QFont font;
        font.setPixelSize(font_size);
        painter->setFont(font);

        QString text = c->getLabel().c_str();

        if(text.length() != 0) {
            text += "\n";
        }
        text += c->getType()->name().c_str();

        QFontMetrics metrics(font);

        int dx = 160;
        int dy = lines * metrics.height();

        QRectF rect(c->centerPoint() + QPointF(output ? 2*connector_radius_ : -2*connector_radius_-dx, -dy / 2.0), QSize(dx, dy));

        QTextOption opt(Qt::AlignVCenter | (output ? Qt::AlignLeft : Qt::AlignRight));
        painter->drawText(rect, text, opt);
    }
}

void Overlay::drawActivity(int life, Connector* c)
{
    if(c->isEnabled() && life > 0) {
        int r = std::min(Connection::activity_marker_max_lifetime_, life);
        double f = r / static_cast<double> (Connection::activity_marker_max_lifetime_);

        bool mini = c->isMinimizedSize();

        int min = mini ? 4 : activity_marker_min_width_;
        int max = mini ? 8 : activity_marker_max_width_;
        double w = min + f * (max - min);

        QColor color = c->isOutput() ? (c->isInput() ? color_connected : color_out_connected) : color_in_connected;
        color.setAlpha(activity_marker_min_opacity_ + (activity_marker_max_opacity_ - activity_marker_min_opacity_) * f);

        painter->setPen(QPen(color, w));
        painter->drawEllipse(QPointF(c->centerPoint()), w, w);
    }
}

void Overlay::tick()
{
}

bool Overlay::mouseMoveEventHandler(QMouseEvent *e)
{
    int x = e->x();
    int y = e->y();

    if(x < 0 || x >= schematics.width() || y < 0 || y >= schematics.height()) {
        return false;
    }

    unsigned int id = rgb2id(schematics.pixel(x,y));

    if(id != 3*255) {
        highlight_connection_id_ = id;
        repaint();
    } else {
        highlight_connection_id_ = -1;
    }

    return true;
}

bool Overlay::keyPressEventHandler(QKeyEvent*)
{
    return true;
}

bool Overlay::keyReleaseEventHandler(QKeyEvent* e)
{
    if(e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace) {
        Graph::root()->deleteSelectedConnections();

        repaint();

        return false;
    }

    return true;
}

bool Overlay::mousePressEventHandler(QMouseEvent *)
{
    return true;
}

bool Overlay::mouseReleaseEventHandler(QMouseEvent *e)
{
    Graph::Ptr graph_ = Graph::root();

    bool shift = Qt::ShiftModifier == QApplication::keyboardModifiers();

    if(highlight_connection_id_ != -1 && e->button() == Qt::MiddleButton) {
        graph_->deleteConnectionById(highlight_connection_id_);
        return false;
    }

    bool result = graph_->handleConnectionSelection(highlight_connection_id_, shift);

    repaint();

    return result;
}

void Overlay::setSelectionRectangle(const QPoint &a, const QPoint &b)
{
    if(b.x() > a.x()) {
        selection_a = a;
        selection_b = b;
    } else {
        selection_a = b;
        selection_b = a;
    }
}

void Overlay::paintEvent(QPaintEvent*)
{
    Graph::Ptr graph_ = Graph::root();

    QPainter p(this);
    QPainter ps(&schematics);

    painter = &p;
    schematics_painter = &ps;

    if(schema_dirty_) {
        schematics_painter->fillRect(0, 0, schematics.width(), schematics.height(), Qt::white);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 3));

    if(!temp_.empty()) {
        BOOST_FOREACH(TempConnection& temp, temp_) {

            if(dynamic_cast<ConnectorIn*> (temp.from)) {
                drawConnection(temp.to, temp.from->centerPoint(), -1, FLAG_SELECTED);
            } else {
                drawConnection(temp.from->centerPoint(), temp.to, -1, FLAG_SELECTED);
            }
        }
    }

    foreach(const Connection::Ptr& connection, graph_->connections) {
        drawConnection(*connection);
    }

    foreach (Box::Ptr box, graph_->boxes_) {
        if(box->getContent()->isError()) {
            QRectF rect(box->pos() + QPoint(0, box->height() + 8), QSize(box->width(), 64));

            QFont font;
            font.setPixelSize(8);
            painter->setFont(font);
            painter->setPen(Qt::red);

            QTextOption opt(Qt::AlignTop | Qt::AlignHCenter);
            painter->drawText(rect, box->getContent()->errorMessage().c_str(), opt);
        }

        for(int id = 0; id < box->countInputs(); ++id) {
            drawConnector(box->getInput(id));
        }
        for(int id = 0; id < box->countOutputs(); ++id) {
            drawConnector(box->getOutput(id));
        }
    }

    painter->setOpacity(0.35);

    if(!selection_a.isNull() && !selection_b.isNull()) {
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(QBrush(Qt::white));
        painter->drawRect(QRect(selection_a, selection_b));
    }

    //    painter->drawImage(QPoint(0,0), schematics);

    schema_dirty_ = false;

    painter = NULL;
    schematics_painter = NULL;
}

void Overlay::invalidateSchema()
{
    schema_dirty_ = true;
}

void Overlay::refresh()
{
    invalidateSchema();
}

void Overlay::resizeEvent(QResizeEvent *event)
{
    schematics = QImage(event->size(), QImage::Format_RGB888);
    invalidateSchema();
}
