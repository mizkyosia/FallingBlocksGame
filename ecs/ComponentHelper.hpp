#pragma once
#include "Global.hpp"

#include <bitset>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <iostream>
#include <memory>
#include <vector>
#include <array>

namespace traits
{
    template <typename... Ts>
    struct Wrapper
    {
        static ArchetypePtr buildArchetype(World &world)
        {
            auto arch = std::make_shared<Archetype<Ts...>>(world);
            std::cout << "Building archetype : " << __PRETTY_FUNCTION__ << std::endl;
            return arch;
        };
    };

    // Type list
    template <typename... Ts>
    struct TypeList
    {
    };

    // Filter metafunction
    template <typename List, size_t Bitset, std::size_t Index = 0>
    struct FilterTypes;

    template <size_t Bitset, std::size_t Index>
    struct FilterTypes<TypeList<>, Bitset, Index>
    {
        using type = TypeList<>;
    };

    template <typename Head, typename... Tail, size_t Bitset, std::size_t Index>
    struct FilterTypes<TypeList<Head, Tail...>, Bitset, Index>
    {
    private:
        using Rest = typename FilterTypes<TypeList<Tail...>, Bitset, Index + 1>::type;

    public:
        using type = std::conditional_t<
            1 & (Bitset >> Index),
            decltype(std::tuple_cat(std::declval<TypeList<Head>>(), std::declval<Rest>())),
            Rest>;
    };

    // Unpack TypeList into a parameter pack
    template <typename TL>
    struct Unpack;

    template <typename... Ts>
    struct Unpack<TypeList<Ts...>>
    {
        using type = Wrapper<Ts...>;
    };

    template <typename... AllTypes>
    struct WrapperFactory
    {
        using List = TypeList<AllTypes...>;
        static constexpr size_t NumTypes = sizeof...(AllTypes);

        using WrapperBase = std::function<void()>;

        static ArchetypePtr generate(std::bitset<32> bits, World& world)
        {
            // Loop through all possible combinations
            for (size_t i = 0; i < (1 << NumTypes); ++i)
            {
                std::bitset<32> b(i);
                if (b == bits)
                {
                    return call<i>(world);
                }
            }
        }

        template <size_t bits>
        static ArchetypePtr call(World& world)
        {
            using Filtered = typename FilterTypes<List, bits, 0>::type;
            using Type = typename Unpack<Filtered>::type;

            return Type::buildArchetype(world);
        }
    };

}

struct IComponentHelper
{
    virtual ArchetypePtr buildArchetype(const Signature &sig, World &world) = 0;
};

template <typename... Cs>
struct ComponentHelper : IComponentHelper
{
    ArchetypePtr buildArchetype(const Signature &sig, World &world) override
    {
        auto i = sig.to_ulong();
        std::cout << "Signature to long : " << i << std::endl;

        return traits::WrapperFactory<Cs...>::generate(sig, world);
    }
};