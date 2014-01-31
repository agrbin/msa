#include "MultipleAligner.h"
#include <iostream>

#include "test.h"

using namespace acgt;
using namespace std;

void test0() {
  MultipleAligner MA;
  MA.addSequence("atcgtattttacgatcg", 0);
  MA.addSequence(        "tttcgatcg", 7);
  /*
  MA.addSequence("atcggatattta"     , 0);
  MA.addSequence("atctgatatctt"     , 1);
  MA.addSequence(              "ccg",14);
  */
  cout << MA.getConsensus() << endl;
}

void test1() {
  MultipleAligner MA;
  MA.addSequence("atcgtattttacgatcg", 0);
  MA.addSequence(         "ttcgatcg", 9);
  MA.addSequence("atcggatattta", 0);
  MA.addSequence("atctgatatctt", 0);
  MA.getConsensus();
}

/*
// these examples are a bit wrong. you'll have gaps in output!
void test2() {
  MultipleAligner MA;
  MA.addSequence("act-t", 0);
  MA.addSequence("act-t", 0);
  MA.addSequence("a-cct", 0);
  MA.addSequence("a-c-t", 0);
  MA.addSequence("a-t-t", 0);
  MA.addSequence("a-t-t", 0);
  int score;
  cout << MA.getConsensus(&score) << endl;
  EQ(score, 4);
}

void test3() {
  MultipleAligner MA;
  MA.addSequence("gact", 0);
  MA.addSequence("gact", 0);
  MA.addSequence("gact", 0);
  MA.addSequence("gc-t", 0);
  MA.addSequence("gc-t", 0);
  MA.addSequence("gc-t", 0);
  int score;
  cout << MA.getConsensus(&score) << endl;
  EQ(score, 3);
}

void test4() {
  MultipleAligner MA;
  MA.addSequence("--gact----", 0);
  MA.addSequence("gact-----------", 0);
  MA.getConsensus();
}
*/

int main() {
  test0();
  //test1();
  /*
  test2();
  test3();
  test4();
  */
  return 0;
}
