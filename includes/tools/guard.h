#pragma once
#include <functional>
namespace Tools
{
    template <typename FuncType>
    class Guarder
    {
    public:
        using func_type = std::function<FuncType>;
        Guarder(func_type _start, func_type _end) : start(_start), end(_end)
        {
            _start();
        }
        ~Guarder() { end(); }

    private:
        func_type start;
        func_type end;
    };
}