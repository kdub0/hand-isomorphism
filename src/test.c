#include <assert.h>
#include "test.h"

static test_t * root = 0;

int test_run_all() {
  int passes = 0, failures = 0;
  for(test_t * test=root; test; test=test->next) {

    int pass = 0, fail = 0; test->run(&pass, &fail);
    int total = pass + fail + !(pass+fail);
    printf("%s %d/%d (%0.2lf%%)\n", test->name, pass, total, 100.0*pass/total);

    if (fail > 0) {
      ++failures;
    } else {
      ++passes;
    }
  }
  printf("OVERALL %d/%d (%0.2lf%%)\n", passes, passes+failures, 
         100.0*passes/(passes+failures+!(passes+failures)));
  return failures;
}

void test_register(test_t * test) {
  assert(test);
  test->next = root;
  root = test;
}
