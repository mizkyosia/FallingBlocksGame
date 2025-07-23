#pragma once
#include <cstdint>
#include <bitset>

/** \brief Forward declaration for an ECS World */
class World;

/**
 * \brief Type ID of a component type
 */
using ComponentID = std::uint8_t;

/** \brief Type of an entity's ID */
using Entity = uint32_t;

/** \brief Maximum number of components that can be registered */
inline constexpr ComponentID MaxComponents = 32;

/** \brief Signature of an entity or a system. Fast representation of which components are attached to an entity */
using Signature = std::bitset<MaxComponents>;

template <typename T, template <typename...> class Template>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

/** @brief Helper trait for checking whether a type is a specification of a given template */
template <typename T, template <typename...> class Template>
concept IsSpecializationOf = is_specialization_of<T, Template>::value;

/** @brief Checks if a specific type is in a parameter pack */
template<typename T, typename ...Ts>
concept InPack = (std::is_same_v<T, Ts> || ...);