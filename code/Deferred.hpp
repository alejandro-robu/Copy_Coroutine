
// Este código es de dominio público
// angel.rodriguez@udit.es

#pragma once

#include <coroutine>
#include <exception>
#include <optional>

namespace udit
{

    template< typename TYPE >
    class Deferred
    {
    public:

        class promise_type
        {
            std::optional< TYPE > result;

        public:

            Deferred<TYPE> get_return_object ()
            {
                return std::coroutine_handle< promise_type >::from_promise (*this);
            }

            void return_value        (const TYPE & value) { result = value; }
            auto initial_suspend     () { return std::suspend_never{}; }
            auto final_suspend       () noexcept { return std::suspend_always{}; }
            void unhandled_exception () { std::terminate (); }

            const std::optional< TYPE > & get_result ()
            {
                return result;
            }
        };

    private:

        std::coroutine_handle< promise_type > handle;

    public:

        Deferred(std::coroutine_handle< promise_type > coroutine_handle) : handle(coroutine_handle)
        {
        }

       ~Deferred()
        {
            handle.destroy ();
        }

        Deferred(Deferred && ) = delete;
        Deferred(const Deferred & ) = delete;

        Deferred & operator = (Deferred && ) = delete;
        Deferred & operator = (const Deferred & ) = delete;

    public:

        bool done () const
        {
            return handle.done ();
        }

        void resume () const
        {
            handle.resume ();
        }

        void operator () () const
        {
            handle ();
        }

        const std::optional< TYPE > & result () const
        {
            return handle.promise ().get_result ();
        }

    };

}
