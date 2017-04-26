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
  string kmerStr;
  size_t kmerInt;
  char c[4] = {'A', 'C', 'G', 'T'};

  cerr << "Printing k-mers..." << endl;
  for (const auto& kmer: kmers.lock_table()) {
    if (kmer.second < minOcc)
      continue;
    kmerStr = "";
    kmerInt = kmer.first;
    for (size_t i = 0; i < kLen; ++i) {
      kmerStr += c[kmerInt % 4];
      kmerInt /= 4;
    }
    reverse(kmerStr.begin(), kmerStr.end());
    ss << ">" << kmer.second << endl << kmerStr << endl;
  }
  out << ss.str();
}

void printMap(hash_map<string> &kmers, size_t &kLen,
	      size_t &minOcc, ostream &out) {
  ostringstream ss;

  cerr << "Printing k-mers..." << endl;
  (void)kLen;
  for (const auto& kmer: kmers.lock_table()) {
    if (kmer.second < minOcc)
      continue;
    ss << ">" << kmer.second << endl << kmer.first << endl;
  }
  out << ss.str();
}
