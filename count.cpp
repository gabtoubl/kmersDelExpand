#include <vector>
#include <string>
#include <future>
#include <fstream>
#include <iostream>
#include <mutex>

#include "kmersCount.hpp"

using namespace std;

extern size_t k;
extern string seed;

mutex mtx;

void delKmers(ostream &out, vector<string> lines, size_t limitLine) {
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
      else {
	mtx.lock();
	out << ">k" << endl << kmer << endl;
	mtx.unlock();
      }
    }
  }
}

static void startNewThread(vector<string> &chunk, future<void> &fut,
			   vector<bool> &inUse, size_t &curThread, size_t &curLine,
			   ostream &out) {
  if (inUse[curThread]) {
    fut.get();
    cerr << "Thread " << curThread << " complete"<< endl;
  }
  cerr << "Starting thread " << curThread << endl;
  fut = async(launch::async, [&out, chunk, curLine]() mutable {
      delKmers(out, chunk, curLine);
    });
  inUse[curThread] = true;
}

void kmersDel(istream &infile, size_t &maxThreads, size_t &maxLine, ostream &out) {
  vector<future<void> > futs(maxThreads);
  vector<vector<string> > chunks;
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
      startNewThread(chunks[curThread], futs[curThread], inUse, curThread, curLine, out);
      curLine = 0;
      curThread = (curThread + 1) % maxThreads;
    }
  }
  if (curLine && curLine != maxLine)
    startNewThread(chunks[curThread], futs[curThread], inUse, curThread, curLine, out);
  for (size_t i = 0; i < maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
    }
  cerr << "End of program" << endl;
}
