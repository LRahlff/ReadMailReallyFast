CFLAGS += -march=native -masm=intel -g -Og -fsanitize=address,signed-integer-overflow,undefined -pedantic -flto -Wall -Wextra -Werror -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict -Wnull-dereference -Wdouble-promotion -Wshadow -Wformat=2 -Wfloat-equal -Wundef -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Winit-self -fno-strict-aliasing -Wno-unknown-warning-option
CXXFLAGS += ${CFLAGS} -std=c++11 -std=c++17 -Wuseless-cast -Weffc++
LFLAGS += -flto
DEPFLAGS = -MT $@ -MMD -MP -MF $(patsubst ${OBJDIR}/%.o,${DEPDIR}/%.d,$@)

CC ?= gcc
CXX ?= g++

MKDIR ?= mkdir -p

INSTALL ?= install
prefix ?= /usr

SRCDIR ?= src
APPDIR ?= ${SRCDIR}/app

BINDIR ?= bin
DEPDIR ?= dep
OBJDIR ?= obj

SOURCES := $(wildcard ${SRCDIR}/*.cpp) $(wildcard ${SRCDIR}/*.c) $(wildcard ${SRCDIR}/**/*.cpp) $(wildcard ${SRCDIR}/**/*.c)

SRCOBJS := $(patsubst ${SRCDIR}/%.c,${OBJDIR}/%.o,$(patsubst ${SRCDIR}/%.cpp,${OBJDIR}/%.o,${SOURCES}))

APPOBJS := $(filter $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%,${SRCOBJS})
OBJECTS := $(filter-out $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%,${SRCOBJS})

TARGETS := $(patsubst $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%.o,${BINDIR}/%,${APPOBJS})

CFLAGS += -I${SRCDIR}

MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

.PHONY: all clean
all: ${TARGETS}

${BINDIR}/%: $(patsubst ${SRCDIR}/%,${OBJDIR}/%,${APPDIR})/%.o ${OBJECTS}
	${MKDIR} ${@D} && ${CXX} ${CXXFLAGS} ${LFLAGS} -o $@ $< ${OBJECTS} && touch $@

${OBJDIR}/%.o: ${SRCDIR}/%.cpp ${DEPDIR}/%.d
	${MKDIR} ${@D} && ${MKDIR} $(patsubst ${OBJDIR}/%,${DEPDIR}/%,${@D}) && ${CXX} ${CXXFLAGS} ${DEPFLAGS} ${LFLAGS} -o $@ -c $< && touch $@

${OBJDIR}/%.o: ${SRCDIR}/%.c ${DEPDIR}/%.d
	${MKDIR} ${@D} && ${MKDIR} $(patsubst ${OBJDIR}/%,${DEPDIR}/%,${@D}) && ${CC} ${CFLAGS} ${DEPFLAGS} ${LFLAGS} -o $@ -c $< && touch $@


${DEPDIR}/%.d: ;
.PRECIOUS: ${DEPDIR}/%.d ${OBJDIR}/%.o

include $(wildcard $(patsubst ${OBJDIR}/%.o,${DEPDIR}/%.d,${SRCOBJS}))

clean:
	rm -rf ${BINDIR}
	rm -rf ${OBJDIR}
	rm -rf ${DEPDIR}

style:
	-astyle --mode=c --options=none --recursive -s4 -f -j -k1 -W3 -p -U -H ${SRCDIR}/*.c
	-astyle --mode=c --options=none --recursive -s4 -f -j -k1 -W3 -p -U -H ${SRCDIR}/*.h
	-astyle --mode=c --options=none --recursive -s4 -f -j -k1 -W3 -p -U -H ${SRCDIR}/*.cpp
	-astyle --mode=c --options=none --recursive -s4 -f -j -k1 -W3 -p -U -H ${SRCDIR}/*.hpp

install:
	${INSTALL} -D -o root -g root -m 700 -t ${DESTDIR}${prefix}/bin ${TARGETS}
