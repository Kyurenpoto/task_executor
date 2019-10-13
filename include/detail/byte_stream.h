#include <cstddef>
#include <type_traits>

namespace task_executor
{
    template<std::size_t N, class T, class... Ts>
    struct size_sum
    {
        static_assert(N < 1 + sizeof...(Ts));

        static constexpr std::size_t value = sizeof(T) + size_sum<N - 1, Ts...>::value;
    };

    template<class T, class... Ts>
    struct size_sum<0, T, Ts...>
    {
        static constexpr std::size_t value = sizeof(T);
    };

    template<std::size_t N, class... Ts>
    constexpr std::size_t size_sum_v = size_sum<N, Ts...>::value;

    template<std::size_t N, class T, class... Ts>
    struct nth_type
    {
        static_assert(N < 1 + sizeof...(Ts));

        using type = typename nth_type<N - 1, Ts...>::type;
    };

    template<class T, class... Ts>
    struct nth_type<0, T, Ts...>
    {
        using type = T;
    };

    template<std::size_t N, class... Ts>
    using nth_type_t = typename nth_type<N, Ts...>::type;

    template<class... Ts>
    struct byte_stream
    {
        static_assert(sizeof...(Ts) > 0);

        byte_stream() = default;

        template<std::size_t N>
        nth_type_t<N, Ts...>& get()
        {
            if constexpr (N == 0)
            {
                return *reinterpret_cast<nth_type_t<0, Ts...>*>(stream);
            }
            else
            {
                return *reinterpret_cast<nth_type_t<N, Ts...>*>(stream + size_sum_v<N - 1, Ts...>);
            }
        }

        template<std::size_t N>
        const nth_type_t<N, Ts...>& get() const
        {
            if constexpr (N == 0)
            {
                return *reinterpret_cast<const nth_type_t<0, Ts...>*>(stream);
            }
            else
            {
                return *reinterpret_cast<const nth_type_t<N, Ts...>*>(stream + size_sum_v<N - 1, Ts...>);
            }
        }

    private:
        std::byte stream[size_sum_v<sizeof...(Ts) - 1, Ts...>];
    };
}
