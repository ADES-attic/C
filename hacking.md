# Hacking
Development notes

## Architecture
```
                                |---------
                                |  (tv)  |      (libxml2 xmlParseFile
                     (mt)       v        |       for example)
              ---------------> libxml2 tree <--------
             /    -----------> in memory             \
MPC 80 column    /    (pt)        |                   \
disk file       /                 |                    \
               /                  |                     \
              /                   |(ts)                  \
PSV format --/                    |                       \--- ADES XML
disk file <---                    |                       ---> disk file
              \                   |                      /
               \                  v                     /
                \  (sp)    observationBatch    (sx)    /
                 \----------- C struct ---------------/
```

Notes in parentheses represent some process.  The two letter mnemonics:

* mt: (M)PC 80 column to libxml2 (t)ree
* pt: (P)SV to (t)ree
* sp: C (s)truct to (P)SV
* tv: (t)ree (v)alidate
* ts: (t)ree to (s)truct
* sx: (s)struct to (X)ML

mt, pt, tv, ts are functions in simarly named files in src/ades.
There are files sp.c and sx.c in src/ades but no functions actually have those
names.  There is no xt because functions for this exist in suitable form
in the libxml2 library.  Also in src/ades there are files ms.c, ps.c, and
xs.c which compose functions from the architecture diagram.

The public API consists of the C struct definition and various functions that
use it:

* readMPC80File in file ms.c
* readPSVFile in file ps.c
* readXMLFile in file xs.c
* writePSVFile in file sp.c
* writeXMLFile in file sx.c

The libxml2 tree struct is not considered part of the public API.
The principle is to facilitate use of the public API from other programming
languages by minimizing the "API surface."  libxml2 has a large API surface.
The surface of the observationBatch data structure in inc/ds.h is
comparatively small and uses the field names of the ADES.

## Why not tx?
Well, it could be done.  Good reasons would be to avoid the overhead of
creating the C struct or to use libxml2 (well tested) XML write functions.
It's not as simple as just calling the libxml2 functions though because ADES
specifies an ordering of observation fields.  Elements of the tree would have
to be sorted somehow before writing.  The ts and sx functions exist though
and represent an easy path to ADES-ordered XML output.

## --version
The stand-alone programs have -v output that includes source information.
For example,

```
$ ./mx -v
mx -- MPC 80 column to XML converter -- Public domain.
Version 0.1 2015-10-21 19:29:23 +0000
Source: https://github.com/IAU-ADES/C
Branch: master
Commit: a4441c99cb2a38da3589d89d3b5dce0c5ae654d6 + local changes
Compiled: Oct 23 2015.
```

The version number is the package version number in `configure.ac`.
Run `autoreconf --install` after changing the version number.
This generates the C header file `config.h`.

Source infomation is queried from git with the Python script `commit.py`
and packaged into the C header file `commit.h`.  This happens automatically
with `make`.  A tricky line in `Makefile.am` reads something like

    commit.h: $(shell ls .git/index)
            ./commit.py

This will run commit.py if it sees the git history change.  If the git history
is not even present, for example when building from a tarball, the make rule
sees no dependencies and so does not attempt to run commit.py.  In fact
commit.py is not even included in the tarball.

## Source repository
https://github.com/IAU-ADES/C

Feel free to file issues as well as submit pull requests.

## Source tree organization.
Currently there are just the subdirectories `inc` and `src`.  Within src there
is a directory for the library, `ades`, and a separate directory for each
executable.

In the top source directory is also the Python script `commit.py`, the
built headers `config.h` and `commit.h`, and all the various scripts and
files used by autoconf, automake, libtool and related programs.

## Code style
`make indent` runs GNU indent with the profile `.indent.pro`.

## Make dist
`make dist` should make a tarball.  `make distcheck` should validate it.

After unpacking the tarball, the usual `./confgure` `make` `make install`
sequence should build and install the executables and libraries.  Additionally
the tarball should include these data files which are used by the MPC 80
reader.

* program_codes.txt
* catalogs.csv
* telescopes.csv

Installed headers are `ades.h` and `ds.h` which contain definitions needed
for the public API.  `inc/globals.h` contains definitions not considered
part of the public API and so is not installed.
