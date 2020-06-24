# kcst - k-mer counting sequence typer

`kcst` predicts species and MLST from assembled sequences or raw reads.


## Introduction

`kcst` predicts species and multi-locus sequence type from assembled or raw
reads.  It does this by comparing the k-mers in the query sequences with those
in the MLST profiles, picking the alleles best covered by the query.

`kcst` does this 'all in one go'.  Instead of first predicting species in
order to pick the applicable MLST scheme, it matches the query across all
MLST schemes at once.

By using a k-mer mapping approach, kcst is very fast.  It typically takes
about a second to type one assembled genome.  The binary core of `kcst` is
`khc` (k-mer hit count).  `khc` can be used standalone to rapidly compute
sequence similarity between a query and a set of subject sequences.

More information: <http://io.zwets.it/kcst>
 

## Quick Start

Assuming you are on a GNU/Linux system, and you have just cloned `kcst` from
GitHub, here are the steps to run it:

* Install requirements

  To build `kcst` you need a C++ compiler and GNU `make`.  Run `c++ --version`
  and `make --version` to check that you have these.  Support for gzipped files
  requires Boost Iostreams (install the `libboost-iostreams-dev` package).

  Running `kcst` requires GNU `awk`, which probably is already on your system
  (try `gawk --version`), or else can be installed via the package manager.

* Build

      # Compile the khc source code
      cd src
      make

      # Optionally run the unit tests
      make test

* Install

  There is no need to install `khc` in a specific place, only that for `kcst`
  to find it, `khc` must be on PATH or in the same directory as `kcst`.

      # Symlink bin/khc to the compiled binary src/khc
      # Note: if you cloned the repository, this link will already be present
      cd bin
      ln -sf ../src/khc

      # For convenience in the steps below, put kcst and khc on the PATH
      # (While in directory ./bin)
      PATH="$PWD:$PATH"

* Run `khc`

      # khc has self-contained usage instructions
      khc --help

      # Example: calculate coverage of ecoli.fsa by k-mers from test.fa.gz
      cd data/examples
      khc -s -k 15 -c 95 ecoli.fsa test.fa.gz

      # Example: pipe from stdin and sort on coverage
      zcat test.fa.gz | khc -s -k 15 -c 95 ecoli.fsa | sort -k4,4nr

* Run `kcst`

      # Construct example database with just ecoli.fsa
      cd data/examples
      ../make-kcst-db.sh -f "$PWD"  # reads file 'config', writes kcst.*

      # Perform MLST of test.fa against the example database
      kcst -d "$PWD" test.fa.gz

      # After you install the default database (see below), this suffices:
      kcst test.fa.gz


## Installation

#### Executables

See the Quick Start above.  `kcst` and `khc` do not need to be installed in any
specific location.  The only requirement is that `kcst` can find `khc` either
on the PATH or in the directory where `kcst` itself is located.

#### Database

You must provide an MLST database for `kcst` to work with.  The simplest way to
install one is by cloning and importing the database maintained by the Centre
for Genomic Epidemiology (CGE) at DTU Copenhagen, as follows:

    # Pick a directory to contain the downloadeded CGE database
    CGE_MLST_DIR=/your/path/of/choice

    # Clone the CGE git repository with the MLST database
    git clone 'https://bitbucket.org/genomicepidemiology/mlst_db.git' "$CGE_MLST_DIR"

    # Import the database to kcst's default MLST database directory (./data)
    cd data
    ./make-kcst-db.sh -v -f "$CGE_MLST_DIR"  # Note default k-mer size is 15

    # Check that the three database files have been created
    ls kcst.*   # should give kcst.db, kcst.cfg, kcst.tsv

    # Test the database: run MLST on test.fa
    kcst -v examples/test.fa.gz

You can now remove `CGE_MLST_DIR`, or keep it around for pulling future updates:

    # Update the CGE repository to the current release
    CGE_MLST_DIR=/path/where/you/cloned/it
    cd $CGE_MLST_DIR
    git pull

    # Import the database in kcst's data directory
    cd $(dirname $(realpath $(command -v kcst)))/../data
    ./make-kcst-db.sh -f -v "$CGE_MLST_DIR"

`kcst` can work with multiple separate databases (see option `--db`), for
instance with different species or k-mer sizes (the default is 15).  Refer to
`data/README.md` for information about importing MLST databases.


---

#### License

kcst - k-mer counting sequence typer  
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

