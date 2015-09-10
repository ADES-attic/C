# px

PSV to XML direct, as described in meta/explore.md.

Status, 10 Sep 2015:  Simplistic Makefile replaced with standard ./configure
make, make install sequence.  My first try at autotools, so certainly
simplistic as well, possibly wrong, but seemed to work.

I think the theory is the commit configure should be good.  To rebuild it
though the magic spell is

    autoreconf --install
