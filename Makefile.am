dist_doc_DATA = readme.md

lib_LIBRARIES = libades.a
include_HEADERS = inc/ades.h inc/ds.h
libades_a_SOURCES = inc/globals.h src/ades/globals.c src/ades/mt.c src/ades/ps.c src/ades/sp.c src/ades/sx.c src/ades/tv.c src/ades/pt.c src/ades/ts.c src/ades/xs.c
libades_a_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)

bin_PROGRAMS = px pv xp xv

px_SOURCES = src/px/px.c
px_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)
px_LDFLAGS = $(LIBXML2_LIBS)
px_LDADD = libades.a

pv_SOURCES = src/pv/pv.c
pv_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)
pv_LDFLAGS = $(LIBXML2_LIBS)
pv_LDADD = libades.a

xp_SOURCES = src/xp/xp.c
xp_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)
xp_LDFLAGS = $(LIBXML2_LIBS)
xp_LDADD = libades.a

xv_SOURCES = src/xv/xv.c
xv_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)
xv_LDFLAGS = $(LIBXML2_LIBS)
xv_LDADD = libades.a

# indent options in .indent.pro
indent:
	indent inc/*.h
	rm inc/*.h~
	indent src/ades/*.c
	rm src/ades/*.c~
	indent src/px/*.c
	rm src/px/*.c~
	indent src/pv/*.c
	rm src/pv/*.c~
	indent src/xv/*.c
	rm src/xv/*.c~
	indent src/xp/*.c
	rm src/xp/*.c~
