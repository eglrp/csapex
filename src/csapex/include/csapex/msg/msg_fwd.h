#ifndef MSG_FWD_H
#define MSG_FWD_H

/// shared_ptr
#include <memory>

#define FWD(name) \
    class name;\
    typedef std::shared_ptr<name> name##Ptr;\
    typedef std::unique_ptr<name> name##UniquePtr;\
    typedef std::weak_ptr<name> name##WeakPtr;\
    typedef std::shared_ptr<const name> name##ConstPtr;


namespace csapex
{
FWD(Input);
FWD(DynamicInput);
FWD(Output);
FWD(DynamicOutput);
FWD(InputTransition);
FWD(OutputTransition);
FWD(MessageProvider);


namespace connection_types
{
FWD(Message);

template <typename Type>
struct GenericPointerMessage;
template <typename Type>
struct GenericValueMessage;
}
}

#undef FWD

#endif // MSG_FWD_H
