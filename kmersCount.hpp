#ifndef  KMERCOUNT_HPP_
# define KMERCOUNT_HPP_

using namespace std;

// count.cpp
void kmersCount(ifstream&, vector<unordered_map<size_t, size_t> >&, size_t&, size_t&);

// options.cpp
void setMaxThreads(string, size_t&);
void setKmerLength(string, size_t&, bool&);
void setSeed(string, size_t&, bool&);
void setInfile(string, ifstream &, bool&);
void setOutfile(string, ofstream &, bool&);
void setMinOcc(string, size_t &);

//maps.cpp
void printMap(unordered_map<size_t, size_t>&, size_t&, size_t &, ostream&);
void mergeMaps(vector<unordered_map<size_t, size_t> >&, size_t&);

#endif   /* KMERCOUNT_HPP_*/
