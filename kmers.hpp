#ifndef KMERS_HPP_
# define KMERS_HPP_
# include <string>
# include <vector>
# include <fstream>
# include <iostream>

using namespace std;
using kmerFunc = void (string &, vector<string>&, size_t, size_t, ostream&);

class Options {
public:
  string seed;
  size_t seedLen;
  bool flagSeed;
  ifstream infile;
  bool flagInfile;
  ofstream outfile;
  bool flagOutfile;
  size_t maxThreads;
  size_t maxLines;
  Options() : flagSeed(false), flagInfile(false), flagOutfile(false) , maxThreads(1) {}
  ~Options() {
    if (flagInfile)
      infile.close();
    if (flagOutfile)
      outfile.close();
  }
};

// threads.cpp
void kmersExec(Options&, kmerFunc*, istream&, ostream&);
void kmersExpand(string &, vector<string>&, size_t, size_t, ostream&);
void kmersDel(string &, vector<string>&, size_t, size_t, ostream&);

// options.cpp
int manageOpt(int ac, char **av, Options&);

#endif /* KMERS_HPP_*/
