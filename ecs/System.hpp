#pragma once
#include <type_traits>
#include <functional>
#include <new>

#include "Commands.hpp"
#include "query/Query.hpp"
#include "Global.hpp"

template<typename T>
concept IsCommands = std::is_same_v<std::remove_cv_t<T>, Commands>;

template<typename T>
concept IsWorldRef = std::is_same_v<std::remove_cv_t<T>, World&>;

template <typename T>
concept IsSystemParam = IsQueryRef<T> || IsCommands<T> || IsWorldRef<T>;

/** Virtual base for all systems. Used for polymorphism & type erasure */
struct ISystem
{
    virtual void worldUpdated(World &world) = 0;
    friend World;

private:
    virtual ~ISystem() = default;
    virtual void run() = 0;
};

template <IsSystemParam... Params>
struct System : public ISystem
{
private:
    std::tuple<Params...> m_params;               // Parameters that are to be passed to the inner system
    std::function<void(Params...)> m_innerSystem; // The real underlying function of this system

    System() = delete;
    System(std::tuple<Params...> params, std::function<void(Params...)> system) : m_params(params), m_innerSystem(system) {};

    /**
     * @brief Called by the `World` at the end of a tick when it was updated. Allows this `System` to update its parameters if needed.
     *
     * @param world
     */
    void worldUpdated(World &world) override {
        // Empty for the moment. Might come in handy for additionnal `SystemParam`s
    };

public:
    friend World;

    /**
     * @brief Runs this system with the given parameters
     */
    void run() override
    {
        // Apply the parameters to the system
        std::apply(m_innerSystem, m_params);
    };
};

/// @cond TURN_OFF_DOXYGEN
namespace internal
{
    template <typename T>
    struct is_system_function : std::false_type
    {
    };

    template <IsSystemParam... Params>
    struct is_system_function<std::function<void(Params...)>> : std::true_type
    {
    };
};
/// @endcond

/** @brief Asserts that a given type is a valid `System` function */
template <typename T>
concept IsSystemFunction = internal::is_system_function<T>::value;