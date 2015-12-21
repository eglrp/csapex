#ifndef ANGLE_PARAMETER_H
#define ANGLE_PARAMETER_H

/// COMPONENT
#include <csapex/param/value_parameter.h>

namespace csapex {
namespace param {


class AngleParameter : public Parameter
{
    friend class ParameterFactory;

public:
    typedef std::shared_ptr<AngleParameter> Ptr;

public:
    AngleParameter();
    explicit AngleParameter(const std::string& name, const ParameterDescription& description, double angle, double min = -M_PI, double max = M_PI);

    virtual const std::type_info &type() const override;

    virtual int ID() const override { return 0x00B; }
    virtual std::string TYPE() const override { return "angle"; }

    virtual std::string toStringImpl() const override;

    void doSetValueFrom(const Parameter& other) override;
    void doClone(const Parameter& other) override;

    void doSerialize(YAML::Node& e) const override;
    void doDeserialize(const YAML::Node& n) override;

    double min() const;
    double max() const;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /*file_version*/) {
        ar & angle_;
    }

protected:
    virtual boost::any get_unsafe() const override;
    virtual bool set_unsafe(const boost::any& v) override;

private:
    double angle_;
    double min_;
    double max_;
};

}
}

#endif // ANGLE_PARAMETER_H
