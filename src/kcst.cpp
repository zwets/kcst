#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "fastareader.h"
#include "kmeriser.h"
#include "utils.h"

static const int MIN_KSIZE = 1;
static const int MAX_KSIZE = 4 * sizeof(int) - 1;
static const int DEFAULT_KSIZE = 10;

static const std::string USAGE("\n"
"Usage: kcst [-k KSIZE]\n"
"\n"
"  Read FASTA on stdin, write to stdout knums.\n"
"\n");

static int ksize = DEFAULT_KSIZE;

int main (int argc, char *argv[]) 
{
    try {
        while (*++argv) 
        {
            if (!std::strcmp("-k", *argv) && *++argv)
            {
                ksize = std::atoi(*argv);
                if (ksize < MIN_KSIZE || ksize > MAX_KSIZE) 
                    raise_error("invalid KSIZE: %d", ksize);
            }
            else {
                std::cerr << USAGE;
                return 1;
            }
        }

        fasta_reader reader(std::cin);
        sequence seq;

        while (reader.next_sequence(seq))
        {
            std::cout << seq.header << std::endl;
            kmerator k(seq.data.c_str(), seq.data.c_str() + seq.data.length(), ksize);
            std::cout << k.val();
            while (k.inc())
                std::cout << ' ' << k.val();
            std::cout << std::endl;
        }

        return 0;
    }
    catch (std::runtime_error e) {
        std::cerr << "kcst: " << e.what() << std::endl;
        return 1;
    }
}

// vim: sts=4:sw=4:et:si:ai
