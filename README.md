Poker Hand Isomorphisms
=======================

author: Kevin Waugh (waugh@cs.cmu.edu)
date: April 7, 2013

This repository contains a C library for efficiently mapping poker hands to and
from a tight set of indices.  Poker hands are isomorphic with respect to
permutations of the suits and ordering within a betting round.  That is,
AsKs, KdAd and KhAh all map to the same index preflop.

Please see:

K. Waugh, 2013. A Fast and Optimal Hand Isomorphism Algorithm.  In the Second
Computer Poker and Imperfect Information Symposium at AAAI 

for more details, and src/hand_index.h for the API's description.
