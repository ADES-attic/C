# Automake input file
# See https://www.gnu.org/software/automake/manual/

# libtool likes this
ACLOCAL_AMFLAGS = -I m4

# See explanation at https://www.gnu.org/software/automake/manual/automake.html#amhello_0027s-Makefile_002eam-Setup-Explained.
dist_doc_DATA = readme.md catalogs.csv program_codes.txt telescopes.csv

# rule for extracting git version information.
# see hacking.md in this directory.
BUILT_SOURCES = $(top_builddir)/commit.h
$(top_builddir)/commit.h: $(shell ls .git/index)
	./commit.py

# ades library API headers, included in tarball
include_HEADERS = inc/ades.h inc/ds.h

# build ades library
lib_LTLIBRARIES = libades.la
libades_la_SOURCES = inc/globals.h src/ades/globals.c src/ades/ms.c src/ades/mt.c src/ades/ps.c src/ades/sp.c src/ades/sx.c src/ades/tv.c src/ades/pt.c src/ades/ts.c src/ades/xs.c
libades_la_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
libades_la_LIBADD = $(LIBXML2_LIBS)
libades_la_LDFLAGS = -version-info 0:0:0

# build stand-alone executables
bin_PROGRAMS = mx px pv xp xv

mx_SOURCES = src/mx/mx.c commit.h
mx_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
mx_LDADD = libades.la

px_SOURCES = src/px/px.c commit.h
px_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
px_LDADD = libades.la

pv_SOURCES = src/pv/pv.c commit.h
pv_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
pv_LDADD = libades.la

xp_SOURCES = src/xp/xp.c commit.h
xp_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
xp_LDADD = libades.la

xv_SOURCES = src/xv/xv.c commit.h
xv_CFLAGS = -std=c99 -I$(top_srcdir)/inc $(LIBXML2_CFLAGS)
xv_LDADD = libades.la

# indent options in .indent.pro
indent:
	indent inc/*.h
	rm inc/*.h~
	indent src/ades/*.c
	rm src/ades/*.c~
	indent src/mx/*.c
	rm src/mx/*.c~
	indent src/px/*.c
	rm src/px/*.c~
	indent src/pv/*.c
	rm src/pv/*.c~
	indent src/xv/*.c
	rm src/xv/*.c~
	indent src/xp/*.c
	rm src/xp/*.c~
