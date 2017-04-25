#ifndef  KMERCOUNT_HPP_
# define KMERCOUNT_HPP_
# include <libcuckoo/cuckoohash_map.hh>

using namespace std;
typedef cuckoohash_map<size_t, size_t> hash_map;

// count.cpp
void kmersCount(ifstream&, hash_map&, size_t&, size_t&);

// options.cpp
void setMaxThreads(string, size_t&);
void setKmerLength(string, size_t&, bool&);
void setSeed(string, size_t&, bool&);
void setInfile(string, ifstream &, bool&);
void setOutfile(string, ofstream &, bool&);
void setMinOcc(string, size_t &);

//maps.cpp
void printMap(hash_map&, size_t&, size_t &, ostream&);

#endif   /* KMERCOUNT_HPP_*/
