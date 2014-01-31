#!/bin/bash

( cd .. && make )

for i in {1..4}; do
  echo
  echo test_$i
  ( time ../bin/msa -p test_$i/readsInput$i.fasta test_$i/layouts$i.afg ) \
          | tee log/msa.log.$i
  ../bin/count-n consensus.fasta | tee --append log/msa.log.$i
  mv consensus.fasta log/consensus.test_$i.fasta
done
