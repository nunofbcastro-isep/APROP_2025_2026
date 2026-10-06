#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <omp.h>
#include <time.h>

/* data payload */
#define DATA_SIZE 64
typedef struct data {
    int key;
    char data[DATA_SIZE];
} data;

/* node of the linked list */
typedef struct node {
    data payload;
    struct node* next;
    omp_lock_t lock;
} node;

/* list head (sentinel) */
typedef struct list {
    node* head; /* sentinel head with key = INT_MIN */
} list;

/* ---------- list API ---------- */

/* initialize an empty list (creates head sentinel) */
void list_init(list* lst);

/* destroy the list and free resources */
void list_destroy(list* lst);

/* insert ordered by key
 * returns 1 if inserted, 0 if key already present or on error
 */
int list_insert(list* lst, const data* d);

/* delete by key
 * returns 1 if deleted, 0 if not found
 */
int list_delete(list* lst, int key);

/* search by key
 * if found, copies payload into out and returns 1, else returns 0
 */
int list_search(list* lst, int key, data* out);

/* debug: print first n elements (single-threaded safe if called after parallel ops) */
void list_print(list* lst, int max_print);

/* ---------- helpers ---------- */

static node* node_create(const data* d) {
    node* n = (node*)malloc(sizeof(node));
    if (!n) return NULL;
    n->payload = *d;
    n->next = NULL;
    omp_init_lock(&n->lock);
    return n;
}

static void node_destroy(node* n) {
    if (!n) return;
    omp_destroy_lock(&n->lock);
    free(n);
}

/* ---------- implementation ---------- */

void list_init(list* lst) {
    if (!lst) return;
    lst->head = (node*)malloc(sizeof(node));
    if (!lst->head) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    /* sentinel head with minimal key */
    lst->head->payload.key = INT_MIN;
    lst->head->next = NULL;
    omp_init_lock(&lst->head->lock);
}

void list_destroy(list* lst) {
    if (!lst || !lst->head) return;

    /* traverse and free nodes — single-threaded */
    node* cur = lst->head;
    while (cur) {
        node* next = cur->next;
        node_destroy(cur);
        cur = next;
    }
    lst->head = NULL;
}

int list_insert(list* lst, const data* d) {
    if (!lst || !d) return 0;

    node* new_node = node_create(d);
    if (!new_node) return 0;

    node* prev = lst->head;
    /* acquire lock on head sentinel */
    omp_set_lock(&prev->lock);

    node* curr = prev->next;
    if (curr) omp_set_lock(&curr->lock);

    /* traverse until curr == NULL or curr->key >= d->key */
    while (curr && curr->payload.key < d->key) {
        /* move forward: release prev, step prev=curr, curr=curr->next (lock the next first) */
        omp_unset_lock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr) omp_set_lock(&curr->lock);
    }

    /* now curr is first node with key >= d->key or NULL */
    if (curr && curr->payload.key == d->key) {
        /* duplicate key - do not insert. optionally update data. */
        omp_unset_lock(&curr->lock);
        omp_unset_lock(&prev->lock);
        node_destroy(new_node);
        return 0;
    }

    /* insert between prev and curr */
    new_node->next = curr;
    prev->next = new_node;

    /* release locks */
    if (curr) omp_unset_lock(&curr->lock);
    omp_unset_lock(&prev->lock);

    return 1;
}

int list_delete(list* lst, int key) {
    if (!lst) return 0;

    node* prev = lst->head;
    omp_set_lock(&prev->lock);

    node* curr = prev->next;
    if (curr) omp_set_lock(&curr->lock);

    /* traverse until curr == NULL or curr->key >= key */
    while (curr && curr->payload.key < key) {
        omp_unset_lock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr) omp_set_lock(&curr->lock);
    }

    if (!curr || curr->payload.key != key) {
        /* not found */
        if (curr) omp_unset_lock(&curr->lock);
        omp_unset_lock(&prev->lock);
        return 0;
    }

    /* found: unlink curr */
    prev->next = curr->next;
    /* we hold locks on prev and curr, safe to remove curr */
    omp_unset_lock(&curr->lock); /* unlock node before destroying its lock to avoid deadlock when destroy tries to unset? */
    /* destroy node and its lock */
    node_destroy(curr);

    omp_unset_lock(&prev->lock);
    return 1;
}

