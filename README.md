# kcst - kmer-counting sequence typer

Proof of concept for a kmer-counting sequence typer.  (WORK IN PROGRESS)

Home: <https://github.com/zwets/kcst>


## Introduction

`kcst` reads in the MLST database, storing every kmer found in an index
that maps the kmer on the allele(s) it was found in.  It then handles queries
by counting hits by the kmers in the query sequence on kmers in the index.

## Running

    ```bash
    # No argument handling yet (WIP)
    ./kcst [OPTIONS] DBFILE <QUERYFILE
    ```

## Building

    ```bash
    cd src
    make
    cd test
    make
    make test
    ```

## Installation

`kcst` has no dependencies, you can put it anywhere.

