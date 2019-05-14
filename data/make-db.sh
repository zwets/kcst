#!/bin/sh
#
#  make-db - Create MLST database for kcst
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
#  Home: http://io.zwets.it/kcst

# Exit on error
set -e

# Make sure sort and awk run predictably and fast
export LC_ALL="C"

# Constants
PROGNAME="$(basename "$0")"
MLST_DB="mlst.db"
MLST_CFG="mlst.cfg"
MLST_TSV="mlst.tsv"
FSA_EXT=".fsa"
TSV_EXT=".tsv"

# Defaults
OUTPUT_DIR="."
KHC_EXE="$(realpath "$(dirname "$0")/../bin/khc")"
K_SIZE=15

# Function outputs $* on stderr if VERBOSE is set
emit() {
    [ -z "$VERBOSE" ] || echo "${PROGNAME}: $*" >&2
}

# Function exits this script with $* on stderr
err_exit() {
    echo "${PROGNAME}: $*" >&2
    exit 1
}

# Function finds single file $2 under directory $1 or errors
find_file() {
    [ -d "$1" ] || err_exit "no such directory: $1"
    local FIND="$(find "$1" -name "$2")" || true
    [ -n "$FIND" ] || err_exit "file not found below directory $1: $2"
    local FILE="$(find "$1" -name "$2" | head -1)"
    [ "$FILE" = "$FIND" ] || echo "WARNING: multiple files for: $2; picking $FILE" >&2
    echo "$FILE"
}

# Function outputs the permutation of space-separated items $2 to obtain $1
permute_to() {
    awk -v "SRC=$2" -v "TGT=$1" 'END {
        N=split(SRC,L1); I=0; while (++I<=N) COL[L1[I]]=I
        N=split(TGT,L2); I=0; while (++I<=N) 
            if ((POS=COL[L2[I]]) > 0) RES = RES OFS POS; else exit 1
        print RES
    }' /dev/null
}

# Function to show usage information and exit
usage_exit() {
    echo "
Usage: $PROGNAME [OPTIONS] INPUT_DIR [OUTPUT_DIR]

  Compile an MLST database for kcst from the FASTA and TSV files in INPUT_DIR
  or its subdirectories.  Write the database to OUTPUT_DIR (default PWD).
  The generated database consists of the files $MLST_DB, $MLST_TSV, $MLST_CFG.

  OPTIONS
   -k, --ksize KSIZE   Use k-mer size KSIZE (default $K_SIZE)
   -f, --force         Overwrite database files that exist in OUTPUT_DIR
   -x, --khc=KHC       Path to the khc binary, if not on PATH or in ../bin
       --fsa-ext=FSA   File extension of FASTA files (if not \"$FSA_EXT\")
       --tsv-ext=TSV   File extension of TSV files (if not \"$TSV_EXT\")
   -v, --verbose       Report progress on stderr

  INPUT_DIR must contain a file called \"config\" listing the MLST schemes to
  include.  Each non-comment line in \"config\" must have three tab-separated
  columns (see the example in the source distribution):

   1. base name: the name of the $FSA_EXT and $TSV_EXT files, without path or
      extension; files will be looked for in INPUT_DIR and its subdirectories
   2. scheme name: the human-facing name for the scheme; usually species name
   3. profile loci: comma-separated list of the loci of the scheme; these must
      correspond to column names in the header line of the $TSV_EXT file

  This script validates a number of constraints, such as that every allele in
  the $FSA_EXT files must be for a gene in the corresponding profile, and every
  gene column in each $TSV_EXT file must match a gene in the profile.
" >&2
    exit ${1:-1}
}

# Parse options

unset FORCE VERBOSE
while [ $# -ne 0 -a "$(expr "$1" : '\(.\)..*')" = "-" ]; do
    case $1 in
    --ksize=*)    K_SIZE="${1##--ksize=}" ;;
    -k|--ksize)   shift || usage_exit; K_SIZE="$1" ;;
    --khc=*)      KHC_EXE="${1##--khc=}" ;;
    -x|--khc)     shift || usage_exit; KHC_EXE="$1" ;;
    --fsa*=*)     FSA_EXT="${1##--fsa*=}" ;;
    --fsa*)       shift || usage_exit; FSA_EXT="$1" ;;
    --tsv*=*)     TSV_EXT="${1##--tsv*=}" ;;
    --tsv*)       shift || usage_exit; TSV_EXT="$1" ;;
    -f|--force)   FORCE=1 ;;
    -v|--verbose) VERBOSE=1 ;;
    -h|--help)    usage_exit 0 ;;
    *)            usage_exit ;;
    esac
    shift || usage_exit
