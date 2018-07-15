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

#include "templatedb.h"
#include "kmerdb.h"
#include "utils.h"

using namespace khc;

static const int MAX_KSIZE = 31;
static const double DEFAULT_COV = 90.0;

static const std::string USAGE("\n"
"Usage: khc [-k KSIZE] [-c COV] [-m MEM] [-d VARS] [-w FILE] [-v] SUBJECTS [QUERY]\n"
"\n"
"  Count for each sequence in SUBJECTS the number of kmers from QUERY that\n"
"  hit it.  Report every sequence with at least COV percent base coverage.\n"
"\n"
"  @DOCUMENTATION will follow once stabilised\n"
"\n");


int main (int, char *argv[]) 
{
    std::string tpl_fname;
    std::string qry_fname;
    std::string out_fname;

    int ksize = 0;
    int max_mem = 0;
    int max_vars = 0;
    double min_cov = DEFAULT_COV;

    set_progname("khc");

        // PARSE ARGUMENTS

    try {
        while (*++argv) 
        {
            if (!std::strcmp("-v", *argv))
            {
                set_verbose(true);
            }
            else if (!std::strcmp("-w", *argv) && *++argv)
            {
                out_fname = *argv;
            }
            else if (!std::strcmp("-k", *argv) && *++argv)
            {
                ksize = std::atoi(*argv);
                if (ksize < 1 || ksize > MAX_KSIZE) 
                    raise_error("invalid KSIZE: %s", *argv);
            }
            else if (!std::strcmp("-c", *argv) && *++argv)
            {
                min_cov = std::atof(*argv);
            }
            else if (!std::strcmp("-m", *argv) && *++argv)
            {
                max_mem = std::atoi(*argv);
                if (max_mem < 1)
                    raise_error("invalid MEM: %s", *argv);
            }
            else if (!std::strcmp("-d", *argv) && *++argv)
            {
                max_vars = std::atoi(*argv);
                if (max_vars < 0)
                    raise_error("invalid VARS: %s", *argv);
            }
            else if (**argv == '-') 
            {
                std::cerr << USAGE;
                return 1;
            }
            else if (tpl_fname.empty())
            {
                tpl_fname = *argv;
                verbose_emit("database file: %s", tpl_fname.c_str());
            }
            else if (qry_fname.empty())
            {
                qry_fname = *argv;
                verbose_emit("query file: %s", qry_fname.c_str());
            }
            else
            {
                std::cerr << USAGE;
                return 1;
            }
        }

        if (tpl_fname.empty())
        {
            std::cerr << USAGE;
            return 1;
        }

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

        query_result res = tpldb->query(qry_fname, min_cov);

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
