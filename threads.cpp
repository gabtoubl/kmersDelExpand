#include <mutex>
#include <future>
#include "kmers.hpp"
using namespace std;

mutex mtx;
static void kmersExpandRec(string &seed, string &kmer, string res,
			   size_t posSeed, size_t posKmer, ostream &out) {
  if (posSeed == seed.length()) {
    mtx.lock();
    out << ">k" << endl << res << endl;
    mtx.unlock();
  }
  else if (seed[posSeed] == '1')
    kmersExpandRec(seed, kmer, res + kmer[posKmer], posSeed + 1, posKmer + 1, out);
  else {
    kmersExpandRec(seed, kmer, res + "A", posSeed + 1, posKmer, out);
    kmersExpandRec(seed, kmer, res + "C", posSeed + 1, posKmer, out);
    kmersExpandRec(seed, kmer, res + "G", posSeed + 1, posKmer, out);
    kmersExpandRec(seed, kmer, res + "T", posSeed + 1, posKmer, out);
  }
}

void kmersExpand(string &seed, vector<string> &chunks, size_t limitLine,
		 size_t kLen, ostream &out) {
  size_t badNuc, lineLen;
  string kmer, line;
  bool isBadNuc;

  for (size_t l = 0; l < limitLine; ++l) {
    line = chunks[l];
    lineLen = line.length();
    isBadNuc = (line.find_first_not_of("ACGT") != string::npos);
    for (size_t i = 0; i + kLen <= lineLen; ++i) {
      kmer = line.substr(i, kLen);
      if (isBadNuc && (badNuc = kmer.find_first_not_of("ACGT")) != string::npos)
	i += badNuc;
      else
	kmersExpandRec(seed, kmer, "", 0, 0, out);
    }
  }
}

void kmersDel(string &seed, vector<string> &chunks, size_t limitLine,
	      size_t seedLen, ostream &out) {
  size_t lineLen;
  string kmer, line;

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
			   vector<bool> &inUse, size_t &curThread, size_t &curLine, size_t &kLen,
			   kmerFunc *func, ostream &out) {
  if (inUse[curThread]) {
    fut.get();
    cerr << "Thread " << curThread << " complete"<< endl;
  }
  cerr << "Starting thread " << curThread << endl;
  fut = async(launch::async, [func, &seed, &chunk, curLine, kLen, &out]() mutable {
      func(seed, chunk, curLine, kLen, out);
    });
  inUse[curThread] = true;
}

void kmersExec(Options &opt, kmerFunc *func, istream &in, ostream &out) {
  vector<future<void> > futs(opt.maxThreads);
  vector<vector<string> > chunks;
  vector<bool> inUse;
  size_t curThread = 0, curLine = 0;

  cerr << "Starting with seed " << opt.seed << " with " << opt.maxThreads << " threads" << endl
       << "Counting k-mers..." << endl;
  for (size_t i = 0; i < opt.maxThreads; ++i) {
    chunks.push_back(vector<string>(opt.maxLines));
    inUse.push_back(false);
  }
  while (getline(in, chunks[curThread][curLine])) {
    getline(in, chunks[curThread][curLine]);
    ++curLine;
    if (curLine == opt.maxLines) {
      startNewThread(opt.seed, chunks[curThread], futs[curThread],
		     inUse, curThread, curLine, opt.seedLen, func, out);
      curLine = 0;
      curThread = (curThread + 1) % opt.maxThreads;
    }
  }
  if (curLine && curLine != opt.maxLines)
    startNewThread(opt.seed, chunks[curThread], futs[curThread],
		   inUse, curThread, curLine, opt.seedLen, func, out);
  for (size_t i = 0; i < opt.maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
    }
  cerr << "End of program" << endl;
}
