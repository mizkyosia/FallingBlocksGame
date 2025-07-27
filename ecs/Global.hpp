#pragma once
#include <cstdint>
#include <bitset>
#include <bits/shared_ptr.h>

#ifndef MAX_COMPONENTS
#define MAX_COMPONENTS 32
#endif

#ifndef MAX_ENTITIES
#define MAX_ENTITIES 5000
#endif

/** @brief Forward declaration for an ECS World */
class World;

/**
 * @brief Type ID of a component type
 */
using ComponentID = std::uint8_t;

/** @brief Type of an entity's ID */
using Entity = uint32_t;

/** @brief Signature of an entity or a system. Fast representation of which components are attached to an entity */
using Signature = std::bitset<MAX_COMPONENTS>;

namespace traits
{
    template <typename T, template <typename...> class Template>
    struct specialization_of : std::false_type
    {
    };

    template <template <typename...> class Template, typename... Args>
    struct specialization_of<Template<Args...>, Template> : std::true_type
    {
    };

    template <typename T>
    struct first_type_param
    {
        using type = void;
    };

    template <template <typename...> class Template, typename First, typename... Args>
    struct first_type_param<Template<First, Args...>>
    {
        using type = First;
    };

    template <typename T, typename... Ts>
    constexpr std::size_t count_occurrences = (0 + ... + (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Ts>> ? 1 : 0));
}

/** @brief Helper trait for checking whether a type is a specification of a given template */
template <typename T, template <typename...> class Template>
concept IsSpecializationOf = traits::specialization_of<T, Template>::value;

/** @brief Helper trait to get the inner type of a template */
template <typename T>
concept GetFirstParam = traits::first_type_param<T>::type;

/** @brief Checks if a specific type is in a parameter pack */
template <typename T, typename... Ts>
concept InPack = (std::is_same_v<T, Ts> || ...);

/** @brief Asserts that all components in the subset is also part of the superset */
template <typename... Subset, typename... Superset>
concept AllInPack = (InPack<Subset, Superset...> && ...);

/** @brief Checks if there are any duplicate types in the given parameter pack */
template <typename... Ts>
constexpr bool has_duplicate_types = ([]<typename T>()
                                      { return traits::count_occurrences<T, Ts...> > 1; }.template operator()<Ts>() ||
                                      ...); // fold expression over parameter pack

/**
 * @brief Global informations on the location of an `Entity`
 *
 */
struct EntityLocation
{
    Signature signature = 0;
    size_t row = SIZE_MAX;   //!< Default instantiation to SIZE_MAX so that an error is thrown if it is not set after construction
};