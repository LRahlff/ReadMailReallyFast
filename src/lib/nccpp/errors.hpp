#pragma once

#include <exception>

#include "lib/nccpp/Color.hpp"

namespace nccpp {

namespace errors {

class Base : public std::exception {
public:
    Base() noexcept = default;

    Base(const Base &) noexcept = default;
    Base &operator=(const Base &) noexcept = default;

    virtual ~Base() = default;

    char const* what() const noexcept override {
        return "nccpp::errors::Base";
    }
};

class NcursesInit : public Base {
public:
    NcursesInit() noexcept = default;
    NcursesInit(const NcursesInit &) noexcept = default;
    NcursesInit &operator=(const NcursesInit &) noexcept = default;

    virtual ~NcursesInit() = default;

    char const* what() const noexcept override {
        return "nccpp::errors::NcursesInit : Can't initialize ncuses, initscr() failed";
    }
};

class WindowInit : public Base {
public:
    WindowInit() noexcept = default;
    WindowInit(const WindowInit &) noexcept = default;
    WindowInit &operator=(const WindowInit &) noexcept = default;

    virtual ~WindowInit() = default;

    char const* what() const noexcept override    {
        return "nccpp::errors::WindowInit : Can't create new window, newwin() failed";
    }
};

class ColorInit : public Base {
public:
    ColorInit() noexcept = default;
    ColorInit(const ColorInit &) noexcept = default;
    ColorInit &operator=(const ColorInit &) noexcept = default;

    virtual ~ColorInit() = default;

    char const* what() const noexcept override    {
        return "nccpp::errors::ColorInit : Can't initialize colors, start_color() failed";
    }
};

class TooMuchColors : public Base {
public:
    TooMuchColors(const Color &c) noexcept : color {c} {}

    TooMuchColors(const TooMuchColors &) noexcept = default;
    TooMuchColors &operator=(const TooMuchColors &) noexcept = default;

    virtual ~TooMuchColors() = default;

    char const* what() const noexcept override    {
        return "nccpp::errors::TooMuchColors : Can't initialize new colors, init_pair failed";
    }

    const Color color; ///< The color that caused the error.
};

} // namespace errors

} // namespace nccpp
