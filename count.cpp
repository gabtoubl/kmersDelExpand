#include <vector>
#include <string>
#include <future>
#include <fstream>
#include <iostream>

using namespace std;

extern size_t k;
extern string seed;

void computeKmers(hash_map<size_t> &kmers, vector<string> lines, size_t limitLine) {
  size_t badNuc, kmerInt;
  string kmer, line;
  bool isBadNuc;

  k = seed == "" ? k : seed.length();
  for (size_t l = 0; l < limitLine; ++l) {
    line = lines[l];
    isBadNuc = (line.find_first_not_of("ACGT") != string::npos);
    for (size_t i = 0; i + k <= line.length(); ++i) {
      kmerInt = 0;
      kmer = "";
      for (size_t j = 0; j < k; ++j)
	if (seed[j] != '0')
	  kmer += line[i + j];
      if (isBadNuc && (badNuc = kmer.find_first_not_of("ACGT")) != string::npos) {
	if (seed == "")
	  i += badNuc;
      }
      else {
	for (auto&& c: kmer) {
	  kmerInt *= 4;
	  switch (c) {
	  case 'C': kmerInt += 1; break;
	  case 'G': kmerInt += 2; break;
	  case 'T': kmerInt += 3; break;
	  }
	}
	kmers.upsert(kmerInt, [](size_t &n) {++n;}, 1);
      }
    }
  }
}

void computeKmers(hash_map<string> &kmers, vector<string> lines, size_t limitLine) {
  size_t badNuc;
  string kmer, line;
  bool isBadNuc;

  k = seed == "" ? k : seed.length();
  for (size_t l = 0; l < limitLine; ++l) {
    line = lines[l];
    isBadNuc = (line.find_first_not_of("ACGT") != string::npos);
    for (size_t i = 0; i + k <= line.length(); ++i) {
      kmer = "";
      for (size_t j = 0; j < k; ++j)
	if (seed[j] != '0')
	  kmer += line[i + j];
      if (isBadNuc && (badNuc = kmer.find_first_not_of("ACGT")) != string::npos) {
	if (seed == "")
	  i += badNuc;
      }
      else
	kmers.upsert(kmer, [](size_t &n) {++n;}, 1);
    }
  }
}

template <typename T>
static void startNewThread(hash_map<T> &kmers, vector<string> &chunk, future<void> &fut,
			   vector<bool> &inUse, size_t &curThread, size_t &curLine) {
  if (inUse[curThread]) {
    fut.get();
    cerr << "Thread " << curThread << " complete"<< endl;
  }
  cerr << "Starting thread " << curThread << endl;
  fut = async(launch::async, [&kmers, chunk, curLine]() mutable {
      computeKmers(kmers, chunk, curLine);
    });
  inUse[curThread] = true;
}

template <typename T>
void kmersCount(ifstream &infile, hash_map<T> &kmers,
		size_t &maxThreads, size_t &maxLine) {
  vector<vector<string> > chunks;
  vector<future<void> > futs(maxThreads);
  vector<bool> inUse;
  size_t curThread = 0, curLine = 0;

  if (seed != "")
    cerr << "Starting with seed " << seed;
  else
    cerr << "Starting with " << k << "-mers";
  cerr << " with " << maxThreads << " threads" << endl << "Counting k-mers..." << endl;
  for (size_t i = 0; i < maxThreads; ++i) {
    chunks.push_back(vector<string>(maxLine));
    inUse.push_back(false);
  }
  while (getline(infile, chunks[curThread][curLine])) {
    getline(infile, chunks[curThread][curLine]);
    ++curLine;
    if (curLine == maxLine) {
      startNewThread(kmers, chunks[curThread], futs[curThread], inUse, curThread, curLine);
      curLine = 0;
      curThread = (curThread + 1) % maxThreads;
    }
  }
  if (curLine && curLine != maxLine)
    startNewThread(kmers, chunks[curThread], futs[curThread], inUse, curThread, curLine);
  for (size_t i = 0; i < maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
    }
}