done

# Check arguments

[ $# -ge 1 ] || usage_exit

INPUT_DIR="$1"
[ -d "$INPUT_DIR" ] || err_exit "no such directory: $INPUT_DIR"

OUTPUT_DIR="${2:-"$OUTPUT_DIR"}"
[ -d "$OUTPUT_DIR" ] || err_exit "no such directory: $OUTPUT_DIR"

CFG_FILE="$INPUT_DIR/config"
[ -r "$CFG_FILE" ] || err_exit "no config file: $CFG_FILE"

# Clear existing database

checked_overwrite() {
    [ -n "$FORCE" ] || [ ! -f "$1" ] || err_exit "file exists (use -f to overwrite): $1"
    rm -f "$1"
}

MLST_DB="$OUTPUT_DIR/$MLST_DB"
MLST_CFG="$OUTPUT_DIR/$MLST_CFG"
MLST_TSV="$OUTPUT_DIR/$MLST_TSV"

checked_overwrite "$MLST_DB"
checked_overwrite "$MLST_CFG"
checked_overwrite "$MLST_TSV"

# Check for availability of KHC

KHC_EXE="${KHC_EXE:-"$(which khc 2>/dev/null)"}"
[ -x "$KHC_EXE" ] || err_exit "khc binary not found; did you compile it?"

# Ready to perform the import

emit "processing config file: $CFG_FILE"

grep -E '^[^#]' "$CFG_FILE" | while read BASE REST; do

    LOCI="$(echo "$REST" | cut -f2)"
    
    # Locate the FSA and TSV below INPUT_DIR
    FSA_FILE="$(find_file "$INPUT_DIR" "${BASE}${FSA_EXT}")"
    TSV_FILE="$(find_file "$INPUT_DIR" "${BASE}${TSV_EXT}")"

    # MLST_CFG: append the current entry from CFG_FILE
    printf "${BASE}\t${REST}\n" >> "$MLST_CFG"

    # MLST_TSV: map the TSV column order on that in the config
    COL_ORDER="$(permute_to "$(echo "$LOCI" | tr ',' ' ')" "$(head -1 "$TSV_FILE" | tr '\t' ' ')")" ||
        err_exit "$TSV_FILE: header is missing one or more loci from config: $LOCI"

    # ... then use awk to append columnsturn numbers into refs for awk and process TSV_FILE onto MLST_TSV
    COL_REFS="\$1$(echo "$COL_ORDER" | sed -e 's/ /,\$/g')"
    awk "BEGIN { OFS=\"\t\" } 
         NR>1 && NF>1 { print \"${BASE}\",${COL_REFS} }" "$TSV_FILE" >> "$MLST_TSV"

    # MLST_FSA: append the FSA_FILE rewriting sequence headers to ">BASE:GENE:ALLELE"
    awk -v S="$BASE" '
        /^>/     { match($1,"^>(.*)[^0-9]([0-9]+)( .*)?$",A); print ">" S ":" A[1] ":" A[2] }
        /^[^>]/' "$FSA_FILE" >> "$MLST_DB.TMP"
done

emit "compiling FASTA files to $MLST_DB"

echo | $KHC_EXE ${VERBOSE:+"-v"} -k $K_SIZE -w "$MLST_DB" "$MLST_DB.TMP"

rm -f "$MLST_DB.TMP"

emit "done"

# vim: sts=4:sw=4:et:si:ai
