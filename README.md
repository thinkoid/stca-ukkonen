# Linear-Time Suffix Tree construction

Straightforward implementation of the algorithm:

  E. Ukkonen: Constructing suffix trees on-line in linear time.
  Proc. Information Processing 92, Vol. 1, IFIP Transactions A-12,
  484-492, Elsevier 1992.

AFAICT the implementation is somewhat unusual in that it follows very closely the paper; in particular, the implementation of test-and-split, canonize, and update functions described in pseudocode has been adapted almost verbatim in C++.
