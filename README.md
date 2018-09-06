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
`khc` (k-mer hit count).  It can be used standalone to rapidly compute
sequence similarity of a query with each of a set of subject sequences.

 
> **Work in Progress**
>
> 1. The `kcst` wrapper which translates the allele hit scores produced by
> `khc` to corresponding ST-number(s) is work in progress.
>
> 2. `khc` accepts reads files (it auto-detects whether input is FASTA, FASTQ,
> or plain DNA), but until it can do input sampling, querying with reads is
> obviously much slower than with assemblies.


## Quick Start

Assuming you are on a GNU/Linux system, and you have just cloned `kcst` from
GitHub, here are the steps to run it:

* Requirements

  To build `kcst` you need a C++ compiler and GNU `make`.  If your system
  doesn't already have these, figuring out how to install them should be easy.
  On Ubuntu: `apt install g++ make`.

  To run `kcst` you need GNU awk (`gawk`), which probably is already on your
  system, or else can be easily installed.  On Ubuntu: `apt install gawk`.
  `awk` is used by the wrapper scripts that drive the binary (C++) core.

* Build

      # Compile the sources
      cd src
      make

      # Optionally run the unit tests
      make test

* Run `khc`

      cd src
      ./khc --help

      # Calculate coverage of each SUBJECT by kmers from QUERY
      ./khc [OPTIONS] SUBJECTS [QUERY]

* Run `kcst` **In Progress**
   
      # In progress: perform kmer counting ST
      ./kcst [OPTIONS] SUBJECTS [QUERY]


## Installing

`kcst` has no dependencies or installation requirements.  Just run it.


## Usage

Use `kch --help` and `kcst --help` for concise instructions.

_In Progress_


## Database

_In Progress_


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

