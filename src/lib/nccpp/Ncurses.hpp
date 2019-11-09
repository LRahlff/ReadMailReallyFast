#pragma once

#include <vector>

#include "lib/ncurses/ncurses.hpp"

#include "lib/nccpp/Color.hpp"

namespace nccpp {
    class Ncurses;
}

#include "lib/nccpp/Window.hpp"

namespace nccpp{

class Ncurses : public Window{
    friend Ncurses &ncurses();
public:
    Ncurses(const Ncurses &) = delete;
    Ncurses(Ncurses &&) = delete;
    Ncurses &operator=(const Ncurses &) = delete;
    Ncurses &operator=(Ncurses &&) = delete;
    ~Ncurses();

    void exit_ncurses_mode();
    void resume_ncurses_mode();

    // Input options

    int cbreak(bool);
    int echo(bool);
    int halfdelay(int);
    int intrflush(bool);
    int meta(bool);
    int raw(bool);
    void qiflush(bool);
    int typeahead(int);

    // Output options

    int clearok(bool, bool = false);
    int idlok(bool);
    void idcok(bool);
    void immedok(bool);
    int leaveok(bool);
    int scrollok(bool);
    int nl(bool);

    // Input functions

    int ungetch(int);
    int has_key(int);

    // Misc

    int doupdate();
    int line_count();
    int column_count();

    // Mouse

    bool has_mouse();
    int getmouse(MEVENT &);
    int ungetmouse(MEVENT &);
    mmask_t mousemask(mmask_t, mmask_t* = nullptr);
    int mouseinterval(int);

    // Window

    /// \cond NODOC
    WINDOW* newwin_(int, int, int, int, Window::Key);

#ifndef NDEBUG
    void register_window_(Window &, Window::Key);
    void unregister_window_(Window &, Window::Key);
#endif

    // Color

    void start_color();
    int use_default_colors();

    short color_to_pair_number(Color const &);
    attr_t color_to_attr(Color const &);
    Color pair_number_to_color(short);
    Color attr_to_color(attr_t);

    int init_color(short, short, short, short);

private:
    Ncurses();

    std::vector<Color> registered_colors_;

#ifndef NDEBUG
    std::vector<Window*> windows_;
    bool is_exit_;
#endif

    bool colors_initialized_;

    void assign(WINDOW*) override;
    void destroy() override;
};

/**
 * Access the Ncurses singleton.
 *
 * \exception errors::NcursesInit Thrown when ncurses can't be initialized.
 * \return A reference to the singleton.
 */
inline Ncurses &ncurses()
{
    static Ncurses nc{};
    return nc;
}

} // namespace nccpp
