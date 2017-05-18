#include <vector>
#include <string>
#include <future>
#include <fstream>
#include <iostream>
#include <mutex>
#include "kmers.hpp"

using namespace std;

mutex mtx;
void delKmers(string &seed, vector<string> &chunks, size_t limitLine,
	      size_t seedLen, ostream &out) {
  size_t lineLen;
  string kmer, line;
  seedLen = seed.length();
  for (size_t l = 0; l < limitLine; ++l) {
    line = chunks[l];
    lineLen = line.length();
    for (size_t i = 0; i + seedLen <= lineLen; ++i) {
      kmer = "";
      for (size_t j = 0; j < seedLen; ++j)
	if (seed[j] != '0')
	  kmer += line[i + j];
      if (kmer.find_first_not_of("ACGT") == string::npos) {
	mtx.lock();
	out << ">k" << endl << kmer << endl;
	mtx.unlock();
      }
    }
  }
}

static void startNewThread(string &seed, vector<string> &chunk, future<void> &fut,
			   vector<bool> &inUse, size_t &curThread, size_t &curLine,
			   size_t &seedLen, ostream &out) {
  if (inUse[curThread]) {
    fut.get();
    cerr << "Thread " << curThread << " complete"<< endl;
  }
  cerr << "Starting thread " << curThread << endl;
  fut = async(launch::async, [&seed, &chunk, curLine, seedLen, &out]() mutable {
      delKmers(seed, chunk, curLine, seedLen, out);
    });
  inUse[curThread] = true;
}

void kmersDel(string &seed, size_t &maxThreads, size_t &maxLine,
	      size_t &seedLen, istream &in, ostream &out) {
  vector<future<void> > futs(maxThreads);
  vector<vector<string> > chunks;
  vector<bool> inUse;
  size_t curThread = 0, curLine = 0;

  cerr << "Starting with seed " << seed << " with " << maxThreads << " threads" << endl
       << "Counting k-mers..." << endl;
  for (size_t i = 0; i < maxThreads; ++i) {
    chunks.push_back(vector<string>(maxLine));
    inUse.push_back(false);
  }
  while (getline(in, chunks[curThread][curLine])) {
    getline(in, chunks[curThread][curLine]);
    ++curLine;
    if (curLine == maxLine) {
      startNewThread(seed, chunks[curThread], futs[curThread],
		     inUse, curThread, curLine, seedLen, out);
      curLine = 0;
      curThread = (curThread + 1) % maxThreads;
    }
  }
  if (curLine && curLine != maxLine)
    startNewThread(seed, chunks[curThread], futs[curThread],
		   inUse, curThread, curLine, seedLen, out);
  for (size_t i = 0; i < maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
    }
  cerr << "End of program" << endl;
}
