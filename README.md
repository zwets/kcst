# kcst - k-mer counting sequence typer

`kcst` predicts species and MLST from assembled sequences or raw reads.

Home: <https://github.com/zwets/kcst>


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
 

## Quick Start

Assuming you are on a GNU/Linux system, and you have just cloned `kcst` from
GitHub, here are the steps to run it:

* Install requirements

  To build `kcst` you need a C++ compiler and GNU `make`.  Run `c++ --version`
  and `make --version` to check that you have these.

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
  to find it, `khc` must be on PATH or in kcst's bin directory.

      # Symlink bin/khc to the compiled binary src/khc
      # Note: if you cloned the repository, this link will already be present
      cd bin
      ln -sf ../src/khc

      # For convenience in the steps below, put kcst and khc on the PATH
      # In directory ./bin
      PATH="$PWD:$PATH"

* Run `khc`

      # khc has self-contained usage instructions
      khc --help

      # Example: calculate coverage of ecoli.fsa by k-mers from test.fa
      cd ../data/examples
      zcat test.fa.gz | khc -s -k 15 -c 95 ecoli.fsa

* Run `kcst`

      # Construct example database with just ecoli.fsa
      # (Still in data/examples)
      ../make-db.sh -f "$PWD"  # reads file 'config', writes mlst.*

      # Perform MLST on test.fa against the example database
      kcst -d "$PWD" test.fa.gz   # Note kcst un(b|g|x)zips transparently


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

    # Import the database to kcst's default MLST database directory
    cd data
    ./make-db.sh -f -v "$CGE_MLST_DIR"

    # Check that the three database files have been created
    ls mlst.*   # should give mlst.db, mlst.cfg, mlst.tsv

    # Test the database using the sample assembly
    kcst data/examples/test.fa.gz

You can now remove `CGE_MLST_DIR`, but you can also keep it around for pulling
future updates:

    # Update the CGE repository to the current release
    CGE_MLST_DIR=/path/where/you/cloned/it
    cd $CGE_MLST_DIR
    git pull

    # Reimport the database in kcst's data directory
    cd data  # the kcst data directory
    ./make-db.sh -f -v "$CGE_MLST_DIR/config"

You can use multiple MLST databases with `kcst` (see option `--db`).  Refer to
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

