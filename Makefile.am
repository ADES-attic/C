dist_doc_DATA = readme.md

lib_LIBRARIES = libades.a
libades_a_SOURCES = src/ades/ps.c src/ades/tables.c
libades_a_CFLAGS = -std=c99 -Iinc

bin_PROGRAMS = px
px_SOURCES = src/px/px.c
px_CFLAGS = -std=c99 $(LIBXML2_CFLAGS)
px_LDFLAGS = $(LIBXML2_LIBS)

# indent options in .indent.pro
indent:
	indent inc/*.h
	rm inc/*.h~
	indent src/ades/*.c
	rm src/ades/*.c~
	indent src/px/*.c
	rm src/px/*.c~
