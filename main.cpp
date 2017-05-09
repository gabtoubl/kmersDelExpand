#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include "kmersCount.hpp"

using namespace std;

size_t k = 16;
string seed = "";

static void setMaxLine(ifstream &infile, size_t &maxLine, size_t &maxThreads) {
  maxLine = count(istreambuf_iterator<char>(infile),
		  istreambuf_iterator<char>(), '\n') / 2;
  infile.clear();
  infile.seekg(0);
  maxLine = (maxLine - maxLine % maxThreads) / maxThreads;
  maxLine = maxLine > 200000 ? 200000 : maxLine < 1 ? 1 : maxLine;
}

static int usage() {
  cerr << "usage: ./kmersCount OPTIONS" << endl
       << "-> mandatory: -k KMERLEN  Length of k-mers (either -k or -s)" << endl
       << "              -s KMERSEED Seed for spaced kmers (either -k or -s)" << endl
       << "-> optional:  -i INFILE   Input File in FASTA format, default stdin" << endl
       << "              -o OUTFILE  Output File, default stdout" << endl
       << "              -j THREADS  Number of threads, default 1" << endl
       << "              -L MINOCC   Minimum number of occurences, default 1" << endl
       << "              -h          Help: This usage message" << endl;
  return EXIT_FAILURE;
}

int main(int ac, char **av) {
  bool flagInfile = false, flagOutfile = false, flagOpt = false;
  size_t maxThreads = 1, maxLine = 100000, kLen, minOcc = 1;
  ifstream infile;
  ofstream outfile;
  char opt;

  while ((opt = getopt(ac, av, "j:k:s:i:o:L:h")) != -1) {
    switch (opt) {
    case 'j': setMaxThreads(optarg, maxThreads); break;
    case 'k': setKmerLength(optarg, kLen, flagOpt); break;
    case 's': setSeed(optarg, kLen, flagOpt); break;
    case 'i': setInfile(optarg, infile, flagInfile); break;
    case 'o': setOutfile(optarg, outfile, flagOutfile); break;
    case 'L': setMinOcc(optarg, minOcc); break;
    case 'h':
    default : return usage();
    }
  }
  if (!flagOpt)
    return usage();
  if (!flagInfile)
    cerr << "Using Standard Input as FASTA file" << endl;
  else
    setMaxLine(infile, maxLine, maxThreads);
  if (kLen <= 16) {
    hash_map<size_t> kmers;
    kmersCount(flagInfile ? infile : cin, kmers, maxThreads, maxLine);
    printMap(kmers, kLen, minOcc, flagOutfile ? outfile : cout);
  }
  else {
    hash_map<string> kmers;
    kmersCount(flagInfile ? infile : cin, kmers, maxThreads, maxLine);
    printMap(kmers, kLen, minOcc, flagOutfile ? outfile : cout);
  }
  if (flagInfile)
    infile.close();
  if (flagOutfile)
    outfile.close();
  return EXIT_SUCCESS;
}
