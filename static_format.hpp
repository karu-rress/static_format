/**
*
* static_format.hpp
*
* Copyright 2024 Sunwoo Na.
* All rights reserved.
*
*
* consteval implementation of C++20 std::format.
*
* Usage:
*
*    sf::static_format("{}, format!", "Hello")
*
*
* Last update: 2024/04/18
*
*/

#ifndef STATIC_FORMAT_HPP
#define STATIC_FORMAT_HPP

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <string>

namespace sf {
    // arithmetic concept (integral || floating_point)
    template<typename T>
    concept arithmetic = std::is_arithmetic_v<T>;

    constexpr size_t max_len = 1000;

    template<std::size_t N>
    class static_string {
    public:
        consteval static_string() : str{} {  }
        consteval static_string(const std::string_view& s): static_string(s, N) {  }
        consteval static_string(const std::string_view& s, std::size_t size): length{size} {
            std::ranges::copy(s, std::begin(str));
        }

        // implicit operator to std::string_view
        constexpr operator std::string_view() const { return to_string_view(); }
        constexpr char& operator[](std::size_t i) { return str[i]; }

        template<std::size_t M>
        constexpr static_string<N + M> operator+(const static_string<M>& rhs) const {
            static_string<N + M> ret;

            std::copy_n(cbegin(), size() - 1, ret.begin());
            std::copy_n(rhs.cbegin(), rhs.size(), ret.begin() + size() - 1);
            return ret;
        }

        constexpr auto begin() { return str.begin(); }
        constexpr auto cbegin() const { return str.cbegin(); }

        constexpr std::size_t size() const { return length; }
        constexpr void set_size(std::size_t s) { length = s; }

        constexpr std::string_view to_string_view() const {
            return std::string_view(str.data(), size());
        }
        constexpr const char* c_str() const { return str.data(); }

    private:
        std::array<char, N> str;
        std::size_t length = N;
    };

    template<std::size_t N>
    std::ostream& operator<<(std::ostream& os, const static_string<N>& str) {
        return os << str.to_string_view();
    }

    template<std::size_t N>
    consteval auto new_static_string(char const (&str)[N]) {
        static_string<N - 1> ret;
        std::copy_n(str, N, ret.begin());
        return ret;
    }

    template<std::integral T>
    consteval auto to_static_string(T n) {
        static_string<std::numeric_limits<T>::digits10 + 1> ret;
        std::size_t pos{};
        T cn{n};

        do {
            ret[pos++] = abs(cn % 10) + static_cast<T>('0');
            cn /= 10;
        } while (cn != T{0});

        if (n < T{0})
            ret[pos++] = '-';

        ret[pos] = '\0';
        ret.set_size(pos);

        std::reverse(ret.begin(), ret.begin() + ret.size());
        return ret;
    }

    // TODO: Improve this floating-point to string coversion
    template<std::floating_point T>
    consteval static_string<max_len> to_static_string(T n) {
        constexpr auto abs = [](auto x) { return x < 0 ? -x : x; };

        long long n_int = static_cast<long long>(n);
        T n_frac = abs(n) - static_cast<T>(abs(n_int));

        std::string buf{to_static_string(n_int)};

        // If n_frac doesn't have precision
        if (n_frac < 0.000001)
            return {buf, buf.length()};

        static_string<6> ret;
        for (std::size_t i{}; i < ret.size(); i++) {
            n_frac *= 10;
            ret[i] = static_cast<char>(n_frac) + '0';
            if (n_frac -= static_cast<int>(n_frac); n_frac < 0.000001)
                break;
        }
        buf += std::string{"."};
        buf += ret;

        return {buf, buf.length()};
    }

    template<typename T>
    consteval auto static_strcat(T&& arg) {
        if constexpr (arithmetic<std::remove_cvref_t<T>>)
            return static_strcat(std::forward<T>(arg));

        else
            return new_static_string(std::forward<T>(arg));
    }

    template<typename... Args>
    [[deprecated("Use static_format() instead.")]]
    consteval auto static_strcat(Args&&... args) {
        return (static_strcat(std::forward<Args>(args)) + ...);
    }

    template<typename T>
    consteval void format_builder(std::string& buf, std::string_view& fmt, T&& arg) {
        constexpr std::size_t N{std::string::npos};
        std::size_t start{}, end{};

        // Find the first '{' and '}'
        if (start = fmt.find('{'); start == N)
            return;

        if (end = fmt.find('}', start + 1); end == N)
            return;

        buf += std::string{fmt.substr(0, start)};

        // If arithmetic, append the converted string
        if constexpr (arithmetic<std::remove_cvref_t<T>>)
            buf += std::string{to_static_string(arg)};
        // Otherwise, append the string
        else
            buf += std::string{arg};

        fmt = fmt.substr(end + 1);
    }

    template<typename... Args>
    [[nodiscard("Formatted string shouldn't be discarded.")]]
    consteval auto static_format(std::string_view fmt, Args&&... args) -> static_string<max_len> {
        // buf should be destructed before returned
        // (constant expression of dynamic stroage)
        std::string buf;

        // C++17 Fold expression
        (format_builder(buf, fmt, std::forward<Args>(args)), ...);
        buf += fmt; // Concatenate the remaining string

        return {buf, buf.length()};
    }
}

#endif // STATIC_FORMAT_HPP