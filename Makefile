CFLAGS += -march=native -masm=intel -g -Og -fsanitize=address,signed-integer-overflow,undefined -pedantic -Wall -Wextra -Werror -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict -Wnull-dereference -Wdouble-promotion -Wshadow -Wformat=2 -Wfloat-equal -Wundef -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Winit-self -fno-strict-aliasing -Wno-unknown-warning-option
CXXFLAGS += ${CFLAGS} -std=c++11 -std=c++17 -Wuseless-cast -Weffc++ -I/usr/local/include -Wno-non-virtual-dtor
DEPFLAGS = -MT $@ -MMD -MP -MF $(patsubst ${OBJDIR}/%.o,${DEPDIR}/%.d,$@)

PODOMAIN ?= rmrf
POVERSION ?= 0.1
POLANGS ?= de en
PKG_TOOL = pkg-config

OS = $(shell uname -s)
ifeq "${OS}" "Linux"
CFLAGS += -flto
CXXFLAGS += -flto
LFLAGS += -flto
else ifeq "${OS}" "FreeBSD"
CFLAGS += -Wno-error=sign-conversion
CXXFLAGS += -Wno-error=sign-conversion
PKG_TOOL = pkgconf
endif

CFLAGS += `${PKG_TOOL} --cflags libnl-3.0`
CXXFLAGS += `${PKG_TOOL} --cflags libnl-3.0`

CC ?= gcc
CXX ?= g++

MKDIR ?= mkdir -p

INSTALL ?= install
prefix ?= /usr

XGETTEXT ?= xgettext
XGETTEXT_FLAGS ?= -k_ -c -s -i --no-wrap --force-po --from-code=UTF-8 --check=ellipsis-unicode --sentence-end=single-space \
    --foreign-user --package-name=${PODOMAIN} --package-version=${POVERSION}
MSGINIT ?= msginit
MSGMERGE ?= msgmerge
MSGFMT ?= msgfmt

SED ?= sed

SRCDIR ?= src
APPDIR ?= ${SRCDIR}/app

BINDIR ?= bin
DEPDIR ?= dep
OBJDIR ?= obj

POTDIR ?= po/tpl
PODIR ?= po/lang
MODIR ?= po/bin

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard,${SRCDIR},*.cpp *.c)

SRCOBJS := $(patsubst ${SRCDIR}/%.c,${OBJDIR}/%.o,$(patsubst ${SRCDIR}/%.cpp,${OBJDIR}/%.o,${SOURCES}))

APPOBJS := $(filter $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%,${SRCOBJS})
OBJECTS := $(filter-out $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%,${SRCOBJS})

TARGETS := $(patsubst $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%.o,${BINDIR}/%,${APPOBJS})

POTSRCS := ${SOURCES} $(call rwildcard,${SRCDIR},*.hpp *.h)
POTOBJS := ${POTDIR}/${PODOMAIN}.pot
POOBJS := $(foreach POLANG,${POLANGS},$(patsubst ${POTDIR}/%.pot,${PODIR}/${POLANG}/%.po,${POTOBJS}))
MOOBJS := $(patsubst ${PODIR}/%.po,${MODIR}/%.mo,${POOBJS})

CFLAGS += -I${SRCDIR}

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

.PRECIOUS: ${DEPDIR}/%.d ${OBJDIR}/%.o ${POTOBJS} ${POOBJS}

.PHONY: all clean install lintian style translation
all: ${TARGETS} translation

${BINDIR}/%: $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%.o ${OBJECTS} Makefile ${APPDIR}/%.ldflags
	${MKDIR} ${@D} && ${CXX} ${CXXFLAGS} ${LFLAGS} -o $@ $< ${OBJECTS} $(shell [ -r $(patsubst ${OBJDIR}/%.o,${SRCDIR}/%.ldflags,$<) ] && cat $(patsubst ${OBJDIR}/%.o,${SRCDIR}/%.ldflags,$<) ) && touch $@

${OBJDIR}/%.o: ${SRCDIR}/%.cpp ${DEPDIR}/%.d Makefile
	${MKDIR} ${@D} && ${MKDIR} $(patsubst ${OBJDIR}/%,${DEPDIR}/%,${@D}) && ${CXX} ${CXXFLAGS} ${DEPFLAGS} ${LFLAGS} -o $@ -c $< && touch $@

${OBJDIR}/%.o: ${SRCDIR}/%.c ${DEPDIR}/%.d Makefile
	${MKDIR} ${@D} && ${MKDIR} $(patsubst ${OBJDIR}/%,${DEPDIR}/%,${@D}) && ${CC} -std=c11 -std=c17 ${CFLAGS} ${DEPFLAGS} ${LFLAGS} -o $@ -c $< && touch $@

${POTDIR}/${PODOMAIN}.pot: ${POTSRCS}
	${MKDIR} ${@D} && ${XGETTEXT} ${XGETTEXT_FLAGS} $( [ -r $@ ] && echo -- -j ) -o $@ $^ && \
		${SED} --expression='s/charset=CHARSET/charset=UTF-8/g' --in-place $@

define genlangporules
${PODIR}/$(1)/%.po: ${POTDIR}/%.pot
	$${MKDIR} $${@D} && ( [ ! -r $$@ ] && $${MSGINIT} --no-translator --input=$$< --locale=$$(patsubst $${PODIR}/%/$$(patsubst $${POTDIR}/%.pot,%.po,$$<),%,$$@).UTF-8 --output=$$@ ) || ( $${MSGMERGE} --update $$@ $$< ) && touch $$@
endef
$(foreach POLANG,${POLANGS},$(eval $(call genlangporules,$${POLANG})))

${MODIR}/%.mo: ${PODIR}/%.po
	${MKDIR} ${@D} && ${MSGFMT} --statistics --verbose --output-file=$@ $< && touch $@

${APPDIR}/%.ldflags: ;

${DEPDIR}/%.d: ;

include $(wildcard $(patsubst ${OBJDIR}/%.o,${DEPDIR}/%.d,${SRCOBJS}))

translation: ${MOOBJS}

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}
	rm -rf ${DEPDIR}
	rm -rf ${MODIR}

style:
	-astyle --mode=c --options=none --recursive -q -Q -s4 -f -H -j -J -k1 -W3 -p -U -xb -y ${SRCDIR}/*.c
	-astyle --mode=c --options=none --recursive -q -Q -s4 -f -H -j -J -k1 -W3 -p -U -xb -y ${SRCDIR}/*.h
	-astyle --mode=c --options=none --recursive -q -Q -s4 -f -H -j -J -k1 -W3 -p -U -xb -y ${SRCDIR}/*.cpp
	-astyle --mode=c --options=none --recursive -q -Q -s4 -f -H -j -J -k1 -W3 -p -U -xb -y ${SRCDIR}/*.hpp

lintian:
	lintian --pedantic --profile debian --verbose --display-experimental --show-overrides
	@if lintian --pedantic --profile debian --verbose --display-experimental --show-overrides 2>&1 | grep -q '^W:'; then false; fi

install:
	${INSTALL} -D -o root -g root -m 700 -t ${DESTDIR}${prefix}/bin ${TARGETS}
	for POLANG in ${POLANGS}; do \
		${INSTALL} -D -o root -g root -m 644 -t ${DESTDIR}${prefix}/share/locale/$${POLANG}/LC_MESSAGES/ ${MODIR}/$${POLANG}/*.mo; \
	done
