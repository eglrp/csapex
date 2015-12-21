#ifndef VALUE_PARAMETER_H
#define VALUE_PARAMETER_H

/// COMPONENT
#include <csapex/param/parameter.h>

namespace csapex {
namespace param {

class ValueParameter : public Parameter
{
    friend class ParameterFactory;

public:
    typedef std::shared_ptr<ValueParameter> Ptr;

public:
    ValueParameter();
    explicit ValueParameter(const std::string& name, const ParameterDescription &description);
    virtual ~ValueParameter();

    virtual int ID() const override { return 0x008; }
    virtual std::string TYPE() const override { return "value"; }

    virtual const std::type_info &type() const override;
    virtual std::string toStringImpl() const override;

    void doSetValueFrom(const Parameter& other) override;
    void doClone(const Parameter& other) override;

    void doSerialize(YAML::Node& e) const override;
    void doDeserialize(const YAML::Node& n) override;

    template <typename T>
    T def() const { return read<T>(def_); }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /*file_version*/) {
        ar & value_;
    }

protected:
    virtual boost::any get_unsafe() const override;
    virtual bool set_unsafe(const boost::any& v) override;

private:
    template <typename T>
    T read(const boost::any& var) const
    {
        try {
            return boost::any_cast<T> (var);

        } catch(const boost::bad_any_cast& e) {
            throw std::logic_error(std::string("typeof ValueParameter is not ") + typeid(T).name() + ": " + e.what());
        }
    }

private:
    boost::any value_;
    boost::any def_;
};

}
}

#endif // VALUE_PARAMETER_H