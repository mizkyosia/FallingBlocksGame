#pragma once
#include <bits/shared_ptr.h>
#include <managers/AssetManager.hpp>
#include <functional>

namespace Assets
{
    /**
     * Represents a handle to an asset that may or may not have been loaded yet.
     *
     * A worse version of a `shared_ptr`, but easier to manage
     * \tparam _AssetType Type of the underlying data, e.g `sf::Texture`
     * \tparam _AssetType Type of the class derived from the asset, e.g `Core::Texture`
     */
    template <typename _Derived, typename DataType>
    struct Asset
    {
        using SelfType = Asset<_Derived, DataType>;
        using Path = std::filesystem::path;

        inline static const volatile AssetType AssetTypeID{AssetManager::Register<_Derived>()};

    private:
        friend AssetManager;

        /**
         * \brief Actual pointer to the asset. Using a `shared_ptr` so it is correctly freed
         */
        std::shared_ptr<DataType> m_handle;

        /**
         * \brief Is the asset loaded yet ? If not, the value inside the handle may be wrong.
         */
        bool *m_loaded;

    protected:
    public:
        /** \brief Cannot create an empty asset */
        Asset() = delete;

        /**
         * \brief Create an asset handle for the asset at the given path.
         *
         * If this asset has not been loaded yet, starts loading it from the given path using the given builder function */
        Asset(std::filesystem::path path, std::function<DataType()> builder) : m_loaded(new bool(false)), m_handle(std::make_shared<DataType>())
        {
            // Inserts a handle for the asset into the system, so that it is not loaded twice
            AssetManager::InsertAsset<_Derived>(path, _Derived(static_cast<_Derived &>(*this)));
            // Then load the asset using its builder function
            AssetManager::LoadAsset(std::async(std::launch::async, [builder = std::move(builder), this]()
                                               {
                                            // Build data
                                            DataType data = builder();
                                            // Locking shenanigans for access through other threads
                                           std::mutex mut;
                                           {
                                            std::lock_guard lock(mut);
                                            // Move the data inside the handle
                                            *m_handle = std::move(data);
                                           } }));
        }

        /**
         * \brief Copies the necessary data from another asset handle of the same type
         * \param other
         */
        Asset(const SelfType &other) : m_handle(other.m_handle), m_loaded(other.m_loaded) {};

        /**
         * \brief Is the asset ready ?
         */
        bool loaded() const { return *m_loaded; }

        /**
         * \brief Returns the unerlying pointer to the asset data
         */
        std::shared_ptr<DataType> handle() { return m_handle; };

        /** \brief Equality operator */
        bool operator==(const SelfType &other)
        {
            return m_handle == other.m_handle;
        }

        /** \brief Reference access operator for the data linked to the asset handle */
        DataType &operator*()
        {
            return std::ref(*m_handle);
        }

        /** \brief Reference access operator for the data linked to the asset handle */
        DataType *operator->()
        {
            return m_handle;
        }
    };

}