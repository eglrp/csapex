#ifndef token_traits_H
#define token_traits_H

/// COMPONENT
#include <csapex/model/token_data.h>
#include <csapex/utility/tmp.hpp>
#include <csapex/model/model_fwd.h>

/// SYSTEM
#include <string>
#include <type_traits>
#include <vector>

namespace csapex {
namespace connection_types {

/// TRAITS
template <typename T>
struct type;

template <typename T>
inline std::string serializationName()
{
    typedef typename std::remove_const<T>::type TT;
    return type<TT>::name();
}

TokenPtr makeToken(const TokenDataConstPtr& data);

HAS_MEM_FUNC(makeEmpty, has_make_empty);

template <typename T, typename std::enable_if<has_make_empty<T, std::shared_ptr<T>(*)()>::value, int>::type = 0>
inline std::shared_ptr<T> makeEmpty()
{
    return T::makeEmpty();
}

template <typename T, typename std::enable_if<!has_make_empty<T, std::shared_ptr<T>(*)()>::value, int>::type = 0>
inline std::shared_ptr<T> makeEmpty()
{
    return std::make_shared<T>();
}


template <typename T>
inline TokenPtr makeEmptyToken()
{
    return makeToken(connection_types::makeEmpty<T>());
}



template <typename M, bool is_message>
struct MessageContainer;

template <typename M>
struct MessageContainer<M, true>
{
    typedef M type;

    static M& access(M& msg) {
        return msg;
    }
    static const M& accessConst(const M& msg) {
        return msg;
    }
};


template <template <class> class Container, typename T, class Enable = void>
class MessageConversionHook
{
public:
    static void registerConversion() {
        // do nothing
    }
};


template <typename T>
std::shared_ptr<T> makeEmptyMessage(
        typename std::enable_if<!std::is_const<T>::value >::type* = 0)
{
    return makeEmpty<T>();
}
template <typename T>
std::shared_ptr<typename std::remove_const<T>::type > makeEmptyMessage(
        typename std::enable_if<std::is_const<T>::value >::type* = 0)
{
    typedef typename std::remove_const<T>::type TT;
    return makeEmpty<TT>();
}


HAS_MEM_TYPE(Ptr, has_ptr_member);
HAS_MEM_TYPE(element_type, has_elem_type_member);

template<typename T> struct is_std_vector : std::false_type {};
template<typename T, typename Allocator> struct is_std_vector<std::vector<T, Allocator>> : std::true_type {};

template <typename M>
struct should_use_pointer_message {
    static constexpr bool value =
            std::is_class<M>::value &&
            has_ptr_member<M>::value &&
            !std::is_same<std::string, M>::value &&
            !std::is_base_of<TokenData, M>::value;
};

template <typename M>
struct should_use_value_message {
    static constexpr bool value =
            !should_use_pointer_message<M>::value &&
            !has_elem_type_member<M>::value && // reject shared_ptr
            !std::is_base_of<TokenData, M>::value;
};

template <typename M>
struct should_use_no_generic_message {
    static constexpr bool value =
            !should_use_pointer_message<M>::value &&
            !should_use_value_message<M>::value;
};



}
}


#endif // token_traits_H
