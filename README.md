# kcst - kmer-counting sequence typer

`kcst` predicts species and MLST from assembled sequences or raw reads.

Home: <https://github.com/zwets/kcst>


## Introduction

`kcst` predicts species and multi-locus sequence type from assembled or
raw reads.  It does this by comparing the kmers in the query sequences with
those found in the MLST profiles, and picking the alleles most hit by the
query.

`kcst` does this 'all in one go'.  Instead of first predicting species in
order to pick the applicable MLST schema, it matches the query with all MLST
alleles at once.


## Running

    # Proper argument handling is WIP
    ./kcst [OPTIONS] DBFILE <QUERYFILE


## Building

    # Compile the sources
    cd src
    make

    # Optionally compile and run unit tests
    cd src/test
    make test
   

## Installation

`kcst` has no dependencies or installation requirements.  Just run it.


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

