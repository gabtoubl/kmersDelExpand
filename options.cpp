#include <unistd.h>
#include <algorithm>
#include "kmers.hpp"
using namespace std;

static void setMaxThreads(string optarg, size_t &maxThreads) {
  maxThreads = stoi(optarg);
  maxThreads = maxThreads < 1 ? 1 : maxThreads;
}

static void setSeed(string optarg, string &seed, size_t &kLen, bool &flagSeed) {
  flagSeed = true;
  seed = optarg;
  kLen = seed.length() - count(seed.begin(), seed.end(), '0');
}

static void setInfile(string optarg, ifstream &infile, bool &flagInfile) {
  flagInfile = true;
  infile.open(optarg);
  if (!infile.is_open()) {
    cerr << "Error: " << optarg << " could not be opened" << endl;
    exit(EXIT_FAILURE);
  }
}

static void setOutfile(string optarg, ofstream &outfile, bool &flagOutfile) {
  flagOutfile = true;
  outfile.open(optarg);
  if (!outfile.is_open()) {
    cerr << "Error: " << optarg << " could not be opened" << endl;
    exit(EXIT_FAILURE);
  }
}

static void setMaxLine(ifstream &infile, size_t &maxLines, size_t &maxThreads) {
  maxLines = count(istreambuf_iterator<char>(infile),
		   istreambuf_iterator<char>(), '\n') / 2;
  infile.clear();
  infile.seekg(0);
  maxLines = (maxLines - maxLines % maxThreads) / maxThreads;
  maxLines = maxLines < 1 ? 1 : maxLines;
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

int manageOpt(int ac, char **av, Options &opt) {
  char optChar;

  while ((optChar = getopt(ac, av, "j:k:s:i:o:L:h")) != -1) {
    switch (optChar) {
    case 'j': setMaxThreads(optarg, opt.maxThreads); break;
    case 's': setSeed(optarg, opt.seed, opt.seedLen, opt.flagSeed); break;
    case 'i': setInfile(optarg, opt.infile, opt.flagInfile); break;
    case 'o': setOutfile(optarg, opt.outfile, opt.flagOutfile); break;
    case 'h':
    default : return usage();
    }
  }
  if (!opt.flagSeed)
    return usage();
  if (!opt.flagInfile)
    cerr << "Using Standard Input as FASTA file" << endl;
  else
    setMaxLine(opt.infile, opt.maxLines, opt.maxThreads);
  return EXIT_SUCCESS;
}
