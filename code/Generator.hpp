
// Este código es de dominio público
// angel.rodriguez@udit.es

#pragma once

#include <coroutine>
#include <exception>
#include <optional>

namespace udit
{

    template< typename TYPE >
    class Generator
    {
    public:

        class promise_type
        {
            std::optional< TYPE > current_value;

        public:

            Generator<TYPE> get_return_object ()
            {
                return std::coroutine_handle< promise_type >::from_promise (*this);
            }

            auto yield_value         (const TYPE & value) { current_value = value; return std::suspend_always (); }
            void return_void         () { }
            auto initial_suspend     () { return std::suspend_never{}; }
            auto final_suspend       () noexcept { return std::suspend_always{}; }
            void unhandled_exception () { std::terminate (); }

            const std::optional< TYPE > & get_current_value ()
            {
                return current_value;
            }
        };

        class Iterator
        {
            Generator * handler;

        public:

            explicit Iterator(Generator * given_handler) : handler(given_handler)
            {
            }

            void operator ++ ()
            {
                handler->resume ();
            }

            const TYPE & operator * () const
            {
                return handler->current ().value ();
            }

            bool operator == (nullptr_t) const
            {
                return not handler->handle || handler->done ();
            }

        };

    private:

        std::coroutine_handle< promise_type > handle;

    public:

        Generator(std::coroutine_handle< promise_type > coroutine_handle) : handle(coroutine_handle)
        {
        }

       ~Generator()
        {
            handle.destroy ();
        }

        Generator(Generator && ) = delete;
        Generator(const Generator & ) = delete;
        
        Generator & operator = (Generator && ) = delete;
        Generator & operator = (const Generator & ) = delete;

    public:

        bool done () const
        {
            return handle.done ();
        }

        void resume () const
        {
            if (handle) handle.resume ();
        }

        void operator () () const
        {
            handle ();
        }

        const std::optional< TYPE > & current () const
        {
            return handle.promise ().get_current_value ();
        }

        Iterator begin ()
        {
            return Iterator{ this };
        }

        nullptr_t end ()
        {
            return nullptr;
        }

    };

}
