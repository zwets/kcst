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
#  Home: http://io.zwets.it/kcst

# Make sure sort and awk run predictably and fast
export LC_ALL="C"

# Constants
PROGNAME="$(basename "$0")"
MLST_DB="mlst.db"
MLST_CFG="mlst.cfg"
MLST_TSV="mlst.tsv"
FSA_EXT=".fsa"
TSV_EXT=".tsv"
TAB="$(printf '\t')"

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
    echo "${PROGNAME}: $*"
    exit 1
}

# Function to show usage information and exit
usage_exit() {
    echo "
Usage: $PROGNAME [OPTIONS] INPUT_DIR [OUTPUT_DIR]

  Compile an MLST database for kcst from the FASTA and TSV files in INPUT_DIR
  or its subdirectories.  Write the database to OUTPUT_DIR (default PWD).
  The generated database consists of the files $MLST_DB, $MLST_TSV, $MLST_CFG.

  OPTIONS
   -k, --ksize K   Use k-mer size K (default $K_SIZE)
   -f, --force     Overwrite database files that exist in OUTPUT_DIR
   --fsa-ext FSA   File extension of FASTA files (default \"$FSA_EXT\")
   --tsv-ext TSV   File extension of TSV files (default \"$TSV_EXT\")
   -x, --khc KHC   Path to the khc binary, if not on PATH or in ../bin
                   (default $KHC_EXE, or searched for on the PATH)
   -v, --verbose   Report progress on stderr

  INPUT_DIR must contain a file called \"config\" listing the MLST schemes to
  include.  Each non-comment line in \"config\" must have three columns:

   1. base name: the name of the $FSA_EXT and $TSV_EXT files, without path or
      extension; files will be looked for in INPUT_DIR and its subdirectories
   2. scheme name: the human-readable name for the scheme; usually the species
      name, or the species and MLST variant (e.g.: A. baumanni (Oxford))
   3. profile genes: a comma-separated list of the genes in the scheme; these
      must correspond to column names in the header line of the $TSV_EXT file

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

# Finally ready to perform the import

emit "processing config file: $CFG_FILE"

grep -E '^[^#]' "$CFG_FILE" | while read SCHEME REST; do
    
    FSA_NAME="${SCHEME}${FSA_EXT}"
    FSA_FILE="$(find "$INPUT_DIR" -name "$FSA_NAME")"
    [ -f "$FSA_FILE" ] || err_exit "file not found: $FSA_NAME"

    TSV_NAME="${SCHEME}${TSV_EXT}"
    TSV_FILE="$(find "$INPUT_DIR" -name "$TSV_NAME")"
    [ -f "$TSV_FILE" ] || err_exit "file not found: $TSV_NAME"

    # MLST_CFG: append the current entry from CFG_FILE
    echo "${SCHEME}${TAB}${REST}" >> "$MLST_CFG"

    # MLST_TSV: append the TSV_FILE except the header while prefixing a SCHEME column
    tail -n +2 "$TSV_FILE" | sed -e "s/^/${SCHEME}${TAB}/" >> "$MLST_TSV"

    # MLST_FSA: append the FSA_FILE rewriting sequence headers to ">SCHEME:GENE:ALLELE"
    awk -v S="$SCHEME" '
        /^>/     { match($1,"^>(.*)[^0-9]([0-9]+)( .*)?$",A); print ">" S ":" A[1] ":" A[2] }
        /^[^>]/' "$FSA_FILE" >> "$MLST_DB.TMP"
done | 

emit "compiling FASTA files to $MLST_DB"

echo | $KHC_EXE ${VERBOSE:+"-v"} -k $K_SIZE -w "$MLST_DB" "$MLST_DB.TMP"

rm -f "$MLST_DB.TMP"

emit "done"

# vim: sts=4:sw=4:et:si:ai
