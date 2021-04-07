#include "user_tasks.h"

#ifdef SCHED_PRIORITY_QUEUE_USE_HEAP

/* What we do here is a kind of OCaml functor for C. It takes as argment
   (that must be defined earlier, possibly as static inline)

   - A prefix_elt_id_t type identifying elements in the array (e.g. an
     index, or a pointer to an element. Structs should also work, but
     involve more copies).
   - A prefix_priority_t type (e.g. an int).
   - A function to get the priorities of an element (prefix_get_priority)
   - A function to compare the priorities (prefix_is_gt_priority): > for max_heap, < for min_heap. */

#define INSTANTIATE_HEAP(prefix)                                        \
                                                                        \
prefix ## _priority_t prefix ## _get_priority(prefix ## _elt_id_t tid); \
__attribute__((noinline)) _Bool prefix ## _is_gt_priority(prefix ## _priority_t a, prefix ## _priority_t b); \
                                                                        \
struct prefix ## _heap {                                                \
  /* Number of elements currently in the heap. */                       \
  unsigned int size;                                                    \
  prefix ## _elt_id_t *  array;                                    \
};                                                                      \
                                                                        \
                                                                        \
                                                                        \
prefix ## _elt_id_t prefix ## _get_first(struct prefix ## _heap *heap){        \
  return heap->array[0];                                       \
}                                                               \
                                                                \
                                                \
                                                                        \
void prefix ## _insert_elt(struct prefix ## _heap *heap, prefix ## _elt_id_t elt){ \
  /* Temp */ assert(heap->size < user_tasks_image.nb_contexts);            \
  unsigned int i = heap->size++;                                        \
  prefix ## _priority_t priority = prefix ## _get_priority(elt);        \
  while(1){                                                             \
    if(i == 0) break;                 /* root */                        \
    unsigned int parent = (i - 1)/2;                                    \
    if(prefix ## _is_gt_priority(priority,prefix ## _get_priority(heap->array[parent]))) { \
      heap->array[i] = heap->array[parent];                             \
      i = parent;                                                       \
    }                                                                   \
    else break;                                                         \
  }                                                                     \
  heap->array[i] = elt;                                                 \
}                                                                       \
                                                                        \
prefix ## _elt_id_t prefix ## _remove_elt(struct prefix ## _heap *heap) {\
  prefix ## _elt_id_t res = heap->array[0];                             \
    /* Temp */ assert(heap->size <= user_tasks_image.nb_contexts);         \
  heap->array[0] = heap->array[--heap->size];                           \
                                                                        \
  unsigned int i = 0;                                                   \
  prefix ## _priority_t i_priority = prefix ## _get_priority(heap->array[i]); \
                                                                        \
  while(1){                                                             \
    unsigned int left = 2 * i + 1;                                      \
    unsigned int right = 2 * i + 2;                                     \
    /* Temp */ assert(i < user_tasks_image.nb_contexts);                   \
    unsigned int largest = i;                                           \
    prefix ## _priority_t largest_priority = i_priority;                \
    /* prefix ## _priority_t largest_priority = prefix ## _get_priority(heap->array[i]); */ \
                                                                        \
    if(right < heap->size){                                             \
      prefix ## _priority_t right_priority = prefix ## _get_priority(heap->array[right]); \
      if (prefix ## _is_gt_priority(right_priority,largest_priority)){  \
        largest = right;                                                \
        largest_priority = right_priority;                              \
      }                                                                 \
      /* As right is higher than left, we can skip the test. */         \
      goto skip;                                                        \
    }                                                                   \
    if(left < heap->size){                                              \
    skip:;                                                              \
      prefix ## _priority_t left_priority = prefix ## _get_priority(heap->array[left]); \
      if (prefix ## _is_gt_priority(left_priority,largest_priority)){   \
        largest = left;                                                 \
        largest_priority = left_priority;                               \
      }                                                                 \
    }                                                                   \
                                                                        \
    if(largest == i) return res;                                        \
    /* swap i and largest. */                                           \
    prefix ## _elt_id_t tmp = heap->array[i];                           \
    heap->array[i] = heap->array[largest];                              \
    heap->array[largest] = tmp;                                         \
                                                                        \
    i = largest;                                                        \
  }                                                                     \
}

#endif

#ifdef SCHED_PRIORITY_QUEUE_USE_LIST

#define INSTANTIATE_HEAP(prefix)                                                \
                                                                                \
prefix ## _priority_t prefix ## _get_priority(prefix ## _elt_id_t tid);         \
_Bool prefix ## _is_gt_priority(prefix ## _priority_t a, prefix ## _priority_t b); \
                                                                                \
