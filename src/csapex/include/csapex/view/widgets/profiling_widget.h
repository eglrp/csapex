#ifndef PROFILING_WIDGET_H
#define PROFILING_WIDGET_H

/// COMPONENT
#include <csapex/profiling/profiler.h>
#include <csapex/utility/slim_signal.hpp>

/// SYSTEM
#include <QWidget>
#include <map>

class QSpacerItem;
class QVBoxLayout;

namespace csapex
{

class ProfilingWidget : public QWidget
{
    Q_OBJECT

public:
    ProfilingWidget(std::shared_ptr<Profiler> profiler, QWidget* parent=0);
    ~ProfilingWidget();

public Q_SLOTS:
    void reset();
    void exportCsv();

protected:
    void paintEvent(QPaintEvent *);
    void paintInterval(QPainter &p, const Timer::Interval &interval);
    float paintInterval(QPainter &p, const Timer::Interval &interval, float height_offset, int depth);

private:
    std::shared_ptr<Profiler> profiler_;

    std::vector<slim_signal::ScopedConnection> connections_;

    QVBoxLayout* layout_;
    QSpacerItem* space_for_painting_;
    float bar_height_;
    float content_height_ ;

    float left_space;
    float padding;
    float line_height;


    float left;
    float right;
    float up;
    float bottom;

    double max_time_ms_;

    double current_draw_x;

    double content_width_;
    double indiv_width_;

    std::map<std::string, QColor> steps_;
};

}

#endif // PROFILING_WIDGET_H
