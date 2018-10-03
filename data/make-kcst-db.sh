#!/bin/sh
#
#  make-kcst-db - Create MLST database for kcst
#  Copyright (C) 2018  Marco van Zwetselaar <io@zwets.it>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#  Home: https://github.com/zwets/kcst

# Make sure sort and awk run predictably and fast
export LC_ALL="C"

# Constants
PROGNAME="$(basename "$0")"
TAB="$(printf '\t')"

# Defaults
KHC_EXE="$(realpath "$(dirname "$0")/../bin/khc")"
DB_DIR="$(realpath "$(dirname "$0")/cge-db")"
K_SIZE=15

# Function outputs $* on stderr
err_msg() {
    echo "${PROGNAME}: $*" >&2
}

# Function exits this script with $* on stderr
err_exit() {
    err_msg $*
    exit 1
}

# Function emits $* on stderr if VERBOSE is set
dbg_msg() {
    [ -z "$VERBOSE" ] || echo "${PROGNAME}: $*" >&2
}

# Function to show usage information and exit
usage_exit() {
    echo "
Usage: $PROGNAME [OPTIONS] {CONFIG_FILE | DIRECTORY}

  Determine species and MLST for the contigs, reads, or plain DNA in FILE.
  FILE must be FASTA, FASTQ, or plain DNA, and may be (g|b|x)zipped.

  OPTIONS
   -o, --out-dir DIR  Directoru t   minimum percentage allele coverage (default $PCT_COVER)
   -c, --cge-config C N    for reads: sample every N-th read (default 1)
   -d, --db-dir DIR  path to the database directory
                     (default $DB_DIR)
   -x, --khc KHC     path to the khc binary
                     (default $KHC_EXE, or the PATH)
   -v, --verbose     report progress on stderr

  The program outputs the ST (if any) corresponding to the combination of
  top scoring alleles for each gene.  Matches below 100% identity are marked
  with an asterisk (*).  Matches below COV are not considered.

  When the input is FASTQ, enabling sampling will speed up the process, at the
  cost of lower sensitivity.

  $PROGNAME uses the khc binary which must have already been compiled.  The
  database directory DIR must have been set up according to the instructions
  in the README.  For more information, see https://github.com/zwets/kcst.
" >&2
    exit ${1:-1}
}

# Check dependencies

checked_dep() {
    local EXE="${1:-$(which "$2")}"
    local VAR="$(echo "CMD_$2" | tr 'a-z' 'A-Z')"
    [ -x "$EXE" ] && echo "$EXE" || 
        err_exit "no such executable: $2.\nInstall $2 and/or set $VAR to its location."
}

export \
    FILE_EXE="$(checked_dep "$CMD_FILE" 'file')" \
    GAWK_EXE="$(checked_dep "$CMD_GAWK" 'gawk')" \
    SORT_EXE="$(checked_dep "$CMD_SORT" 'sort')" \
    SED_EXE="$(checked_dep "$CMD_SED" 'sed')" \
    || exit 1

# Parse options

unset VERBOSE
while [ $# -ne 0 -a "$(expr "$1" : '\(.\)..*')" = "-" ]; do
    case $1 in
    --db*=*)      DB_DIR="${1##--db*=}" ;;
    -d|--db*)     shift || usage_exit; DB_DIR="$1" ;;
    --cov*=*)     PCT_COVER="${1##--cover*=}" ;;
    -c|--cov*)    shift || usage_exit; PCT_COVER="$1" ;;
    --sample=*)   SAMPLE_NTH="${1##--sample=}" ;;
    -s|--sample)  shift || usage_exit; SAMPLE_NTH="$1" ;;
    --khc=*)      KHC_EXE="${1##--khc=}" ;;
    -x|--khc)     shift || usage_exit; KHC_EXE="$1" ;;
    -v|--verbose) VERBOSE=1 ;;
    -h|--help)    usage_exit 0 ;;
    *)            usage_exit ;;
    esac
    shift || usage_exit
done

# Check arguments

[ $# -eq 1 ] || usage_exit

FILE="$1"
[ -f "$FILE" ] || err_exit "no such file: $FILE"

# Check for KHC

KHC_EXE="${KHC_EXE:-$(which khc 2>/dev/null)}"
[ -x "$KHC_EXE" ] || err_exit "khc binary not found; compile it and use option -x or PATH"

# Check for database

[ -d "$DB_DIR" ] || err_exit "no such directory (use option -d): $DB_DIR"

MLST_DB="$DB_DIR/khc-mlst.db"
MLST_CFG="$DB_DIR/khc-mlst.config"
MLST_TSV="$DB_DIR/khc-mlst.tsv"

[ -f "$MLST_DB" ] || err_exit "database file missing (use option -d): $MLST_DB"
[ -f "$MLST_CFG" ] || err_exit "database config file missing: $MLST_CFG"
[ -f "$MLST_TSV" ] || err_exit "database profile table missing: $MLST_TSV"
[ $# -gt 0 ] || usage_exit

# Do the work

maybe_decompress "$FILE" |
khc_query |
top_hits

# vim: sts=4:sw=4:et:si:ai
# Top directory of the MLST database
MLST_DB_DIR="/data/genomics/cgetools/databases/mlst"

# Extension of the FASTA files
FASTA_EXT=".fsa"

# Extension of the MLST table files
TABLE_EXT=".tsv"

# Function to display a warning on standard error
warn() {
    echo "$(basename "$0"): warning: $*" >&2
}

find "$MLST_DB_DIR" -name "*$FASTA_EXT" | while read FSA; do

    SCHEME="$(basename "$FSA" "$FASTA_EXT")"
    TSV="${FSA%$FASTA_EXT}$TABLE_EXT"
    [ -f "$TSV" ] || { 
        warn "ignoring file, no '$TABLE' file for: $FSA"
        continue
    }

    uf "$FSA" | awk -v S="$SCHEME" '
        /^>/ {	match($1,"^>(.*)[^0-9]([0-9]+)( .*)?$",A); print ">" S ":" A[1] ":" A[2] }
	/^[^>]/'
done

# vim: sts=4:sw=4:et:si:ai
