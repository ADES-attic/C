# C

C language ADES library and programs.

## Contents, 24 Sep 2015

* Configure/make related scripts
* pxdirect, an executable, a validating PSV to XML converter.
* A header file with C structures representing the ADES.
* ades, a C library with callable functions, currently readPSVFile and writeXMLFile
* px, an executable, a PSV to XML converter based on the ades library.

## Building

The standard ./configure, make sequence should work.  Configure --prefix= and make install if you like.

I think the theory is the committed configure should be good.  To rebuild it
though the magic spell is

    autoreconf --install
