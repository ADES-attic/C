# C

C language ADES library and programs.

## Version 0.1, 20 Oct 2015

* Executables to validate ADES XML and PSV files and convert between the two formats.
* An executable to convert from MPC 80 column format to XML.
* A linkable C library with callable functions.
* Configure/make related scripts.

Executables are:
* xv - validate an XML file in place.
* pv - validate a PSV file in place.
* xp - convert an XML file to a PSV file.
* px - convert a PSV file to an XML file.
* mx - convert an MPC 80 column file to an XML file.

Each executable prints a usage message when run without arguments.

See `inc/ades.h` for the library API.

## Building

The standard ./configure, make sequence should work.  Configure --prefix= and make install if you like.

The provided ./configure should work.  To rebuild it though the magic spell is

    autoreconf --install