struct prefix ## _heap {                                                        \
  unsigned int size;                                                                  \
  prefix ## _elt_id_t head;                                                       \
};                                                                              \
                                                                                \
__attribute__((noinline,optnone)) void prefix ## _insert_elt(struct prefix ## _heap *heap, prefix ## _elt_id_t elt) { \
  prefix ## _priority_t priority = prefix ## _get_priority(elt);                \
  if(!heap->head) {                                                             \
    heap->head = elt;                                                           \
    elt->sched_context.next = 0;                                                           \
    goto insert_end;                                                            \
  }                                                                             \
                                                                                \
  prefix ## _elt_id_t cur = heap->head, prev = 0;                              \
  while(cur) {                                                                  \
    if(prefix ## _is_gt_priority(priority, prefix ## _get_priority(cur))) { \
      if(prev)                                                                  \
        prev->sched_context.next = elt;                                                       \
      else                                                                      \
        heap->head = elt;                                                       \
      elt->sched_context.next = cur;                                                          \
      break;                                                                    \
    }                                                                           \
    prev = cur;                                                                 \
    cur = cur->sched_context.next;                                                            \
  }                                                                             \
  /* If not inserted yet, insert at the end */                                  \
  if(!cur) {                                                                    \
    prev->sched_context.next = elt;                                                           \
    elt->sched_context.next = 0;                                                           \
  }                                                                             \
insert_end:                                                                     \
  heap->size++;                                                                 \
}                                                                               \
                                                                                \
prefix ## _elt_id_t prefix ## _remove_elt(struct prefix ## _heap *heap) {       \
  assert(heap->head);                                                           \
  prefix ## _elt_id_t to_remove = heap->head;                               \
  heap->head = to_remove->sched_context.next;                                                 \
  heap->size--;                                                                 \
  return to_remove;                                                                   \
}                                                                               \
                                                                                \
prefix ## _elt_id_t prefix ## _get_first(struct prefix ## _heap *heap) {        \
  assert(heap->head);                                                           \
  return heap->head;                                                            \
}                                                                               \
                                                                                \
void prefix ## _heap_init(struct prefix ## _heap *heap) {                       \
  heap->head = 0;                                                            \
  heap->size = 0;                                                               \
}


#endif


#if 0
/* Unit test and example of instantiation. */

/* An instance of the heap. */
typedef int test_priority_t;
struct test_t {
  test_priority_t prio;
  struct { struct test_t *next; } sched_context;
};
typedef struct test_t * test_elt_id_t;
static inline test_priority_t test_get_priority(test_elt_id_t tid) {return tid->prio;}
static inline _Bool test_is_gt_priority(test_priority_t a, test_priority_t b) {
  return a > b;
}

INSTANTIATE_HEAP(test)

#include <stdio.h>
#include <assert.h>

#define NB_ELTS 8

struct test_t the_heap_array[NB_ELTS];
  
struct test_heap the_heap = {
    .size = 0,
    .head = 0
};

int after_heap;
struct test_heap *heap = &the_heap;
  
void check_is_a_heap(void){
  test_elt_id_t cur = heap->head;
  while(cur) {
    if(cur->sched_context.next && !test_is_gt_priority(cur->prio, cur->sched_context.next->prio)) {
      printf("FAILURE\n");
    }
    cur = cur->sched_context.next;
  }
}

void print_elt_array(void){
  printf("|");
  for(test_elt_id_t cur = heap->head; cur; cur = cur->sched_context.next){
    printf("%d|", cur->prio);
  }
  printf("\n");
}

void insert_with_prio(test_priority_t prio) {
  test_elt_id_t elt = &the_heap_array[heap->size];
  elt->prio = prio;
  test_insert_elt(heap, elt);
}

#include <limits.h>
#include <assert.h>

int main(void){
  print_elt_array();  
  insert_with_prio(88);
  print_elt_array();
  check_is_a_heap();
  insert_with_prio(66);
  print_elt_array();
  check_is_a_heap();
  insert_with_prio(44);
  print_elt_array();
  check_is_a_heap();  
  insert_with_prio(77);
  print_elt_array();
  check_is_a_heap();  
  insert_with_prio(11);
  print_elt_array();
  check_is_a_heap();  
  insert_with_prio(55);
  print_elt_array();
  check_is_a_heap();
  insert_with_prio(22);
  print_elt_array();
  check_is_a_heap();
  insert_with_prio(33);
  print_elt_array();
  check_is_a_heap();

  int last = INT_MAX;
  for(int i = 0; i < NB_ELTS; i++){
    int new = test_remove_elt(heap)->prio;
    printf("%d\n", new);
    print_elt_array();
    check_is_a_heap();
    assert(new <= last);
    last = new;
  }

  return 0;
}

#endif
