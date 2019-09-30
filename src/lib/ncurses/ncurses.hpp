#pragma once

#ifndef NCURSES_NOMACROS
#define NCURSES_NOMACROS
#endif

#ifdef __FreeBSD__
#include <ncurses/cursesw.h>
#else
#include <ncursesw/ncurses.h>
#endif
