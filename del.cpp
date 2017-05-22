#include "kmers.hpp"
using namespace std;

int main(int ac, char **av) {
  Options opt;

  if (manageOpt(ac, av, opt) != EXIT_SUCCESS)
    return EXIT_FAILURE;
  opt.seedLen = opt.seed.length();
  kmersExec(opt, &kmersDel,
	    opt.flagInfile ? opt.infile : cin,
	    opt.flagOutfile ? opt.outfile : cout);
  return EXIT_SUCCESS;
}
