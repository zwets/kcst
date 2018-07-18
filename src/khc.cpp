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

#include <stdexcept>
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
"Usage: khc [OPTIONS] SUBJECTS [QUERY]\n"
"\n"
"  Compute for each sequence in SUBJECTS its coverage by all kmers from QUERY.\n"
"  Report all sequences with minimum COV pct base coverage in decreasing order\n"
"  of coverage.\n"
"\n"
"  File SUBJECTS must be either FASTA or an optimised binary file previously\n"
"  produced by -w FILE.  Its sequences may contain degenerate bases (wildcards\n"
"  such as R and N that match multiple bases), but a limit can be set on the\n"
"  number of kmer variants these wildcards would allow (-j VARS).\n"
"\n"
"  File QUERY can be FASTA, FASTQ, or plain DNA characters.  If QUERY is omitted\n"
"  or '-', it is read from stdin.  It is an error for QUERY to have degenerate\n"
"  bases.  Use option -s to skip all kmers which contain a degenerate base.\n"
"\n"
"  OPTIONS\n"
"   -k KSIZE  kmer size KSIZE; compulsory unless a binary SUBJECTS file is used\n"
"             in which case it is the KSIZE used when producing the binary file\n"
"   -c COV    minimum coverage percentage (default %.1f%%); coverage is measured\n"
"             as percentage of bases covered by at least one kmer for query\n"
"   -j VARS   allow at most VARS variants per kmer (default %d); note how\n"
"             N produces 4 variants, B,D,H,V produce 3, K,M,S,W,R,Y produce 2.\n"
"   -w FILE   write binary representation of SUBJECTS to FILE; FILE can then be\n"
"             used in subsequent invocations, with considerable speed gains\n"
"   -m MEM    constrain memory use to about MEM GB (default: all minus 2GB)\n"
"   -s        skip kmers in QUERY that contain degenerate bases (including N)\n"
"   -v        produce verbose output to stderr\n"
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

    set_progname("khc");

        // PARSE ARGUMENTS

    try {
        while (*++argv) 
        {
            if (!std::strcmp("-v", *argv)) {
                set_verbose(true);
            }
            else if (!std::strcmp("-s", *argv)) {
                skip_degens = true;
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
            else if (tpl_fname.empty()) {
                tpl_fname = *argv;
                verbose_emit("database file: %s", tpl_fname.c_str());
            }
            else if (qry_fname.empty()) {
                qry_fname = *argv;
                verbose_emit("query file: %s", qry_fname.c_str());
            }
            else
                usage_exit();
        }

        if (tpl_fname.empty())
            usage_exit();

        if (qry_fname.empty())
            qry_fname = "-";

            // READ TEMPLATE DB

        std::ifstream tpl_file(tpl_fname);

        if (!tpl_file)
            raise_error("failed to open template file: %s", tpl_fname.c_str());

        std::unique_ptr<template_db> tpldb = template_db::read(tpl_file, max_mem, ksize, max_vars);

        tpl_file.close();

            // WRITE TEMPLATE DB

        if (!out_fname.empty() && !tpldb->write(out_fname))
            raise_error("failed to write binary template file: %s" , out_fname.c_str());

            // PERFORM QUERY

        query_result res = tpldb->query(qry_fname, min_cov, skip_degens);

            // SHOW RESULT

        for (size_t i = 0; i != res.size(); ++i)
            std::cout << res[i].seqid << ' ' << res[i].len << ' ' << res[i].hits << ' ' << res[i].phit << std::endl;
    }
    catch (std::runtime_error e) {
        std::cerr << std::endl << "khc: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// vim: sts=4:sw=4:et:si:ai
