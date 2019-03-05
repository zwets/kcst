/* khc.cpp - kmer hit count
 * 
 * Copyright (C) 2018  Marco van Zwetselaar <io@zwets.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "templatedb.h"
#include "kmerdb.h"
#include "utils.h"

using namespace khc;

static const int MAX_KSIZE = 31;
static const int MAX_VARS = 1024;
static const double DEFAULT_COV = 90.0;

static const char USAGE[] = "\n"
"Usage: khc [OPTIONS] SUBJECTS [QUERY ...]\n"
"\n"
"  For each QUERY in turn, map all its k-mers on each sequence in SUBJECTS.\n"
"  Report for each QUERY the sequences in SUBJECTS that are covered by QUERY\n"
"  for at least COV percent.\n"
"\n"
"  OPTIONS\n"
"   -k KSIZE  k-mer size KSIZE; compulsory unless SUBJECTS was generated with\n"
"             -w FILE, in which case KSIZE was set at the time of generation\n"
"   -c COV    coverage threshold (default %.1f%%); coverage is measured as the\n"
"             percentage of bases covered by at least one k-mer from QUERY\n"
"   -j VARS   allow at most VARS variants (default %d) for a SUBJECT k-mer\n"
"             that contains degenerate bases\n"
"   -s        skip k-mers in QUERY that contain degenerate bases (including\n"
"             N), instead of terminating the program when one is encountered\n"
"   -t        precede QUERY outputs by a title line '## Query: NAME'\n"
"   -w FILE   write an optimised binary representation of SUBJECTS to FILE;\n"
"             FILE can then be used instead of SUBJECT, with large speed gains\n"
"   -m MEM    constrain memory use to about MEM GB (default: all minus 2GB)\n"
"   -v        produce verbose output to stderr\n"
"\n"
"  File SUBJECTS must be either (optionally compressed) FASTA or an optimised\n"
"  binary file previously produced by -w FILE.\n"
"\n"
"  SUBJECT sequences may contain degenerate bases (IUPAC symbols BDHKMNRSVYW),\n"
"  but a limit applies to the number of k-mer variants these wildcards would\n"
"  generate (-j VARS).  Note how a single N generates 4 k-mer variants, BDHV\n"
"  generate 3, KMSWRY generate 2, and combinations of these letters multiply.\n"
"\n"
"  Each QUERY can be an (optionally compressed) FASTA, FASTQ, or plain DNA\n"
"  file.  If QUERY is omitted or '-', it is read from stdin.  QUERY must not\n"
"  have degenerate bases, or option -s must be set to skip query k-mers with\n"
"  degenerate bases.\n"
"\n"
"  The OUTPUT of each QUERY is separated from the preceding QUERY by a single\n"
"  empty line.  Use option -t to add '## QUERY' title lines.\n"
"\n"
"  More information: http://io.zwets.it/kcst.\n"
"\n";


void
usage_exit()
{
    fprintf(stderr, USAGE, DEFAULT_COV, MAX_VARS);
    std::exit(1);
}

int main (int, char *argv[]) 
{
    std::string tpl_fname;
    std::string qry_fname;
    std::string out_fname;

    int ksize = 0;
    int max_mem = 0;
    int max_vars = MAX_VARS;
    double min_cov = DEFAULT_COV;
    bool skip_degens = false;
    bool write_titles = false;

    set_progname("khc");

        // PARSE ARGUMENTS

    while (*++argv) 
    {
        if (!std::strcmp("-v", *argv)) {
            set_verbose(true);
        }
        else if (!std::strcmp("-s", *argv)) {
            skip_degens = true;
        }
        else if (!std::strcmp("-t", *argv)) {
            write_titles = true;
        }
        else if (!std::strcmp("-w", *argv) && *++argv) {
            out_fname = *argv;
        }
        else if (!std::strcmp("-k", *argv) && *++argv) {
            ksize = std::atoi(*argv);
            if (ksize < 1 || ksize > MAX_KSIZE) 
                raise_error("invalid KSIZE: %s", *argv);
        }
        else if (!std::strcmp("-c", *argv) && *++argv) {
            min_cov = std::atof(*argv);
        }
        else if (!std::strcmp("-m", *argv) && *++argv) {
            max_mem = std::atoi(*argv);
            if (max_mem < 1)
                raise_error("invalid MEM: %s", *argv);
        }
        else if (!std::strcmp("-j", *argv) && *++argv) {
            max_vars = std::atoi(*argv);
            if (max_vars < 0)
                raise_error("invalid VARS: %s", *argv);
        }
        else if (**argv == '-') {
            usage_exit();
        }
        else {
            tpl_fname = *argv;
            verbose_emit("database file: %s", tpl_fname.c_str());
            break;
        }
    }

    if (tpl_fname.empty())
        usage_exit();

        // READ TEMPLATE DB

    std::ifstream tpl_file(tpl_fname, std::ios_base::in|std::ios_base::binary);

    if (!tpl_file)
        raise_error("failed to open template file: %s", tpl_fname.c_str());

    std::unique_ptr<template_db> tpldb = template_db::read(tpl_file, max_mem, ksize, max_vars);

    tpl_file.close();

        // WRITE TEMPLATE DB

    if (!out_fname.empty() && !tpldb->write(out_fname))
        raise_error("failed to write binary template file: %s" , out_fname.c_str());

        // ITERATE OVER QUERY FILES

    qry_fname = *(argv+1) ? *++argv : "-";

    bool single_query = !*(argv+1); // when single query we do no newline after results

    do {
        verbose_emit("query file: %s", qry_fname.c_str());

        if (write_titles)
            std::cout << "## Query: " << qry_fname << std::endl;

        query_result res = tpldb->query(qry_fname, min_cov, skip_degens);

        for (size_t i = 0; i != res.size(); ++i)
            std::cout << res[i].seqid << ' ' << res[i].len << ' ' << res[i].hits << ' ' << res[i].phit << std::endl;

        if (!single_query)
            std::cout << std::endl;

        qry_fname = *++argv ? *argv : "";

    } while (!qry_fname.empty());

    return 0;
}

// vim: sts=4:sw=4:et:si:ai
