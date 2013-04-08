#include <assert.h>
#include <sys/time.h>
#include "test.h"

static test_t * root = 0;

int test_run_all() {
  int passes = 0, failures = 0;
  for(test_t * test=root; test; test=test->next) {

    struct timeval start; gettimeofday(&start, 0);
    int pass = 0, fail = 0; test->run(&pass, &fail);
    struct timeval end; gettimeofday(&end, 0);

    double elapsed = end.tv_sec - start.tv_sec + 1e-6*(end.tv_usec - start.tv_usec);
    int total = pass + fail + !(pass+fail);
    printf("%-33s %d/%d (%0.2lf%%) [%g ms]\n", test->name, pass, total, 100.0*pass/total, 1000*elapsed);

    if (fail > 0) {
      ++failures;
    } else {
      ++passes;
    }
  }
  printf("%-33s %d/%d (%0.2lf%%)\n", "OVERALL", passes, passes+failures, 
         100.0*passes/(passes+failures+!(passes+failures)));
  return failures;
}

void test_register(test_t * test) {
  assert(test);
  test->next = root;
  root = test;
}
