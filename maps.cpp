#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void printMap(unordered_map<size_t, size_t> &kmers, size_t &kLen,
	      size_t &minOcc, ostream &out) {
  string kmerStr;
  size_t kmerInt;
  char c[4] = {'A', 'C', 'G', 'T'};

  cerr << "Printing merged map..." << endl;
  for (const auto& kmer: kmers) {
    if (kmer.second < minOcc)
      continue;
    kmerStr = "";
    kmerInt = kmer.first;
    for (size_t i = 0; i < kLen; ++i) {
      kmerStr += c[kmerInt % 4];
      kmerInt /= 4;
    }
    reverse(kmerStr.begin(), kmerStr.end());
    out << ">" << kmer.second << endl
	<< kmerStr << endl;
  }
}

void mergeMaps(vector<unordered_map<size_t, size_t> > &kmers, size_t &maxThreads) {
  cerr << "Merging maps together..." << endl;
  for (size_t i = 1; i < maxThreads; ++i)
    for (auto&& k: kmers[i])
      kmers[0][k.first] += k.second;
}
