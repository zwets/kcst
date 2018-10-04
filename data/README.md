# kcst database creation

`kcst` operates on a compiled database of MLST alleles (mlst.db), gene schemas
(mlst.cfg), and allele profiles (mlst.tsv).  Use the `make-kcst-db.sh` script
to generate the database.

The simplest way to set this up is to download the curated MLST database of the
Centre for Genomic Epidemiology at DTU, and import this with `make-kcst-db.sh`
as shown below.

You can however also pick your own set of FASTA and TSV files, write a config
file, and import these.


## Using the CGE MLST database

Using the CGE MLST database is by far the easiest, and has the additional
benefit that updating the database requires just a simple `git pull`.

    # Pick a fresh directory to contain the downloadeded CGE database
    CGE_MLST_DIR=/your/path/of/choice

    # Clone the database from the CGE bitbucket repository
    git clone 'https://bitbucket.org/genomicepidemiology/mlst_db.git' "$CGE_MLST_DIR"

    # Import the database to directory ./db
    mkdir -p ./db && ./make-kcst-db.sh -v "$CGE_MLST_DIR" ./db

    # Check that the database files are there
    ls ./db  # should show: mlst.config mlst.db mlst.tsv

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
    ./make-kcst-db.sh -f "$CGE_MLST_DIR" ./db
    
    
## Using a database of your own

@@TODO@@ complete this section


