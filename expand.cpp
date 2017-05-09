#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <future>

#include "kmersCount.hpp"

using namespace std;

size_t k = 0;
string seed = "";

static void setMaxLine(ifstream &infile, size_t &maxLine, size_t &maxThreads) {
  maxLine = count(istreambuf_iterator<char>(infile),
		  istreambuf_iterator<char>(), '\n') / 2;
  infile.clear();
  infile.seekg(0);
  maxLine = (maxLine - maxLine % maxThreads) / maxThreads;
  maxLine = maxLine > 200000 ? 200000 : maxLine < 1 ? 1 : maxLine;
}

static int usage() {
  cerr << "usage: ./kmersExpand OPTIONS" << endl
       << "-> mandatory: -s KMERSEED Seed for expanded kmers" << endl
       << "-> optional:  -i INFILE   Input File in FASTA format, default cin" << endl
       << "              -o OUTFILE  Output File, default stdout" << endl
       << "              -j THREADS  Number of threads, default 1" << endl
       << "              -h          Help: This usage message" << endl;
  return EXIT_FAILURE;
}

static void getExpandedKmers(string res, ostringstream &ss, string kmer, size_t posSeed, size_t posKmer) {
  if (posSeed == seed.length()) {
    ss << ">expanded" << endl << res << endl;
    return;
  }
  if (seed[posSeed] == '1')
    getExpandedKmers(res + kmer[posKmer], ss, kmer, posSeed + 1, posKmer + 1);    
  else {
    getExpandedKmers(res + "A", ss, kmer, posSeed + 1, posKmer);
    getExpandedKmers(res + "C", ss, kmer, posSeed + 1, posKmer);
    getExpandedKmers(res + "G", ss, kmer, posSeed + 1, posKmer);
    getExpandedKmers(res + "T", ss, kmer, posSeed + 1, posKmer);
  }
}

static void expandKmers(ostringstream &ss, vector<string> lines, size_t limitLine, size_t kLen) {
  size_t badNuc;
  string kmer, line;
  bool isBadNuc;

  for (size_t l = 0; l < limitLine; ++l) {
    line = lines[l];
    isBadNuc = (line.find_first_not_of("ACGT") != string::npos);
    for (size_t i = 0; i + kLen <= line.length(); ++i) {
      kmer = line.substr(i, kLen);
      if (isBadNuc && (badNuc = kmer.find_first_not_of("ACGT")) != string::npos)
	i += badNuc;
      else
	getExpandedKmers("", ss, kmer, 0, 0);
    }
  }
}

static void startNewThread(ostringstream &ss, vector<string> &chunk, future<void> &fut,
			   vector<bool> &inUse, size_t &curThread, size_t &curLine,
			   size_t &kLen, ostream &out) {
  if (inUse[curThread]) {
    fut.get();
    cerr << "Thread " << curThread << " complete"<< endl;
    out << ss.str();
  }
  cerr << "Starting thread " << curThread << endl;
  fut = async(launch::async, [&ss, chunk, curLine, kLen]() mutable {
      expandKmers(ss, chunk, curLine, kLen);
    });
  inUse[curThread] = true;
}

static void kmersExpand(istream &infile, size_t &maxThreads, size_t &maxLine,
			size_t &kLen, ostream &out) {
  vector<future<void> > futs(maxThreads);
  vector<ostringstream> ss(maxThreads);
  vector<vector<string> > chunks;
  vector<bool> inUse;
  size_t curThread = 0, curLine = 0;

  cerr << "Starting with seed " << seed << " with " << maxThreads << " threads" << endl
       << "Counting k-mers..." << endl;
  for (size_t i = 0; i < maxThreads; ++i) {
    chunks.push_back(vector<string>(maxLine));
    inUse.push_back(false);
  }
  while (getline(infile, chunks[curThread][curLine])) {
    getline(infile, chunks[curThread][curLine]);
    ++curLine;
    if (curLine == maxLine) {
      startNewThread(ss[curThread], chunks[curThread], futs[curThread],
		     inUse, curThread, curLine, kLen, out);
      curLine = 0;
      curThread = (curThread + 1) % maxThreads;
    }
  }
  if (curLine && curLine != maxLine)
    startNewThread(ss[curThread], chunks[curThread], futs[curThread],
		   inUse, curThread, curLine, kLen, out);
  for (size_t i = 0; i < maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
      out << ss[i].str();
    }
}

int main(int ac, char **av) {
  bool flagInfile = false, flagOutfile = false, flagSeed = false;
  size_t maxThreads = 1, maxLine = 100000, kLen;
  ifstream infile;
  ofstream outfile;
  char opt;

  while ((opt = getopt(ac, av, "j:s:i:o:h")) != -1) {
    switch (opt) {
    case 'j': setMaxThreads(optarg, maxThreads); break;
    case 's': setSeed(optarg, kLen, flagSeed); break;
    case 'i': setInfile(optarg, infile, flagInfile); break;
    case 'o': setOutfile(optarg, outfile, flagOutfile); break;
    case 'h':
    default : return usage();
    }
  }
  if (!flagSeed)
    return usage();
  if (!flagInfile)
    cerr << "Using Standard Input as FASTA file" << endl;
  else
    setMaxLine(infile, maxLine, maxThreads);
  kmersExpand(flagInfile ? infile : cin, maxThreads, maxLine, kLen,
	      flagOutfile ? outfile : cout);
  if (flagInfile)
    infile.close();
  if (flagOutfile)
    outfile.close();
  return EXIT_SUCCESS;
}
