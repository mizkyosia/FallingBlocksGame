#pragma once
#include <_Entity.hpp>
#include <functional>

struct ISystemWrapper
{
    virtual void run();
    virtual ~ISystemWrapper() = default;
};

template <typename... _Components>
struct SystemWrapper
{
    SystemWrapper(std::function<_Components...> innerSystem);
};

struct SystemTest
{
    Signature signature;
    std::unique_ptr<ISystemWrapper> system;

    /** \brief Converting constructor */
    template <typename... _Components>
    SystemTest(std::function<_Components...> innerSystem) : system(std::make_unique<SystemWrapper<_Components...>>(innerSystem))
    {
    }
};
