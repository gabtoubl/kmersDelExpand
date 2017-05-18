#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "kmers.hpp"

using namespace std;

static void setMaxLine(ifstream &infile, size_t &maxLine, size_t &maxThreads) {
  maxLine = count(istreambuf_iterator<char>(infile),
		  istreambuf_iterator<char>(), '\n') / 2;
  infile.clear();
  infile.seekg(0);
  maxLine = (maxLine - maxLine % maxThreads) / maxThreads;
  maxLine = maxLine > 200000 ? 200000 : maxLine < 1 ? 1 : maxLine;
}

static int usage() {
  cerr << "usage: ./kmersDel OPTIONS" << endl
       << "-> mandatory: -s KMERSEED Seed for spaced kmers" << endl
       << "-> optional:  -i INFILE   Input File in FASTA format, default stdin" << endl
       << "              -o OUTFILE  Output File, default stdout" << endl
       << "              -j THREADS  Number of threads, default 1" << endl
       << "              -h          Help: This usage message" << endl;
  return EXIT_FAILURE;
}

int main(int ac, char **av) {
  bool flagInfile = false, flagOutfile = false, flagSeed = false;
  size_t maxThreads = 1, maxLine = 100000, seedLen;
  ofstream outfile;
  ifstream infile;
  string seed;
  char opt;

  while ((opt = getopt(ac, av, "j:k:s:i:o:L:h")) != -1) {
    switch (opt) {
    case 'j': setMaxThreads(optarg, maxThreads); break;
    case 's': setSeed(optarg, seed, seedLen, flagSeed); break;
    case 'i': setInfile(optarg, infile, flagInfile); break;
    case 'o': setOutfile(optarg, outfile, flagOutfile); break;
    case 'h':
    default : return usage();
    }
  }
  if (!flagSeed)
    return usage();
  if (!flagInfile)
    cerr << "Using Standard Input as FASTA file" << endl;
  else
    setMaxLine(infile, maxLine, maxThreads);
  seedLen = seed.length();
  kmersDel(seed, maxThreads, maxLine, seedLen,
	   flagInfile ? infile : cin,
	   flagOutfile ? outfile : cout);
  if (flagInfile)
    infile.close();
  if (flagOutfile)
    outfile.close();
  return EXIT_SUCCESS;
}
