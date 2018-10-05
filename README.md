# kcst - kmer-counting sequence typer

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

  To run `kcst` you need GNU `awk`, which probably is already on your system
  (try `gawk --version`), or else can be installed via the package manager.

* Build

      # Compile the khc source code
      cd src
      make

      # Optionally run the unit tests
      make test

* Install

  There is no need to install khc in a specific place, only that for `kcst`
  to find it, `khc` must be on PATH or in kcst's bin directory.  At checkout,
  the symlink `bin/khc` points to `src/khc`, resolving this requirement.

      # Symlink bin/khc to the compile binary src/khc
      cd bin
      ln -sf ../src/khc

      # For convenience later prepend the kcst and khc bin directory to PATH
      PATH="$PWD:$PATH"

* Run `khc`

      # khc has self-contained usage instructions
      khc --help

      # Calculate coverage of sequences in ecoli.fsa by k-mers from test.fa
      cd data/examples
      zcat test.fa.gz | khc -k 15 -c 95 -s ecoli.fsa

* Run `kcst`

      # Construct example database with just ecoli.fsa
      cd data/examples
      ../make-db.sh -f "$PWD"  # reads file 'config', writes mlst.*

      # Perform MLST on test.fa (still in directory data/examples)
      kcst -d "$PWD" test.fa.gz


## Installation

#### Executables

See the Quick Start above.  `kcst` and `khc` do not need to be installed in any
specific location.  The only requirement is that `kcst` can find `khc` either
on the PATH or in the directory where `kcst` itself is located.  The symlink
`bin/khc` to `src/khc` fulfils the latter requirement by default.

#### Database

The simplest way to install the MLST database for `kcst` is by cloning the data
base maintained by the Centre for Genomic Epidemiology (CGE) at DTU Copenhagen.

    # Pick a fresh directory to contain the downloadeded CGE database
    CGE_MLST_DIR=/your/path/of/choice

    # Clone the CGE git repository having the MLST database
    git clone 'https://bitbucket.org/genomicepidemiology/mlst_db.git' "$CGE_MLST_DIR"

    # Import the database to kcst's default MLST database directory
    cd data
    ./make-db.sh -f -v "$CGE_MLST_DIR"

    # Check that the database files are there
    ls  # should list: mlst.config mlst.db mlst.tsv

    # Test the database with the example E coli assembly (kcst gunzips automatically)
    kcst data/examples/test.fa.gz

You can now remove the `CGE_MLST_DIR`, or keep it around to pull for updates.

    # Update the CGE database to the latest release
    CGE_MLST_DIR=/path/where/you/cloned/it/before
    (cd $CGE_MLST_DIR && git pull)

    # Update the kcst import of the database
    cd data
    ./make-db.sh -f -v "$CGE_MLST_DIR/config"

You can add other databases and use these using `kcst --db DIR`.   Refer to
`data/README.md` for more information.


---

#### License

kcst - k-mer-counting sequence typer  
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

