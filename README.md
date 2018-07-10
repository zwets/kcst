# kcst - kmer-counting sequence typer

`kcst` predicts species and MLST from assembled sequences or raw reads.

@@ UNDER CONSTRUCTION @@

Home: <https://github.com/zwets/kcst>


## Introduction

`kcst` predicts species and multi-locus sequence type from assembled or raw
reads.  It does this by comparing the kmers in the query sequences with those
found in the MLST profiles, picking the alleles best covered by the query.

`kcst` does this 'all in one go'.  Instead of first predicting species in
order to pick the applicable MLST schema, it matches the query across all MLST
alleles.


## Quick Start

Assuming you are on a GNU/Linux system, and you have just cloned `kcst` from
GitHub, here are the steps to run it:

* Install requirements

  To build `kcst` you need a C++ compiler and GNU `make`.  If your system
  doesn't already have these, figuring out how to install them should be easy.
  On Ubuntu: `apt install g++ make`.

  To run `kcst` you need GNU awk (`gawk`), which probably is already on your
  system, or else can be easily installed.  On Ubuntu: `apt install gawk`.
  `awk` is used by the wrapper scripts that drive the binary (C++) core.

* Build Sources

      # Compile the sources
      cd src
      make

      # Optionally compile and run unit tests
      cd src/test
      make test

* Run Core

  The binary core of `kcst` is `khc` (for _k_mer _h_it _c_ount).  It can be
  used standalone as a tool to rapidly compare two sequences for similarity:

      cd src
      ./khc 

* Run `kcst`
   
    # Perform a kmer hit count of QUERY on SUBJECTS
    ./khc [OPTIONS] SUBJECTS [QUERY]

    # In progress: perform kmer counting ST
    ./kcst [OPTIONS] SUBJECTS [QUERY]

## Building

## Installing

`kcst` has no dependencies or installation requirements.  Just run it.

## Database



---

#### License

kcst - kmer-counting sequence typer  
Copyright (C) 2018  Marco van Zwetselaar <io@zwets.it>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

