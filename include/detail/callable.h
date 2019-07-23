#pragma once

#include <tuple>

#include "traits.h"

namespace task_executor
{
    inline namespace invoker_v1
    {
        template<class T>
        struct callable :
            callable<decltype(&T::operator())>
        {
            virtual ~callable() = default;

            callable(T * obj) :
                callable<decltype(&T::operator())>{ obj, &T::operator() }
            {}
        };

        template<class R, class... A>
        struct callable<R(*)(A...)>
        {
            virtual ~callable() = default;

            explicit callable(R(*func)(A...)) :
                func{ func }
            {}

            R operator() (A && ... args)
            {
                return func(std::forward(args)...);
            }

            R(*func)(A...) = nullptr;
        };

        template<class C, class R, class... A>
        struct callable<R(C::*)(A...)>
        {
            virtual ~callable() = default;

            explicit callable(C * obj, R(C::*memFunc)(A...)) :
                obj{ obj },
                memFunc{ memFunc }
            {}

            R operator() (A && ... args)
            {
                if (obj)
                    return (obj->*memFunc)(std::forward(args)...);
                else
                    return std::declval<R>();
            }

            C * obj = nullptr;
            R(C::*memFunc)(A...) = nullptr;
        };
        
        template<class C, class R, class... A>
        struct callable<R(C::*)(A...) const>
        {
            virtual ~callable() = default;

            explicit callable(C * const obj, R(C::*memFunc)(A...) const) :
                obj{ obj },
                memFunc{ memFunc }
            {}

            R operator() (A && ... args)
            {
                if (obj)
                    return (obj->*memFunc)(std::forward(args)...);
                else
                    return std::declval<R>();
            }

            C * const obj;
            R(C::*memFunc)(A...) const;
        };

        template<class C, class R, class... A>
        struct callable<R(C::*)(A...) volatile>
        {
            virtual ~callable() = default;

            explicit callable(C * volatile obj, R(C::*memFunc)(A...) volatile) :
                obj{ obj },
                memFunc{ memFunc }
            {}

            R operator() (A && ... args)
            {
                if (obj)
                    return (obj->*memFunc)(std::forward(args)...);
                else
                    return std::declval<R>();
            }

            C * volatile obj;
            R(C::*memFunc)(A...) volatile;
        };

        template<class C, class R, class... A>
        struct callable<R(C::*)(A...) const volatile>
        {
            virtual ~callable() = default;

            explicit callable(C * const volatile obj, R(C::*memFunc)(A...) const volatile) :
                obj{ obj },
                memFunc{ memFunc }
            {}

            R operator() (A && ...)
            {
                if (obj)
                    return (obj->*memFunc)(std::forward(args)...);
                else
                    return std::declval<R>();
            }

            C * const volatile obj;
            R(C::*memFunc)(A...) const volatile;
        };

        template<class T>
        auto make_callable(T * obj)
        {
            return callable<decltype(&T::operator())>{ obj };
        }

        template<class R, class... A>
        auto make_callable(R(*func)(A...))
        {
            return callable<R(*)(A...)>{ func };
        }

        template<class C, class R, class... A>
        auto make_callable(C * obj, R(C::*func)(A...))
        {
            return callable<R(C::*)(A...)>{ obj, func };
        }
        
        template<class C, class R, class... A>
        auto make_callable(C * const obj, R(C::* func)(A...) const)
        {
            return callable<R(C::*)(A...) const>{ obj, func };
        }

        template<class C, class R, class... A>
        auto make_callable(C * volatile obj, R(C::* func)(A...) volatile)
        {
            return callable<R(C::*)(A...) volatile>{ obj, func };
        }

        template<class C, class R, class... A>
        auto make_callable(C * const volatile obj, R(C::* func)(A...) const volatile)
        {
            return callable<R(C::*)(A...) const volatile>{ obj, func };
        }
    }
}
