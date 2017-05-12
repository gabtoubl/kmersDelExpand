#ifndef KMERCOUNT_HPP_
# define KMERCOUNT_HPP_

using namespace std;

// count.cpp
void kmersDel(istream&, size_t&, size_t&, ostream&);

// options.cpp
void setMaxThreads(string, size_t&);
void setKmerLength(string, size_t&, bool&);
void setSeed(string, size_t&, bool&);
void setInfile(string, ifstream &, bool&);
void setOutfile(string, ofstream &, bool&);
void setMinOcc(string, size_t &);

#endif /* KMERCOUNT_HPP_*/
