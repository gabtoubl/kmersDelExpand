#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "kmersCount.hpp"

using namespace std;

void printMap(hash_map &kmers, size_t &kLen,
	      size_t &minOcc, ostream &out) {
  auto kmersTable = kmers.lock_table();
  ostringstream ss;
  string kmerStr;
  size_t kmerInt;
  char c[4] = {'A', 'C', 'G', 'T'};

  cerr << "Printing k-mers..." << endl;
  for (const auto& kmer: kmersTable) {
    if (kmer.second < minOcc)
      continue;
    kmerStr = "";
    kmerInt = kmer.first;
    for (size_t i = 0; i < kLen; ++i) {
      kmerStr += c[kmerInt % 4];
      kmerInt /= 4;
    }
    reverse(kmerStr.begin(), kmerStr.end());
    ss << ">" << kmer.second << "\n" << kmerStr << "\n";
  }
  out << ss.str();
}
