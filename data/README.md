# kcst database creation

`kcst` operates on a compiled database of MLST alleles (mlst.db), gene schemas
(mlst.cfg), and allele profiles (mlst.tsv).  Use the `./make-db.sh` script
to generate the database.

The simplest way to set this up is to download the MLST database maintained in
a git repository by the Centre for Genomic Epidemiology at DTU Copenhagen.
Instructions are in ../README.md.

You can however also create your own database.  It requires a set of FASTA and
TSV files (in pairs), and a config file that specifies the scheme's profiles.
A look at `data/examples/config` will probably give enough information.

