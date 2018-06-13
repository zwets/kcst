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

#include "seqreader.h"
#include "hitcounter.h"
#include "kmerise.h"
#include "kmerdb.h"
#include "utils.h"

using namespace kcst;

static const int MAX_VARIANTS_PER_KMER = 64;
static const int DEFAULT_KSIZE = 13;
static const int DEFAULT_MEM = 16; // GB

static const std::string USAGE("\n"
"Usage: kcst [-k KSIZE] [-v] DATABASE [QUERIES]\n"
"\n"
"  Read FASTA DATABASE, then read QUERIES on stdin.\n"
"\n");

static int ksize = DEFAULT_KSIZE;
static int max_mem = DEFAULT_MEM;
static int verbose = false;

int main (int, char *argv[]) 
{
    std::string db_fname;
    std::string qry_fname;
    std::string out_fname;

    try {
        while (*++argv) 
        {
            if (!std::strcmp("-v", *argv))
            {
                verbose = true;
            }
            else if (!std::strcmp("-w", *argv) && *++argv)
            {
                out_fname = *argv;
            }
            else if (!std::strcmp("-k", *argv) && *++argv)
            {
                ksize = std::atoi(*argv);
                if (ksize < 1 || ksize > kmeriser::max_ksize) 
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

        // HANDLE DATABASE

        std::ifstream db_file;
        db_file.open(db_fname.c_str());

        if (!db_file)
        {
            std::cerr << "cannot open file: " << db_fname << std::endl;
            return 1;
        }

        kmer_db *db = 0;

        hit_counter counter;

        if (db_file.peek() == '~')
        {
            std::cerr << "reading database file ... ";

            std::string magic;
            size_t nseq;

            db_file >> magic >> nseq;

            if (magic != "~kcst~")
                raise_error("invalid kcst database file: %s", db_fname.c_str());

            std::string line;
            getline(db_file, line);

            while (nseq-- && getline(db_file, line))
                counter.add_target(line);

            db = kmer_db::read_db(db_file, max_mem);
        }
        else
        {
            std::cerr << "creating database from sequences ... ";

            db = kmer_db::new_db(ksize, max_mem);

            sequence_reader reader(db_file, sequence_reader::fasta);
            kmerator k_ator(ksize, MAX_VARIANTS_PER_KMER);
            sequence seq;

            while (reader.next(seq))
            {
                kloc_t loc = counter.add_target(seq.header);
                k_ator.set(seq.data.c_str(), seq.data.c_str() + seq.data.length());
                do {
                    db->add_kloc(k_ator.knum(), loc);
                } while (k_ator.inc());
            }

            if (!out_fname.empty()) 
            {
                std::cerr << "ok\nwriting database file to " <<  out_fname << " ... ";
                std::ofstream out_file(out_fname.c_str());

                out_file << "~kcst~ " << counter.targets().size() << " sequences" << std::endl;
                for (std::vector<std::string>::const_iterator p = counter.targets().begin(); p != counter.targets().end(); ++p)
                    out_file << *p << std::endl;

                db->write(out_file);

                out_file.close();
            }
        }

        std::cerr << "ok" << std::endl;
        db_file.close();

        // HANDLE QUERY

        std::ifstream qry_file;
        std::istream *is = &std::cin;

        if (!qry_fname.empty())
        {
            qry_file.open(qry_fname.c_str());

            if (!qry_file)
            {
                std::cerr << "failed to open query file: " << qry_fname << std::endl;
                return 1;
            }

            is = &qry_file;
            std::cerr << "reading query sequences " << std::endl;
        }
        else
        {
            std::cout << "please enter fasta, fastq, or base sequence data: " << std::endl;
        }

        sequence_reader qry_reader(*is);
        kmeriser k_iser(ksize);
        sequence qry;

        while (qry_reader.next(qry))
        {
            k_iser.set(qry.data.c_str(), qry.data.c_str() + qry.data.length());

            do {
                const std::vector<kloc_t>& hits = db->get_klocs(k_iser.knum());
                for (std::vector<kloc_t>::const_iterator p = hits.begin(); p != hits.end(); ++p)
                    counter.count_hit(*p);
            } while (k_iser.inc());
        }

        if (is != &std::cin)
            qry_file.close();

        std::vector< std::pair<hitc_t,std::string> > results = counter.score_list();

        int i = 21;
        for (std::vector< std::pair<hitc_t,std::string> >::const_reverse_iterator p = results.rbegin(); --i && p != results.rend(); ++p)
        {
            std::cout << p->first << '\t' << p->second << std::endl;
        }

        return 0;
    }
    catch (std::runtime_error e) {
        std::cerr << std::endl << "kcst: " << e.what() << std::endl;
        return 1;
    }
}

// vim: sts=4:sw=4:et:si:ai
