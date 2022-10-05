#pragma once

namespace nccpp {

/**
 * \brief Class representing a ncurses color pair.
 */
struct Color {
    Color() : Color{-1, -1} {}
    Color(short f, short b) : foreground{f}, background{b} {}

    short foreground; ///< Foreground color.
    short background; ///< Background color.
};

inline bool operator==(nccpp::Color const &lhs, nccpp::Color const &rhs) {
    return (lhs.foreground == rhs.foreground) && (lhs.background == rhs.background);
}

inline bool operator!=(nccpp::Color const &lhs, nccpp::Color const &rhs) {
    return !(lhs == rhs);
}

} // namespace nccpp
