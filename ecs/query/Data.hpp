#pragma once
#include <concepts>

/**
 * @brief Query data specifier. Returns a `bool` value representing if an entity has the specified `Component`
 *
 * @tparam Component
 */
template <typename Component>
struct Has
{
};

/**
 * @brief Query data specifier. Fetches the requested component if an entity has it, or `nullptr` otherwise
 *
 * @tparam Component
 */
template <typename Component>
struct Maybe
{
};

// Forward declaration for concept
struct EntityCommands;

/** @cond TURN_OFF_DOXYGEN */
namespace traits
{
    template <typename T>
    struct is_query_data : std::true_type
    {
    };

    template <typename T>
    struct is_query_data<T *> : std::false_type
    {
    };

    template <typename T>
    struct is_query_data<T &> : std::false_type
    {
    };

    template <>
    struct is_query_data<EntityCommands> : std::true_type
    {
    };

    template <typename T>
    struct is_query_data<Maybe<T>> : std::true_type
    {
    };

    template <typename T>
    struct is_query_data<Has<T>> : std::true_type
    {
    };
}
/** @endcond  */
