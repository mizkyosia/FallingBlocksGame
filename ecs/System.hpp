#pragma once
#include <type_traits>
#include <functional>
#include <tuple>
#include <new>

#include "Commands.hpp"
#include "query/Query.hpp"
#include "Global.hpp"
#include "Resource.hpp"

template <typename T>
concept IsCommands = std::is_same_v<std::remove_cv_t<T>, Commands>;

template <typename T>
concept IsWorldRef = std::is_same_v<std::remove_cv_t<T>, World &>;

template <typename T>
concept IsSystemParam = IsQueryRef<T> || IsCommands<T> || IsWorldRef<T> || IsResource<T>;

/// @cond TURN_OFF_DOXYGEN

namespace traits
{
    template <typename T>
    struct system_state : std::false_type
    {
        using type = std::tuple<>;
    };

    template <IsSystemParam... Ts>
    struct system_state<void(*)(Ts...)> : std::true_type
    {
        using type = std::tuple<Ts...>;
    };
}
/// @endcond

/** Virtual base for all systems. Used for polymorphism & type erasure */
struct ISystem
{
    virtual void worldUpdated(World &world) = 0;
    friend World;

    virtual ~ISystem() = default;

private:
    virtual void run() = 0;
};

template <IsSystemParam... Params>
struct System : public ISystem
{
private:
    std::tuple<Params...> m_params;   //!< Parameters that are to be passed to the inner system
    void (*m_innerSystem)(Params...); //!< The real underlying function of this system

    System() = delete;

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

    System(std::tuple<Params...> params, void (*system)(Params...)) : m_params(params), m_innerSystem(system) {};
    ~System() = default;

    /**
     * @brief Runs this system with the given parameters
     */
    void run() override
    {
        // Apply the parameters to the system
        std::apply(m_innerSystem, m_params);
    };
};

/** @brief Asserts that a given type is a valid `System` function */
template <typename T>
concept IsSystemFunction = traits::system_state<T>::value;