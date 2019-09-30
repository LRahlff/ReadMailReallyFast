#pragma once

#include <libintl.h>
#include <locale.h>

#define _(str) gettext(str)
#define N_(str) gettext_nop(str)
