
// Este código es de dominio público
// angel.rodriguez@udit.es

#pragma once

#include <chrono>
#include <coroutine>
#include <string>
#include <thread>

namespace udit
{

    using namespace std::chrono_literals;

    class Awaitable_Example
    {
        std::string result;

    public:

        bool await_ready ()
        {
            return false;
        }

        void await_suspend (std::coroutine_handle<> caller_handle)
        {
            std::thread
            (
                [this, caller_handle] ()
                {
                    std::this_thread::sleep_for (2000ms);

                    result = "done";

                    caller_handle.resume();
                }
            )
            .detach();
        }

        std::string await_resume ()
        {
            return result;
        }
    };

}
