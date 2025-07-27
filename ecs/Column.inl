#include "Column.hpp"
#include "World.hpp"

#include <memory>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <bits/unique_ptr.h>

template <typename T>
inline T *IColumn::get(size_t elementPosition)
{
    return static_cast<T *>(get(elementPosition));
}

template <typename T>
inline T *IColumn::operator[](size_t elementPosition)
{
    return static_cast<T *>(get(elementPosition));
}

inline void *IColumn::operator[](size_t elementPosition)
{
    return get(elementPosition);
}

inline IColumn::IColumn(ComponentID componentID) : componentID(componentID)
{
}

template <typename Component>
inline Column<Component>::Column(World &world) : IColumn(world.getComponentID<Component>())
{
}

template <typename T>
inline size_t Column<T>::size()
{
    return m_components.size();
}

template <typename Component>
void Column<Component>::push(void *element)
{
    m_components.push_back(std::move(*static_cast<Component *>(element)));
}

template <typename Component>
inline void Column<Component>::pop()
{
    m_components.pop_back();
}

template <typename T>
void *Column<T>::get(size_t elementPosition)
{
    if (elementPosition >= m_components.size())
        throw std::out_of_range{"Column access out of range"};
    return static_cast<void *>(&m_components[elementPosition]);
}

template <typename T>
void Column<T>::erase(size_t index)
{
    auto size = m_components.size();

    if (size == 0)
        throw std::out_of_range{"Column : erase operation out of bounds"};

    if (size > 1)
        m_components[index] = std::move(m_components[size - 1]);

    pop();
}

template <typename T>
inline void Column<T>::resize(size_t newSize)
{
    m_components.resize(newSize);
}

template <typename Component>
inline void Column<Component>::setComponentUnsafe(void *data, size_t index)
{
    m_components[index] = std::move(*static_cast<Component *>(data));
}

template <typename Component>
inline void Column<Component>::set(size_t index, Component component)
{
    m_components[index] = std::move(component);
}

template <typename Component>
inline IColumn *Column<Component>::clone()
{
    return new Column<Component>(*this);
}

template <typename Component>
inline Column<Component>::Column(Column<Component> &other) : IColumn(other.componentID){};