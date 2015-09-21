dist_doc_DATA = readme.md

lib_LIBRARIES = libades.a
libades_a_SOURCES = src/ades/ps.c src/ades/sx.c src/ades/tables.c
libades_a_CFLAGS = -std=c99 -Iinc $(LIBXML2_CFLAGS)

bin_PROGRAMS = pxdirect
pxdirect_SOURCES = src/pxdirect/pxdirect.c
pxdirect_CFLAGS = -std=c99 $(LIBXML2_CFLAGS)
pxdirect_LDFLAGS = $(LIBXML2_LIBS)

# indent options in .indent.pro
indent:
	indent inc/*.h
	rm inc/*.h~
	indent src/ades/*.c
	rm src/ades/*.c~
	indent src/pxdirect/*.c
	rm src/pxdirect/*.c~
