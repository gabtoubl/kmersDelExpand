#ifndef KMERCOUNT_HPP_
# define KMERCOUNT_HPP_
# include <libcuckoo/cuckoohash_map.hh>

using namespace std;
template <typename T>
using hash_map = cuckoohash_map<T, size_t>;

// count.cpp
template <typename T>
void kmersCount(istream&, hash_map<T>&, size_t&, size_t&);
#include "count.cpp"

// options.cpp
void setMaxThreads(string, size_t&);
void setKmerLength(string, size_t&, bool&);
void setSeed(string, size_t&, bool&);
void setInfile(string, ifstream &, bool&);
void setOutfile(string, ofstream &, bool&);
void setMinOcc(string, size_t &);

// print.cpp
template <typename T>
void printMap(hash_map<T>&, size_t&, size_t &, ostream&);
#include "print.cpp"

#endif /* KMERCOUNT_HPP_*/
