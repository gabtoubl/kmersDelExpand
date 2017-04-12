#include <unordered_map>
#include <iostream>
#include <fstream>	// ifstream and ofstream
#include <string>
#include <vector>
#include <algorithm>	// count
#include <future>	// async and future
#include <unistd.h>	// getopt

using namespace std;

size_t k = 16;
size_t kS = 0;
string seed = "";

void computeKmers(unordered_map<size_t, int> &kmers, vector<string> lines,
		  size_t limitLine) {
  size_t found, kmerInt;
  string kmer, line;

  k = seed.length() > 0 ? seed.length() : k;
  for (size_t l = 0; l < limitLine; ++l) {
    line = lines[l];
    for (size_t i = 0; i + k <= line.length(); ++i) {
      kmerInt = 0;
      kmer = "";
      if (seed == "")
	kmer = line.substr(i, k);
      else
	for (size_t j = 0; j < k; ++j)
	  if (seed[j] != '0')
	    kmer += line[i + j];
      if ((found = kmer.find_first_not_of("ACGT")) != string::npos) {
	if (seed == "")
	  i += found;
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
	if (!kS)
	  kS = seed == "" ? k : kmer.length();
	kmers[kmerInt] += 1;
      }
    }
  }
}

void kmersCount(ifstream &infile, vector<unordered_map<size_t, int> > &kmers,
		size_t maxThreads, size_t maxLine) {
  vector<vector<string> > chunks;
  vector<future<void> > futs(maxThreads);
  vector<bool> inUse;
  size_t curThread = 0, curLine = 0;

  for (size_t i = 0; i < maxThreads; ++i) {
    chunks.push_back(vector<string>(maxLine));
    kmers.push_back(unordered_map<size_t, int>());
    inUse.push_back(false);
  }
  while (getline(infile, chunks[curThread][curLine])) {
    getline(infile, chunks[curThread][curLine]);
    ++curLine;
    if (curLine == maxLine) {
      if (inUse[curThread]) {
	futs[curThread].get();
	inUse[curThread] = false;
	cerr << "Thread " << curThread << " complete"<< endl;
      }
      cerr << "Starting thread " << curThread << endl;
      futs[curThread] = async(launch::async, computeKmers, ref(kmers[curThread]), chunks[curThread], maxLine);
      inUse[curThread] = true;
      curLine = 0;
      curThread = (curThread + 1) % maxThreads;
    }
  }
  if (curLine != maxLine) {
    if (inUse[curThread])
      futs[curThread].get();
    cerr << "Starting thread " << curThread << endl;
    futs[curThread] = async(launch::async, computeKmers, ref(kmers[curThread]), chunks[curThread], curLine);
  }
  for (size_t i = 0; i < maxThreads; ++i)
    if (inUse[i]) {
      futs[i].get();
      cerr << "Thread " << i << " complete"<< endl;
    }
}

void printKmers(unordered_map<size_t, int> &kmers, ostream &os) {
  string kmerStr;
  size_t kI;
  char c[4] = {'A', 'C', 'G', 'T'};

  for (const auto& kmer: kmers) {
    kmerStr = "";
    kI = kmer.first;
    for (size_t i = 0; i < kS; ++i) {
      kmerStr += c[kI % 4];
      kI /= 4;
    }
    reverse(kmerStr.begin(), kmerStr.end());
    os << ">" << kmer.second << endl
       << kmerStr << endl;
  }
}

int usage() {
  cerr << "usage: ./kmersCount OPTIONS" << endl
       << "-> mandatory: -i INFILE\tInput File in FASTA format" << endl
       << "              -k KMERLEN\tLength of k-mers (either -k or -s)" << endl
       << "              -s KMERSEED\tSeed for spaced kmers (either -k or -s)" << endl
       << "-> optional : -o OUTFILE\tOutput File, default stdout" << endl
       << "              -j THREADS\tNumber of threads, default 1" << endl
       << "              -h\t\tHelp: This usage message" << endl;
  return 1;
}

int main(int ac, char **av) {
  bool flagInFile = false, flagOutFile = false, flagOpt = false;
  char opt;
  vector<unordered_map<size_t, int> > kmers;
  ifstream infile;
  ofstream outfile;
  size_t maxThreads = 1, maxLine;

  while ((opt = getopt(ac, av, "j:k:s:i:o:h")) != -1) {
    switch (opt) {
    case 'j':
      maxThreads = stoi(optarg);
      maxThreads = maxThreads < 1 ? 1 : maxThreads;
      break;
    case 'k':
      flagOpt = true;
      k = stoi(optarg);
      k = k > 32 ? 32 : k;
      break;
    case 's':
      flagOpt ^= true;
      seed = optarg;
      break;
    case 'i':
      flagInFile = true;
      infile.open(optarg);
      if (!infile.is_open()) {
	cerr << "Error: " << optarg << " could not be opened" << endl;
	return 1;
      }
      break;
    case 'o':
      flagOutFile = true;
      outfile.open(optarg);
      if (!outfile.is_open()) {
	cerr << "Error: " << optarg << " could not be opened" << endl;
	return 1;
      }
      break;
    case 'h':
    default :
      return usage();
      break;
    }
  }
  if (!flagInFile || !flagOpt)
    return usage();
  maxLine = count(istreambuf_iterator<char>(infile),
		  istreambuf_iterator<char>(), '\n') / 2;
  infile.clear();
  infile.seekg(0);
  maxLine = (maxLine - maxLine % maxThreads) / maxThreads;
  maxLine = maxLine > 200000 ? 200000 : maxLine < 1 ? 1 : maxLine;
  cerr << "Counting k-mers..." << endl;
  kmersCount(infile, kmers, maxThreads, maxLine);
  cerr << "Merging maps together..." << endl;
  for (size_t i = 1; i < maxThreads; ++i)
    for (auto&& k: kmers[i])
      kmers[0][k.first] += k.second;
  cerr << "Printing merged map..." << endl;
  printKmers(kmers[0], flagOutFile ? outfile : cout);
  infile.close();
  if (flagOutFile)
    outfile.close();
  return 0;
}
