/**
 * test.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 7, 2013
 *
 * very simple unit testing.
 */
#ifndef _TEST_H_
#define _TEST_H_

#include <stdio.h>

/**
 * test structure
 */
typedef struct test_s {
  struct test_s * next;
  const char * name;
  void (*run)(int *, int *);
} test_t;

/**
 * preprocessor macro to turn a token into a string
 */
#ifndef TEXT
#  define _TEXT(x) #x
#  define TEXT(x)  _TEXT(x)
#endif

/**
 * create and register a test
 * @param name
 *
 * @example
 * TEST(add) {
 *   expect(1+1 == 2);
 *   require(1+1 == 3); // failure!
 *   expect(1+1 == 2);  // unreachable
 * }
 */
#define TEST(_name) \
 static void __test_##_name(int * pass, int * fail); \
 static test_t __test_obj_##_name = { .name = TEXT(_name), .run = __test_##_name }; \
 static void __attribute__((constructor)) __test_register_##_name() { \
   test_register(&__test_obj_##_name); \
 } \
 static void __test_##_name(int * __pass, int * __fail) 

/**
 * expect p to be true, continues running test on failure
 * @param p
 */
#define expect(p) \
  if (!(p)) { \
    fprintf(stderr, "%s:%d: Expectation failed `%s'.\n", __FILE__, __LINE__, TEXT(p)); \
    ++(*__fail); \
  } else { \
    ++(*__pass); \
  }

/**
 * require p to be true, test stops on failure
 * @param p
 */
#define require(p) \
  if (!(p)) { \
    fprintf(stderr, "%s:%d: Expectation failed `%s'.\n", __FILE__, __LINE__, TEXT(p)); \
    ++(*__fail); return; \
  } else { \
    ++(*__pass); \
  }

/**
 * run all tests
 * @returns number of failed tests
 */
int test_run_all();

/**
 * register a test
 * @param test
 */
void test_register(test_t * test);

#endif
