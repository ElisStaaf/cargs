LIBNAME          := cargs
ifeq ($(LIBNAME),)
$(error LIBNAME was not set)
endif

CC               ?= gcc
CXX              ?= g++
AR               ?= ar
CFLAGS           := -MMD -MP -Iinclude
CXXFLAGS         := -MMD -MP -Iinclude
LDFLAGS          := 

C_RELEASEFLAGS   := -O2
C_DEBUGFLAGS     := -g
CXX_RELEASEFLAGS := -O2
CXX_DEBUGFLAGS   := -g

CWARNINGS        := -Wall -Wextra
CFLAGS           += $(CWARNINGS)

CXXWARNINGS      := -Wall -Wextra
CXXFLAGS         += $(CXXWARNINGS)

EXTRA_CFLAGS     ?=
EXTRA_CXXFLAGS   ?= 
EXTRA_LDFLAGS    ?=

CFLAGS           += $(EXTRA_CFLAGS)
CXXFLAGS         += $(EXTRA_CXXFLAGS)
LDFLAGS          += $(EXTRA_LDFLAGS)

BUILDDIR         := build
INCLUDEDIR       := include
SRCDIR           := src

SRC              := \
$(SRCDIR)/cargs.c   \
$(SRCDIR)/cargs.cpp

HEADERS          :=     \
$(INCLUDEDIR)/cargs.h   \
$(INCLUDEDIR)/cargs.hpp \

OBJS_SUFFIX      := 
IS_RELEASE       := 
ifeq ($(filter release, $(MAKECMDGOALS)), release)
	CFLAGS       += $(C_RELEASEFLAGS)
	IS_RELEASE   := 1
	OBJS_SUFFIX  := release
else
	IS_RELEASE   := 0
	CFLAGS       += $(C_DEBUGFLAGS)
	OBJS_SUFFIX  := debug
endif

CDEPS             := $(SRC:%.c=%_$(OBJS_SUFFIX).c.d)
CDEPS             := $(patsubst %.cpp,,$(CDEPS))
CDEPS             := $(CDEPS:$(SRCDIR)/%=%)
CDEPS             := $(addprefix $(BUILDDIR)/, $(CDEPS))

CXXDEPS             := $(SRC:%.cpp=%_$(OBJS_SUFFIX).cpp.d)
CXXDEPS             := $(patsubst %.c,,$(CXXDEPS))
CXXDEPS             := $(CXXDEPS:$(SRCDIR)/%=%)
CXXDEPS             := $(addprefix $(BUILDDIR)/, $(CXXDEPS))

COBJS             := $(SRC:%.c=%_$(OBJS_SUFFIX).c.o) 
COBJS             := $(patsubst %.cpp,,$(COBJS))
COBJS             := $(COBJS:$(SRCDIR)/%=%)
COBJS             := $(addprefix $(BUILDDIR)/, $(COBJS))

CXXOBJS           := $(SRC:%.cpp=%_$(OBJS_SUFFIX).cpp.o)
CXXOBJS           := $(patsubst %.c,,$(CXXOBJS))
CXXOBJS           := $(CXXOBJS:$(SRCDIR)/%=%)
CXXOBJS           := $(addprefix $(BUILDDIR)/, $(CXXOBJS))

OUT_LIBSTATIC_PATH := $(BUILDDIR)/lib$(LIBNAME).a
OUT_LIBSHARED_PATH := $(BUILDDIR)/lib$(LIBNAME).so

all: debug

debug: $(COBJS) $(CXXOBJS) Makefile
	@# ar doesn't replace archive members sometimes.. ? 
	@# I think it skips them when the new ones are smaller than the old ones???
	@# so I'm deleting the existing one before re-ar-ing.
	rm -f $(OUT_LIBSTATIC_PATH)
	$(AR) rcs $(OUT_LIBSTATIC_PATH) $(COBJS) $(CXXOBJS)
	$(CXX) -shared $(COBJS) $(CXXOBJS) $(LDFLAGS) -o $(OUT_LIBSHARED_PATH)

# release does the same thing as debug, only some CFLAGS and suffixes change.
release: debug

-include $(CDEPS)
-include $(CXXDEPS)

$(BUILDDIR)/%_$(OBJS_SUFFIX).c.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $< -c $(CFLAGS) $(LDFLAGS) -o $@

$(BUILDDIR)/%_$(OBJS_SUFFIX).cpp.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $< -c $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	find $(BUILDDIR) \( -name '*.cpp.o' -o -name '*.cpp.d' -o -name '*.c.o' -o -name '*.c.d' \) -type f -delete || true

mrclean: clean
	rm -f $(BUILDDIR)/*.so $(BUILDDIR)/*.a
	find $(BUILDDIR) -name '*' -type d -delete || true

tests: $(TESTS)
	$(MAKE)
	$(CC) tests/test.c -g -Wall -Wextra -Iinclude -Lbuild -l:libcargs.a -o tests/test.out
	tests/test.out --help --uint 2976579765 --string itiomorbovina
	tests/test.out -h -u 2976579765 -s itiomorbovina
	@rm -f tests/test.out

install:
	$(MAKE) release
	cp $(HEADERS) /usr/include/
	cp $(OUT_LIBSHARED_PATH) /usr/lib

uninstall:
	rm -f $(addprefix /usr/include/$(LIBNAME)/, $(notdir $(HEADERS)))
	rmdir /usr/include/$(LIBNAME) || true
	rm -f /usr/lib/$(notdir $(OUT_LIBSHARED_PATH))

.PHONY: all debug release clean mrclean install uninstall tests
