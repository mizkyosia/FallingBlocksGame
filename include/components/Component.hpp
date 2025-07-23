#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <bits/shared_ptr.h>

#include <managers/ComponentManager.hpp>

#define DECLARE_COMPONENT()                                                   \
private:                                                                      \
    inline static ComponentID s_ComponentID{ComponentManager::MaxComponents}; \
public:                                                                       \
    friend ::App;                                                             \
    inline static ComponentID ComponentID() { return s_ComponentID; };

/** \brief Concept asserting that the given type param is a component */
template <typename T>
concept IsComponent = requires {
    { T::s_ComponentID } -> std::same_as<ComponentID>;
};

/** \brief Concept asserting that the given type parameter is a reference to a component */
template <typename T>
concept IsComponentRef = std::is_reference_v<T> && IsComponent<std::remove_reference_t<T>>;

// Forward declaration for the macro
class App;