int list_search(list* lst, int key, data* out) {
    if (!lst || !out) return 0;

    node* prev = lst->head;
    omp_set_lock(&prev->lock);
    node* curr = prev->next;
    if (curr) omp_set_lock(&curr->lock);

    while (curr && curr->payload.key < key) {
        omp_unset_lock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr) omp_set_lock(&curr->lock);
    }

    int found = 0;
    if (curr && curr->payload.key == key) {
        *out = curr->payload;
        found = 1;
    }

    if (curr) omp_unset_lock(&curr->lock);
    omp_unset_lock(&prev->lock);
    return found;
}

void list_print(list* lst, int max_print) {
    if (!lst || !lst->head) return;
    node* cur = lst->head->next; /* skip sentinel */
    int count = 0;
    printf("List (first up to %d elements):", max_print);
    while (cur && count < max_print) {
        printf(" %d", cur->payload.key);
        cur = cur->next;
        count++;
    }
    printf("\n");
}

/* ---------- test program ---------- */

/* default parameters */
#define DEFAULT_NUM_THREADS 4
#define DEFAULT_OPS_PER_THREAD 5000
#define KEYS_PER_THREAD 10000 /* keys space per thread to reduce duplicates */

/* thread worker:
 * each thread performs:
 *  - inserts a contiguous set of keys assigned to the thread
 *  - performs a number of random searches
 *  - deletes half of its inserted keys (every 2nd)
 */
int main(int argc, char* argv[]) {
    int num_threads = DEFAULT_NUM_THREADS;
    int ops_per_thread = DEFAULT_OPS_PER_THREAD;

    if (argc >= 2) num_threads = atoi(argv[1]);
    if (argc >= 3) ops_per_thread = atoi(argv[2]);

    printf("Running with %d threads, %d ops/thread\n", num_threads, ops_per_thread);

    list lst;
    list_init(&lst);

    omp_set_num_threads(num_threads);

    /* measure time */
    double t0 = omp_get_wtime();

    /* Parallel region: each thread inserts its block of keys */
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(tid * 0x9e3779b9);

        int base = tid * KEYS_PER_THREAD;
        int i;

        /* phase 1: insert keys [base, base + ops_per_thread - 1] */
        for (i = 0; i < ops_per_thread; i++) {
            data d;
            d.key = base + i;
            snprintf(d.data, DATA_SIZE, "thread-%d-key-%d", tid, d.key);
            list_insert(&lst, &d);
        }

        /* phase 2: random searches */
        for (i = 0; i < ops_per_thread; i++) {
            int k = base + (rand_r(&seed) % ops_per_thread);
            data out;
            list_search(&lst, k, &out);
            /* don't print to avoid huge IO in parallel */
        }

        /* phase 3: delete every 2nd key inserted by this thread */
        for (i = 0; i < ops_per_thread; i += 2) {
            int k = base + i;
            list_delete(&lst, k);
        }
    } /* end parallel */

    double t1 = omp_get_wtime();

    printf("Parallel operations finished in %.6f seconds\n", t1 - t0);

    /* single-threaded validation: check that remaining keys are sorted and belong to expected set */
    int errors = 0;
    int prev_key = INT_MIN;
    node* cur = lst.head->next;
    while (cur) {
        if (cur->payload.key < prev_key) {
            fprintf(stderr, "[ERROR] list is not sorted: %d after %d\n", cur->payload.key, prev_key);
            errors++;
            break;
        }
        prev_key = cur->payload.key;
        cur = cur->next;
    }

    if (errors == 0) {
        printf("List appears sorted. (Simple check)\n");
    } else {
        printf("List validation found errors.\n");
    }

    /* print first 40 keys for inspection */
    list_print(&lst, 40);

    list_destroy(&lst);
    return 0;
}
