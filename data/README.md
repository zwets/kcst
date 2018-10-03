# kcst database creation

`kcst` operates on a compiled database of MLST alleles (mlst.db), gene schemas
(mlst.config), and allele profiles (mlst.tsv).  Use the `make-kcst-db.sh`
script to generate the database.

The simplest way to set this up is to download the curated MLST database of the
Centre for Genomic Epidemiology (https://genomicepidemiology.org/organisation).

You can also pick your own set of FASTA and TSV files, and optionally provide
a config file.


## Using the CGE MLST database

Using the CGE MLST database is by far the easiest, and has the additional
benefit of being very simple to update.  Just follow these steps.


    # Pick a fresh directory to contain the downloadeded CGE database
    CGE_MLST_DIR=/your/path/of/choice

    # Clone the database from the CGE bitbucket repository
    git clone 'https://bitbucket.org/genomicepidemiology/mlst_db.git' "$CGE_MLST_DIR"

    # Go to the data directory having the make-kcst-db.sh script and run it
    ./make-kcst-db.sh -c "$CGE_MLST_DIR/config"

    # This will create the ./cge-db directory having the three database files:
    ls ./cge-db/
    mlst.db mlst.config mlst.tsv

You can now remove the `CGE_MLST_DIR`, or keep it to occasionally update
to the latest version of the database.


### Updating the CGE MLST database

    # Go to the directory where you cloned the CGE mlst_db
    CGE_MLST_DIR=/your/path/of/choice
    cd CGE_MLST_DIR

    # Pull to latest release
    git pull

    # Go back to the data directory that has this README
    cd KCST_DIR/data

    # Overwrite the previous import with the current files
    ./make-kcst-db.sh -c "$CGE_MLST_DIR/config"
    
    
## Using a database of your own

@@TODO@@ complete this section


