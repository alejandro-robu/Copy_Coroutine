
// Este código es de dominio público
// angel.rodriguez@udit.es

#pragma once

#include <coroutine>
#include <exception>

namespace udit
{

    class Resumable
    {
    public:

        class promise_type
        {
        public:

            Resumable get_return_object ()
            {
                return std::coroutine_handle< promise_type >::from_promise (*this);
            }

            auto initial_suspend     () { return std::suspend_never{}; }
            auto final_suspend       () noexcept { return std::suspend_always{}; }
            void return_void         () { }
            void unhandled_exception () { std::terminate (); }
        };

    private:

        std::coroutine_handle< promise_type > handle;

    public:

        Resumable(std::coroutine_handle< promise_type > coroutine_handle) : handle(coroutine_handle)
        {
        }
        ~Resumable()
        {
            if (handle)
                handle.destroy();
        }


       Resumable(Resumable&& other) noexcept
           : handle(other.handle)
       {
           other.handle = nullptr;
       }

        Resumable(const Resumable & ) = delete;
        
        Resumable & operator = (const Resumable & ) = delete;
        Resumable& operator = (Resumable&& other) noexcept
        {
            if (this != &other)
            {
                if (handle)
                    handle.destroy();

                handle = other.handle;
                other.handle = nullptr;
            }

            return *this;
        }

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

    };

}
