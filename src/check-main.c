#include "test.h"

TEST(add) {
  expect(1+1 == 2);
  require(1+1 == 3);
  expect(1+1 == 2);
}

int main(int argc, char ** argv) {
  return test_run_all();
}
