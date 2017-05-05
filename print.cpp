#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

void printMap(hash_map<size_t> &kmers, size_t &kLen,
	      size_t &minOcc, ostream &out) {
  ostringstream ss;
  string kmerStr(kLen, 'X');
  char c[4] = {'A', 'C', 'G', 'T'};

  cerr << "Printing " << kLen << "-mers..." << endl;
  for (const auto& kmer: kmers.lock_table()) {
    if (kmer.second < minOcc)
      continue;
    for (size_t i = 0, kmerInt = kmer.first; i < kLen; ++i, kmerInt >>= 2)
      kmerStr[kLen-i-1] = c[kmerInt & 3];
    ss << ">" << kmer.second << endl << kmerStr << endl;
  }
  out << ss.str();
}

void printMap(hash_map<string> &kmers, size_t &kLen,
	      size_t &minOcc, ostream &out) {
  ostringstream ss;

  cerr << "Printing " << kLen << "-mers..." << endl;
  for (const auto& kmer: kmers.lock_table()) {
    if (kmer.second < minOcc)
      continue;
    ss << ">" << kmer.second << endl << kmer.first << endl;
  }
  out << ss.str();
}
