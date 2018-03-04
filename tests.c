#define TEST_HASHTABLE
#ifdef TEST_HASHTABLE

#include <assert.h>
#include <stdio.h>
#include "hashtable.h"

int ht_tests_passed = 0;
int ht_tests_run = 0;

void assert_(int x, const char* str)
{
  if (!x) {
    int failures = ht_tests_run - ht_tests_passed;
    if (failures < 20) {
      printf("  FAILED: %s\n", str);
    } else if (failures == 20) {
      printf("THERE ARE MORE FAILURES.\n");
    }
  } else {
    ht_tests_passed++;
  }
  ht_tests_run++;
}

void check_invariants(hashtable_t* ht)
{
  assert_(ht->size <= ht->capacity, "size <= capacity");
  assert_(ht->capacity == (1 << ht->capacity_log), "capacity log correct");
  assert_(ht->overflow_heap_ptr <= ht->capacity, "overflow_ptr <= capacity");
}

void test_create()
{
  printf("Tests: ht_create()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  assert_(ht.size == 0, "size == 0");
  assert_(ht.capacity > 0, "capacity > 0");
  check_invariants(&ht);
  int state = 0;
  for (size_t k = 0; k < ht.capacity; k++) {
    state |= ht.backend[k].state;
  }
  assert_(state == 0, "all buckets empty");
  ht_destroy(&ht);
}

void test_destroy()
{
  printf("Tests: ht_destroy()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  ht_destroy(&ht);
  assert_(ht.size == 0, "size == 0");
  assert_(ht.capacity == 0, "capacity == 0");
  assert_(ht.backend == NULL, "backend is NULL");
  assert_(ht.overflow_heap == NULL, "overflow heap is NULL");
}

void test_insert()
{
  int data1 = 5;
  int data2 = 7;
  void* dp1 = &data1;
  void* dp2 = &data2;

  printf("Tests: ht_insert()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  ht_insert(&ht, 137, dp1);
  assert_(ht_contains(&ht, 137), "137 in table");
  assert_(ht_find(&ht, 137) == dp1, "t[137] contains correct value");
  check_invariants(&ht);
  ht_insert(&ht, 78121, dp2);
  assert_(ht_contains(&ht, 78121), "78121 in table");
  assert_(ht_find(&ht, 78121) == dp2, "t[78121] contains correct value");
  check_invariants(&ht);
  ht_destroy(&ht);
}

void test_find()
{
  int data1 = 5;
  void* dp1 = &data1;

  printf("Tests: ht_find()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  assert_(ht_find(&ht, 125) == 0, "125 not in table");
  ht_insert(&ht, 125, dp1);
  check_invariants(&ht);
  assert_(ht_find(&ht, 125) == dp1, "t[125] contains correct value");
  ht_destroy(&ht);
}

void test_contains()
{
  int data1 = 5;
  void* dp1 = &data1;

  printf("Tests: ht_contains()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  assert_(!ht_contains(&ht, 125), "125 not in table");
  ht_insert(&ht, 125, dp1);
  check_invariants(&ht);
  assert_(ht_contains(&ht, 125), "t[125] contains correct value");
  ht_destroy(&ht);
}

void test_remove()
{
  int data1 = 5;
  void* dp1 = &data1;

  printf("Tests: ht_remove()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  assert_(ht_find(&ht, 125) == 0, "125 not in table");
  ht_insert(&ht, 125, dp1);
  check_invariants(&ht);
  assert_(ht_find(&ht, 125) == dp1, "t[125] contains correct value");
  ht_remove(&ht, 125);
  check_invariants(&ht);
  assert_(ht_find(&ht, 125) == 0, "125 not in table");
  ht_destroy(&ht);
}

void test_size()
{
  int data1 = 0;
  void* dp1 = &data1;

  printf("Tests: ht_size()\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  assert_(ht_size(&ht) == 0, "size is 0");
  for (int k = 0; k < 1000; k++) {
    ht_insert(&ht, 2 * k + 1, dp1 + k);
    assert_(ht_size(&ht) == k + 1, "size is k+1");
    check_invariants(&ht);
  }
  for (int k = 0; k < 1000; k++) {
    ht_remove(&ht, 2 * k + 1);
    assert_(ht_size(&ht) == 1000 - k - 1, "size is 1000-k-1");
    check_invariants(&ht);
  }
  ht_destroy(&ht);
}

void test_large_data()
{
  int data1 = 0;
  void* dp1 = &data1;

  printf("Tests: Large amounts of data\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);
  assert_(ht_size(&ht) == 0, "size is 0");
  for (int k = 0; k < 100000; k++) {
    assert_(!ht_contains(&ht, 3 * k + 1), "key not yet in table");
    assert_(ht_find(&ht, 3 * k + 1) == 0, "key not yet in table (find)");
    ht_insert(&ht, 3 * k + 1, dp1 + k);
    assert_(ht_contains(&ht, 3 * k + 1), "key in table");
    assert_(ht_find(&ht, 3 * k + 1) == dp1 + k, "key in table (find)");
    assert_(ht_size(&ht) == k + 1, "size is k+1");
    check_invariants(&ht);
  }
  for (int k = 0; k < 100000; k++) {
    ht_remove(&ht, 3 * k + 1);
    assert_(ht_size(&ht) == 100000 - k - 1, "size is 1000-k-1");
    assert_(!ht_contains(&ht, 3 * k + 1), "key not in table anymore");
    assert_(ht_find(&ht, 3 * k + 1) == 0, "key not in table (find) anymore");
    check_invariants(&ht);
  }
  ht_destroy(&ht);
}

void test_adversary()
{
  int data1 = 0;
  void* dp1 = &data1;

  printf("Tests: Adversary operation sequence\n");
  hashtable_t ht;
  ht_create(&ht, NULL, NULL);
  check_invariants(&ht);

  // Waste overflow nodes.
  for (int k = 0; k < 10000; k++) {
    assert_(ht_size(&ht) == 0, "size is 0");
    for (int j = 0; j < 95; j++) {
      ht_insert(&ht, j, dp1);
      check_invariants(&ht);
    }
    for (int j = 0; j < 95; j++) {
      ht_remove(&ht, j);
      check_invariants(&ht);
    }
  }
  for (int j = 0; j < 95; j++) {
    ht_insert(&ht, j, dp1);
    check_invariants(&ht);
  }

  hashtable_t baseline;
  ht_create(&baseline, NULL, NULL);
  check_invariants(&baseline);
  for (int j = 0; j < 95; j++) {
    ht_insert(&baseline, j, dp1);
    check_invariants(&baseline);
  }

  // Check that the memory usage is still in the same order.
  assert_(ht.size == baseline.size, "both tables have same size");
  assert_(ht.capacity <= 2 * baseline.capacity, "same order of capacity");

  ht_destroy(&ht);
  ht_destroy(&baseline);
}

int main(int argc, char **argv)
{
  test_create();
  test_destroy();
  test_insert();
  test_find();
  test_contains();
  test_remove();
  test_size();
  test_large_data();
  test_adversary();
  if (ht_tests_passed == ht_tests_run) {
    printf("== PASSED ALL %i ASSERTIONS ==\n", ht_tests_run);
    return 0;
  } else {
    printf("== FAILED %i/%i ASSERTIONS ==\n", ht_tests_run - ht_tests_passed,
      ht_tests_run);
    return 1;
  }
}

#endif  // TEST_HASHTABLE
