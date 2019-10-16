#pragma once

#include <Defines.hpp>

namespace dv::util {
    namespace impl{
        template<typename T, bool CopyValue = !std::is_lvalue_reference_v<T>>
        struct ContainerBase;

        template<typename T>
        struct ContainerBase<T, false>
        {
            T& container;
            explicit ContainerBase(T container) : container{container} {}
        };

        template<typename T>
        struct ContainerBase<T, true>
        {
            const T container;
            explicit ContainerBase(T container) : container{std::move(container)} {}
        };
    }

    template<typename Container>
    class Reverse : impl::ContainerBase<Container>
    {
        using Base = impl::ContainerBase<Container>;
    public:
        explicit Reverse(Container&& container) : Base{std::forward<Container>(container)} {}

        auto begin() { return std::rbegin(Base::container); }
        auto end() { return std::rend(Base::container); }
    };

    template<typename Container>
    explicit Reverse(Container&& container) -> Reverse<Container>;
}