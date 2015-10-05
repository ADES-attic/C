# C

C language ADES library and programs.

## Contents, 2 Oct 2015

* Executables to validate ADES XML and PSV files and convert between the two formats.
* A linkable C library with callable functions.
* Configure/make related scripts.

Executables are:
* xv - validate an XML file in place.
* pv - validate a PSV file in place.
* xp - convert an XML file to a PSV file.
* px - convert a PSV file to an XML file.

Each executable prints a usage message when run without arguments.

See `inc/ades.h` for the library API.  See `inc/ds.h` for data structures used by the library.

## Building

The standard ./configure, make sequence should work.  Configure --prefix= and make install if you like.

The provided ./configure should work.  To rebuild it though the magic spell is

    autoreconf --install
