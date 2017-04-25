#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string>

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
  if (k > 32) {
    cerr << "Warning: current program can't count above 32-mers" << endl;
    k = 32;
  }
  kLen = k;
}

void setSeed(string optarg, size_t &kLen, bool &flagOpt) {
  flagOpt ^= true;
  seed = optarg;
  kLen = seed.length() - count(seed.begin(), seed.end(), '0');
  if (kLen > 32) {
    cerr << "Warning: current program can't count above 32-mers" << endl;
    cerr << "Error: with seed, this program can't continue" << endl;
    exit(EXIT_FAILURE);
  }
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

void setMinOcc(string optarg, size_t &minOcc) {
  minOcc = stoi(optarg);
}
