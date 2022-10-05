#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lib/ncurses/ncurses.hpp"

#include "lib/nccpp/Color.hpp"

namespace nccpp {

/** \brief Alias used for input functions. */
using String = std::basic_string<chtype>;

class Ncurses;
class Subwindow;

class Window {
public:
    explicit Window(WINDOW*);
    Window(int, int, int, int);

    Window(Window const &);
    Window &operator=(Window const &);

    Window(Window &&)
#ifdef NDEBUG
    noexcept
#endif
    ;

    Window &operator=(Window &&) noexcept;

    Window(const Ncurses &) = delete;
    Window(Ncurses &&) = delete;
    Window &operator=(const Ncurses &) = delete;
    Window &operator=(Ncurses &&) = delete;

    Window(const Subwindow &) = delete;
    Window(Subwindow &&) = delete;
    Window &operator=(const Subwindow &) = delete;
    Window &operator=(Subwindow &&) = delete;

    ~Window();

    virtual void assign(WINDOW*);
    virtual void destroy();
    WINDOW* get_handle();
    WINDOW const* get_handle() const;

    std::size_t add_subwindow(int, int, int, int);
    Subwindow &get_subwindow(std::size_t);
    void delete_subwindow(std::size_t);

    // Input options

    int keypad(bool);
    int nodelay(bool);
    int notimeout(bool);
    void timeout(int);

    // Output options

    int clearok(bool);
    int setscrreg(int, int);

    // Input functions

    int getch();
    int mvgetch(int, int);

    int scanw(char const*, ...);
    int mvscanw(int, int, char const*, ...);

    int getstr(std::string &);
    int getnstr(std::string &, std::size_t);
    int mvgetstr(int, int, std::string &);
    int mvgetnstr(int, int, std::string &, std::size_t);

    chtype inch();
    chtype mvinch(int, int);

    int instr(std::string &);
    int innstr(std::string &, std::size_t);
    int mvinstr(int, int, std::string &);
    int mvinnstr(int, int, std::string &, std::size_t);

    int inchstr(String &);
    int inchnstr(String &, std::size_t);
    int mvinchstr(int, int, String &);
    int mvinchnstr(int, int, String &, std::size_t);

    // Output functions

    int addch(chtype const);
    int mvaddch(int, int, chtype const);
    int echochar(chtype const);

    int printw(char const*, ...);
    int mvprintw(int, int, char const*, ...);

    int addstr(std::string const &);
    int addnstr(std::string const &, std::size_t);
    int mvaddstr(int, int, std::string const &);
    int mvaddnstr(int, int, std::string const &, std::size_t);

    int addchstr(String const &);
    int addchnstr(String const &, std::size_t);
    int mvaddchstr(int, int, String const &);
    int mvaddchnstr(int, int, String const &, std::size_t);

    int insch(chtype);
    int mvinsch(int y, int x, chtype);

    int insstr(std::string const &);
    int insnstr(std::string const &, std::size_t);
    int mvinsstr(int, int, std::string const &);
    int mvinsnstr(int, int, std::string const &, std::size_t);

    // Deletion functions

    int delch();
    int mvdelch(int, int);

    int insdelln(int);

    // Border

    int border(chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype);
    int box(chtype, chtype);

    int hline(chtype, int);
    int vline(chtype, int);
    int mvhline(int, int, chtype, int);
    int mvvline(int, int, chtype, int);

    // Background

    void bkgdset(int);
    int bkgd(int);
    chtype getbkgd();

    // Attributes

    int attroff(int);
    int attron(int);
    int attrset(int);

    int attr_get(attr_t &);
    int color_get(Color &);

    int attr_color_get(attr_t &, Color &);

    int chgat(int, attr_t, Color);
    int mvchgat(int, int, int, attr_t, Color);

    // Misc

    int move(int, int);
    int mvwin(int, int);

    int erase();
    int clear();
    int clrtobot();
    int clrtoeol();

    int refresh();
    int outrefresh();
    int redraw();
    int redrawln(int, int);

    int scroll(int = 1);

    void get_yx(int &, int &);
    void get_begyx(int &, int &);
    void get_maxyx(int &, int &);

    int touchln(int, int, bool);

    bool enclose(int, int);
    bool coord_trafo(int &, int &, bool);

protected:
    struct Key {};

    WINDOW* win_;

#ifndef NDEBUG
    WINDOW* win_save_;

public:
    void invalidate_for_exit_(Key);
    void validate_for_resume_(Key);
#endif

private:
    std::vector<Subwindow> subwindows_;
};

} // namespace nccpp
