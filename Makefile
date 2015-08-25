px: px.c
	gcc -o px `xml2-config --cflags` px.c `xml2-config --libs`

# indent options in .indent.pro
indent:
	indent *.c
	rm *.c~
