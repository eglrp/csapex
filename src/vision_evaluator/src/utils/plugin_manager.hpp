#ifndef PLUGIN_MANAGER_HPP
#define PLUGIN_MANAGER_HPP

/// COMPONENT
#include "constructor.hpp"
#include "stream_interceptor.h"

/// PROJECT
#include <designer/boxed_object.h>
#include <designer/selector_proxy.h>

/// SYSTEM
#include <boost/signals2.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <pluginlib/class_loader.h>

template <class M, class C>
class PluginManagerImp
{
    template <class, class>
    friend class PluginManager;

protected:
    typedef C Constructor;
    typedef std::map<std::string, Constructor> Constructors;

    static PluginManagerImp<M,C>& instance(const std::string& full_name) {
        static PluginManagerImp<M,C> i(full_name);
        return i;
    }

protected:
    typedef pluginlib::ClassLoader<M> Loader;

    PluginManagerImp(const std::string& full_name)
        : loader_(new Loader("vision_evaluator", full_name)) {
        StreamInterceptor::instance();
    }

    PluginManagerImp(const PluginManagerImp& rhs);
    PluginManagerImp& operator = (const PluginManagerImp& rhs);

protected:
    virtual ~PluginManagerImp() {}

    void registerConstructor(Constructor constructor) {
        available_classes[constructor.getType()] = constructor;
    }

    template <class Class>
    typename boost::enable_if<boost::is_base_of<vision_evaluator::BoxedObject, Class> >::type
    loadSelectorProxy(const std::string& name) {
        std::cout << "loaded instance of boxedobject:" << name << std::endl;

        vision_evaluator::SelectorProxy::Ptr dynamic(new vision_evaluator::SelectorProxyDynamic(name, boost::bind(&Loader::createUnmanagedInstance, loader_, name)));
        vision_evaluator::SelectorProxy::registerProxy(dynamic);
    }

    template <class Class>
    typename boost::disable_if<boost::is_base_of<vision_evaluator::BoxedObject, Class> >::type
    loadSelectorProxy(const std::string& name) {
    }

    void reload() {
        try {
            std::vector<std::string> classes = loader_->getDeclaredClasses();
            for(std::vector<std::string>::iterator c = classes.begin(); c != classes.end(); ++c) {
                std::cout << "load library for class " << *c << std::endl;
                loader_->loadLibraryForClass(*c);

                loadSelectorProxy<M>(*c);

                Constructor constructor;
                constructor.setType(*c);
                constructor.setConstructor(boost::bind(&Loader::createUnmanagedInstance, loader_, *c));
                registerConstructor(constructor);
                std::cout << "loaded " << typeid(M).name() << " class " << *c << std::endl;
            }
        } catch(pluginlib::PluginlibException& ex) {
            ROS_ERROR("The plugin failed to load for some reason. Error: %s", ex.what());
        }
        plugins_loaded_ = true;
    }

protected:
    bool plugins_loaded_;
    Loader* loader_;

    Constructors available_classes;
};

template <class M, class C = DefaultConstructor<M> >
class PluginManager
{
protected:
    typedef PluginManagerImp<M, C> Parent;

public:
    typedef typename Parent::Constructor Constructor;
    typedef typename Parent::Constructors Constructors;

    PluginManager(const std::string& full_name)
        : instance(Parent::instance(full_name))
    {}

    virtual ~PluginManager()
    {}

    virtual void registerConstructor(Constructor constructor) {
        instance.registerConstructor(constructor);
    }

    virtual bool pluginsLoaded() const {
        return instance.plugins_loaded_;
    }

    virtual void reload() {
        instance.reload();
    }

    const Constructors& availableClasses() const {
        return instance.available_classes;
    }
    const Constructor& availableClasses(unsigned index) const {
        typename Constructors::iterator it = instance.available_classes.begin();
        std::advance(it, index);
        return it->second;
    }
    const Constructor& availableClasses(const std::string& key) const {
        return instance.available_classes[key];
    }
    Constructor& availableClasses(const std::string& key) {
        return instance.available_classes[key];
    }

protected:
    Parent& instance;
};

#endif // PLUGIN_MANAGER_HPP
