#include <assert.h>
#include "test.h"

static test_t * root = 0;

int test_run_all() {
  int failures = 0;
  for(test_t * test=root; test; test=test->next) {
    printf("running %s...\n", test->name);

    int pass = 0, fail = 0; test->run(&pass, &fail);
    int total = pass + fail + !(pass+fail);
    printf("  %d/%d (%0.2lf%%)\n", pass, total, 100.0*pass/total);

    if (fail > 0) {
      ++failures;
    }
  }
  return failures;
}

void test_register(test_t * test) {
  assert(test);
  test->next = root;
  root = test;
}
