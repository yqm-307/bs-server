#pragma once
#include "share/boost_define.hpp"

namespace ybs::share::util
{

template<typename T>
class ClassInfo
{
public:
    typedef std::shared_ptr<T> SPtr;
    typedef T*  RawPtr;
    typedef T&  Ref;
};

}