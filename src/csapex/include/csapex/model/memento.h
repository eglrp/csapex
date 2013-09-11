#ifndef MEMENTO_H
#define MEMENTO_H

/// SYSTEM
#include <boost/shared_ptr.hpp>
#include <yaml-cpp/yaml.h>

namespace csapex
{

class Memento
{
public:
    typedef boost::shared_ptr<Memento> Ptr;
    static const Ptr NullPtr;

public:
    Memento();
    virtual ~Memento();

    virtual void writeYaml(YAML::Emitter& out) const;
    virtual void readYaml(const YAML::Node& node);
};

}

#endif // MEMENTO_H