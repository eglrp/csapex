#ifndef PORT_H
#define PORT_H

/// PROJECT
#include <csapex/model/connectable.h>

/// SYSTEM
#include <QFrame>

namespace csapex
{

class Port : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString class READ cssClass)

public:
    Port(Connectable* adaptee);

    QString cssClass() {
        return QString("Port");
    }

    void setMinimizedSize(bool mini);
    bool isMinimizedSize() const;

    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

    void dragEnterEvent(QDragEnterEvent* e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);

    virtual QPoint topLeft();
    virtual QPoint centerPoint();

    virtual bool canOutput() const;
    virtual bool canInput() const;
    virtual bool isOutput() const;
    virtual bool isInput() const;

    Connectable* getAdaptee() const;

protected:
    void paintEvent(QPaintEvent *);
    void refreshStylesheet();

protected:
    Connectable * adaptee_;
    bool refresh_style_sheet_;
    bool minimized_;

    Qt::MouseButtons buttons_down_;
};

}

#endif // PORT_H