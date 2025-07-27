#pragma once
#include "Global.hpp"
#include <cstdio>
#include <vector>

struct IColumn
{
    const ComponentID componentID;

    /**
     * @brief Gets the element at the given position, cast as the given type
     *
     * @tparam T
     * @param index
     * @return T*
     */
    template <typename T>
    T *get(size_t index);

    /**
     * @brief Gets the element at the given position, cast as the given type
     *
     * @tparam T
     * @param index
     * @return T*
     */
    template <typename T>
    T *operator[](size_t index);

    /**
     * @brief Gets the element at the given position
     *
     * @param index
     * @return void*
     */
    void *operator[](size_t index);

    /**
     * @brief Gets the element at the given position
     *
     * @param index
     * @return void*
     */
    virtual void *get(size_t index) = 0;
    /**
     * @brief Size of the `Column`
     *
     * @return size_t
     */
    virtual size_t size() = 0;
    /**
     * @brief Performs a swap & pop operation to remove the element at given index
     *
     * @param index
     */
    virtual void erase(size_t index) = 0;
    /**
     * @brief Pushes the given element into the `Column`
     *
     * @param element
     */
    virtual void push(void *element) = 0;
    /**
     * @brief Remove the `Column`'s last element
     *
     */
    virtual void pop() = 0;

    /**
     * @brief Resizes the `Column`
     * 
     * @param newSize 
     */
    virtual void resize(size_t newSize) = 0;

    /**
     * @brief Sets the component at the given inidex
     * 
     * @param data 
     * @param index 
     */
    virtual void setComponentUnsafe(void* data, size_t index) = 0;

    /**
     * @brief Returns a new instance of `Column` holding the same type of `Component`
     * 
     * @return IColumn* 
     */
    virtual IColumn* clone() = 0;

    IColumn(ComponentID componentID);
};

template <typename Component>
struct Column : public IColumn
{
    std::vector<Component> m_components;

    void erase(size_t index) override;
    void *get(size_t index) override;
    size_t size() override;
    void push(void *element) override;
    void pop() override;
    void resize(size_t newSize) override;
    void setComponentUnsafe(void* data, size_t index) override;
    IColumn* clone() override;

    /**
     * @brief Sets the component at the given index
     * 
     * @param index 
     * @param component 
     */
    void set(size_t index, Component component);

    Column(World& world);
    ~Column() = default;
private: 
    Column(Column<Component>& other);

};
