# Linear-Time Suffix Tree construction

Straightforward implementation of the algorithm:

  E. Ukkonen: Constructing suffix trees on-line in linear time.
  Proc. Information Processing 92, Vol. 1, IFIP Transactions A-12,
  484-492, Elsevier 1992.

Unlike other implementations out there, this one follows very closely the paper; in particular, the pseudocode of test-and-split, canonize, and update functions in the paper has been adapted almost verbatim in C++.
