#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "fastareader.h"

static const int MIN_KSIZE = 1;
static const int MAX_KSIZE = 31;
static const int DEFAULT_KSIZE = 16;

static const std::string USAGE("\n"
"Usage: kcst [-k KSIZE]\n"
"\n"
"  Read FASTA on stdin, write to stdout tab-separated kmers.\n"
"\n");

static int ksize = DEFAULT_KSIZE;
/*
void write_masking (const std::string& bs, const std::string& qs, char min_phred)
{
    std::string::const_iterator p = bs.begin();
    std::string::const_iterator p1 = bs.end();
    std::string::const_iterator q = qs.begin();
    while (p != p1) {
        if (flip) {
            char c = *p++;
            std::cout << static_cast<char>(*q++ < min_phred ? tolower(c) : toupper(c));
        }
        else {
            std::cout << static_cast<char>(*q++ < min_phred ? repl : toupper(*p));
            ++p;
        }
    }
    std::cout << std::endl;
*/

int main (int argc, char *argv[]) 
{
    try {
        while (*++argv) 
        {
            if (!std::strcmp("-k", *argv) && *++argv) {
                ksize = std::atoi(*argv);
                if (ksize < MIN_KSIZE || ksize > MAX_KSIZE) {
                    std::cerr << "error: invalid KSIZE: " << ksize << "\n";
                    return 1;
                }
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
            std::vector<char>::const_iterator p = seq.data.begin();
            while (p != seq.data.end())
                std::cout << *p++;
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
