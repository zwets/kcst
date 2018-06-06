/* kcst.cpp - kmer-counting sequence typer
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
#include <string>
#include <cstring>
#include <cstdlib>

#include "fastareader.h"
#include "hitcounter.h"
#include "kmeriser.h"
#include "kmerdb.h"
#include "utils.h"

static const int MIN_KSIZE = 1;
static const int MAX_KSIZE = 4 * sizeof(knum) - 1;
static const int DEFAULT_KSIZE = 13;
static const int DEFAULT_MEM = 16; // GB

static const std::string USAGE("\n"
"Usage: kcst [-k KSIZE] [-v] DATABASE [QUERIES]\n"
"\n"
"  Read FASTA DATABASE, then read FASTA QUERIES on stdin.\n"
"\n");

static int ksize = DEFAULT_KSIZE;
static int max_mem = DEFAULT_MEM;
static int verbose = false;

int main (int argc, char *argv[]) 
{
    std::string db_fname;
    std::string qry_fname;

    try {
        while (*++argv) 
        {
            if (!std::strcmp("-v", *argv))
            {
                verbose = true;
            }
            else if (!std::strcmp("-k", *argv) && *++argv)
            {
                ksize = std::atoi(*argv);
                if (ksize < MIN_KSIZE || ksize > MAX_KSIZE) 
                    raise_error("invalid KSIZE: %s", *argv);
            }
            else if (!std::strcmp("-m", *argv) && *++argv)
            {
                max_mem = std::atoi(*argv);
                if (ksize < 1)
                    raise_error("invalid MEM: %s", *argv);
            }
            else if (**argv == '-') 
            {
                std::cerr << USAGE;
                return 1;
            }
            else if (db_fname.empty())
            {
                db_fname = *argv;
                std::cerr << "database file: " << db_fname << std::endl;
            }
            else if (qry_fname.empty())
            {
                qry_fname = *argv;
                std::cerr << "query file: " << qry_fname << std::endl;
            }
            else
            {
                std::cerr << USAGE;
                return 1;
            }
        }

        if (db_fname.empty())
        {
            std::cerr << USAGE;
            return 1;
        }

        std::cerr << "creating database ... ";

        std::ifstream db_file;
        db_file.open(db_fname.c_str());

        if (!db_file)
        {
            std::cerr << "cannot open file: " << db_fname << std::endl;
            return 1;
        }

        kmer_db *db = new_kmer_db(ksize, max_mem);

        std::cerr << "ok\nreading database sequences ... ";

        hit_counter counter;

        fasta_reader reader(db_file);
        sequence seq;

        while (reader.next_sequence(seq))
        {
            skey key = counter.add_target(seq.header);

            kmerator k(seq.data.c_str(), seq.data.c_str() + seq.data.length(), ksize);
            do {
                db->add_kmer(k.val(), key);
            } while (k.inc());
        }

        std::cerr << "ok" << std::endl;

        db_file.close();

        std::ifstream qry_file;
        std::istream *is = &std::cin;

        if (!qry_fname.empty())
        {
            qry_file.open(qry_fname.c_str());

            if (!qry_file)
            {
                std::cerr << "failed to open query file: " << qry_fname << std::cerr;
                return 1;
            }

            is = &qry_file;
            std::cerr << "reading query sequences " << std::endl;
        }
        else
        {
            std::cout << "please enter query sequence: " << std::endl;
        }

        fasta_reader qry_reader(*is);
        sequence qry;

        while (qry_reader.next_sequence(qry))
        {
            kmeriser ki(qry.data.c_str(), qry.data.c_str() + qry.data.length(), ksize);

            do {
                const std::vector<skey>& hits = db->kmer_hits(ki.val());
                for (std::vector<skey>::const_iterator p = hits.begin(); p != hits.end(); ++p)
                    counter.count_hit(*p);
            } while (ki.inc());
        }

        if (is != &std::cin)
            qry_file.close();

        std::vector< std::pair<long,std::string> > results = counter.score_list();

        int i = 21;
        for (std::vector< std::pair<long,std::string> >::const_reverse_iterator p = results.rbegin(); --i && p != results.rend(); ++p)
        {
            std::cout << p->first << '\t' << p->second << std::endl;
        }

        return 0;
    }
    catch (std::runtime_error e) {
        std::cerr << "kcst: " << e.what() << std::endl;
        return 1;
    }
}

// vim: sts=4:sw=4:et:si:ai
