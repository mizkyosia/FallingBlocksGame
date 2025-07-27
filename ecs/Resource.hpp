#pragma once
#include "Global.hpp"

struct IResource
{
    virtual ~IResource() = default;
};

template <typename Res>
struct Resource : IResource
{
private:
    std::shared_ptr<Res> m_handle;

    /**
     * @brief Construct a new Resource object from its inner handle
     *
     * @param res
     */
    Resource(std::shared_ptr<Res> res) : m_handle(res) {};

public:
    friend World;

    template <typename T>
    friend class Resource;

    /**
     * @brief Copy constructor
     *
     * @param other
     */
    Resource(const Resource<Res> &other) : m_handle(other.m_handle) {};

    /**
     * @brief Access to the underlying resource
     *
     * @return Res&
     */
    Res &operator*() { return *m_handle; };
    /**
     * @brief Access to the underlying resource
     *
     * @return const Res&
     */
    const Res &operator*() const { return *m_handle; };

    /**
     * @brief Access to the underlying resource
     *
     * @return Res*
     */
    Res *operator->() { return m_handle.get(); };
    /**
     * @brief Access to the underlying resource
     *
     * @return const Res*
     */
    const Res *operator->() const { return m_handle.get(); };
};

namespace traits
{
    template <typename T>
    struct resource_type : std::false_type
    {
        using type = void;
    };

    template <typename T>
    struct resource_type<Resource<T>> : std::true_type
    {
        using type = T;
    };
} // namespace traits

template <typename T>
concept IsResource = traits::resource_type<T>::value;