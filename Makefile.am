dist_doc_DATA = readme.md
bin_PROGRAMS = px
px_SOURCES = src/px/px.c
px_CFLAGS = -std=c99 $(LIBXML2_CFLAGS)
px_LDFLAGS = $(LIBXML2_LIBS)

# indent options in .indent.pro
indent:
	indent src/px/*.c
	rm src/px/*.c~
	indent inc/*.h
	rm inc/*.h~
