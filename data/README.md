# kcst database creation

`kcst` operates on a compiled database of MLST alleles (kcst.db), gene schemas
(kcst.cfg), and allele profiles (kcst.tsv).  Use the `./make-kcst-db.sh` script
to generate the database.

The simplest way to set this up is to download the MLST database maintained in
a git repository by the Centre for Genomic Epidemiology at DTU Copenhagen.
Instructions are in `../README.md`.

You can also create your own database.  It requires a set of FASTA and
TSV files (in pairs), and a config file that specifies the scheme's profiles.
A look at the `./examples/config` will probably give enough information.

