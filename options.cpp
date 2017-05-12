#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <algorithm>

using namespace std;

extern size_t k;
extern string seed;

void setMaxThreads(string optarg, size_t &maxThreads) {
  maxThreads = stoi(optarg);
  maxThreads = maxThreads < 1 ? 1 : maxThreads;
}

void setKmerLength(string optarg, size_t &kLen, bool &flagOpt) {
  flagOpt ^= true;
  k = stoi(optarg);
  kLen = k;
}

void setSeed(string optarg, size_t &kLen, bool &flagOpt) {
  flagOpt ^= true;
  seed = optarg;
  kLen = seed.length() - count(seed.begin(), seed.end(), '0');
}

void setInfile(string optarg, ifstream &infile, bool &flagInfile) {
  flagInfile = true;
  infile.open(optarg);
  if (!infile.is_open()) {
    cerr << "Error: " << optarg << " could not be opened" << endl;
    exit(EXIT_FAILURE);
  }
}

void setOutfile(string optarg, ofstream &outfile, bool &flagOutfile) {
  flagOutfile = true;
  outfile.open(optarg);
  if (!outfile.is_open()) {
    cerr << "Error: " << optarg << " could not be opened" << endl;
    exit(EXIT_FAILURE);
  }
}
