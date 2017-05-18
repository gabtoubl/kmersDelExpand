#ifndef KMERS_HPP_
# define KMERS_HPP_

using namespace std;

// count.cpp
void kmersDel(string&, size_t&, size_t&, size_t&, istream&, ostream&);

// options.cpp
void setMaxThreads(string, size_t&);
void setSeed(string, string&, size_t&, bool&);
void setInfile(string, ifstream&, bool&);
void setOutfile(string, ofstream&, bool&);
void setMinOcc(string, size_t&);

#endif /* KMERS_HPP_*/
