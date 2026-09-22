#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>  //TODO const

#define ARROW_SIZE 20
#define MAX_NUMBER 50

#define ACTIVE true
#define UNACTIVE false

#define FORE_RED "\x1b[31m"
#define FORE_GRE "\x1b[32m"
#define FORE_YEL "\x1b[33m"
#define FORE_BLU "\x1b[34m"
#define FORE_MAG "\x1b[35m"

#define BACK_RED "\x1b[41m"
#define BACK_GRE "\x1b[42m"
#define BACK_YEL "\x1b[43m"
#define BACK_BLU "\x1b[44m"
#define BACK_MAG "\x1b[45m"

#define BACK_LIGHT_RED "\x1b[101m"
#define BACK_LIGHT_GRE "\x1b[102m"
#define BACK_LIGHT_YEL "\x1b[103m"
#define BACK_LIGHT_BLU "\x1b[104m"
#define BACK_LIGHT_MAG "\x1b[105m"

#define BOLD "\x1b[1m"

#define END "\x1b[0m"

#define SetColor(color) printf(color);
#define EndColor printf(END);

#define DEBUG

#ifdef DEBUG
    #define SHOWPROC(beg, end, showed_left_ptr, left_ptr_state, showed_right_ptr, right_ptr_state, element_size, message, pivot)\
            ShowPartitionProcess(beg, end, showed_left_ptr, left_ptr_state, showed_right_ptr, right_ptr_state, element_size, message, pivot);
#else
    #define SHOWPROC(beg, end, showed_left_ptr, left_ptr_state, showed_right_ptr, right_ptr_state, element_size, message, pivot)
#endif

bool MYqsort(void* beg, void* end, size_t element_size,
             void* Partition(void*, void*, size_t, int (*)(void*, void*)), int Compare(void*, void*));

void ShowPartitionProcess(void* beg, void* end, void* showed_left_ptr, bool left_ptr_state,
                          void* showed_right_ptr, bool right_ptr_state, size_t element_size,
                          const char* message, void* pivot);

void* LomutoPartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void* HoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void* UpgradedHoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void Swap(void* p_a, void* p_b, size_t element_size);

void WriteIn(void* p_a, void* p_b, size_t element_size);

void* SetPivot(void* p_value, size_t element_size);

int ToLowComparator(void* without_type_p_a, void* without_type_p_b);


int main() { //TODO split main on functions
    srand(time(NULL));
    int x = 0;

    int* arr = (int*)malloc(ARROW_SIZE * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr,"memory error");
        return 1;
    }

    int* brr = (int*)malloc(ARROW_SIZE * sizeof(int));
    if (brr == NULL) {
        fprintf(stderr, "memory error");
        free(arr);
        return 1;
    }

    int crr[] = {3, 4, 4, 4, 4, 4};

    for (int i = 0; i < ARROW_SIZE; i++) {
        x = rand() % MAX_NUMBER;
        arr[i] = x;
        brr[i] = x;
    }

    //MYqsort(arr, arr + ARROW_SIZE - 1, sizeof(int), LomutoPartition, ToLowComparator);
    MYqsort(brr, brr + ARROW_SIZE - 1, sizeof(int), UpgradedHoarePartition, ToLowComparator);
    //UpgradedHoarePartition(crr, crr + (sizeof(crr) / sizeof(int)) - 1, sizeof(int), ToLowComparator);

    printf("\n");
    /*for (int q = 0; q < ARROW_SIZE; q++)
        printf("%d ", arr[q]);
    printf("\n");*/
    for (int q = 0; q < ARROW_SIZE; q++)
        printf("%d ", brr[q]);
    printf("\n");
    /*for (int q = 0; q < sizeof(crr) / sizeof(int); q++)
        printf("%d ", crr[q]);
    printf("\n");*/

    free(arr);
    free(brr);
}


bool MYqsort(void* beg, void* end, size_t element_size,
             void* Partition(void*, void*, size_t, int (*)(void*, void*)), int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);
    assert(Partition != NULL);

    if (beg >= end)
        return 1;

    char* pivot = (char*)Partition(beg, end, element_size, Compare);

    MYqsort(beg, (void*)(pivot - element_size), element_size, Partition, Compare);
    MYqsort((void*)(pivot + element_size), end, element_size, Partition, Compare);
}


void ShowPartitionProcess(void* beg, void* end, void* showed_left_ptr, bool left_ptr_state,
                          void* showed_right_ptr, bool right_ptr_state, size_t element_size,
                          const char* message, void* pivot) {
    size_t title_size = (((char*)end - (char*)beg) / element_size + 1) * 4;
    size_t title_offset = (title_size > strlen(message)) ? (title_size - strlen(message)) / 2 : 0;
    for(size_t i = 0; i < title_offset; i++)
        putc('_', stdout);
    printf("%s", message);
    for(size_t i = 0; i < title_offset; i++)
        putc('_', stdout);
    printf("  pivot = %d", *(int*)pivot);     //NOTE - be care with this printf, it works only with int arrows
    printf("\n");

    void* similar_ptr = NULL;
    if (showed_left_ptr == showed_right_ptr)
        similar_ptr = showed_left_ptr;
    //printf("%p %p %p\n", similar_ptr, showed_left_ptr, showed_right_ptr);

    for (size_t i = 0; i <= ((char*)end - (char*)beg) / element_size; i++)
        printf("%-3zu ", i);
    printf("\n\n");

    for (char* i_ptr = (char*)beg; i_ptr <= end; i_ptr += element_size) {
        //printf("%p %p\n", i_ptr, end);
        if (i_ptr == similar_ptr) {
            SetColor(BOLD);
            SetColor(FORE_MAG);

            if (left_ptr_state && right_ptr_state)
                SetColor(BACK_LIGHT_MAG)
            else if (left_ptr_state)
                SetColor(BACK_LIGHT_BLU)
            else if (right_ptr_state)
                SetColor(BACK_LIGHT_RED)
        }
        else if (i_ptr == showed_left_ptr) {
            SetColor(FORE_BLU);

            if (left_ptr_state)
                SetColor(BACK_LIGHT_BLU)
        }
        else if (i_ptr == showed_right_ptr) {
            SetColor(FORE_RED);

            if (right_ptr_state)
                SetColor(BACK_LIGHT_RED)
        }

        printf("%-3d", *(int*)i_ptr);     //NOTE - be care with this printf, it works only with int arrows

        EndColor;
        printf(" ");
    }

    (void)getchar();
}


void* LomutoPartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* pos = (char*)beg;
    void* pivot = SetPivot(beg, element_size);

    for (char* i_ptr = (char*)beg + element_size; i_ptr <= end; i_ptr += element_size) {
        if (Compare((void*)i_ptr, (void*)pivot) < 0){
            pos += element_size;
            Swap(pos, i_ptr, element_size);
        }
    }
    WriteIn(beg, (void*)pos, element_size);
    WriteIn((void*)pos, (void*)pivot, element_size);

    free(pivot);
    return (void*)pos;
}


void* HoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* left = (char*)beg + element_size;
    char* right = (char*)end;
    void* pivot = SetPivot(beg, element_size);

    while (1) {
        for (; (left < right) && (Compare(left, pivot) <= 0); left += element_size);
        for (; (Compare(right, pivot) > 0); right -= element_size);
        if (left >= right)
            break;
        Swap(left, right, element_size);
        left += element_size;
        right -= element_size;
    }
    WriteIn(beg, (void*)right, element_size);
    WriteIn((void*)right, pivot, element_size);

    free(pivot);
    return (void*)right;
}


/*void* UpgradedHoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* left = (char*)beg;
    char* right = (char*)end;
    void* pivot = SetPivot(beg, element_size);

    while (1) {
        for(; Compare(right, pivot) > 0; right -= element_size);
        if (left >= right) {
            WriteIn((void*)left, pivot, element_size);
            free(pivot);
            return (void*)left;
        }

        WriteIn((void*)left, (void*)right, element_size);

        left += element_size;
        while (Compare(left, pivot) <= 0) {
            if (left >= right) {
                WriteIn((void*)left, pivot, element_size);
                free(pivot);
                return (void*)left;
            }
            left += element_size;
        }

        WriteIn((void*)right, (void*)left, element_size);
        right -= element_size;
    }
}*/


void* UpgradedHoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* left = (char*)beg;
    char* right = (char*)end;
    void* pivot = SetPivot(beg, element_size);

    while (1) {
        for(; Compare(right, pivot) > 0; right -= element_size)
            SHOWPROC(beg, end, left, ACTIVE, right, UNACTIVE, element_size, "finding bad right", pivot);
        SHOWPROC(beg, end, left, ACTIVE, right, ACTIVE, element_size, "bad right was found", pivot);
        if (left >= right) {
            WriteIn((void*)left, pivot, element_size);
            SHOWPROC(beg, end, left, UNACTIVE, right, UNACTIVE, element_size, "end condition, put pivot to left", pivot);
            free(pivot);
            return (void*)left;
        }

        WriteIn((void*)left, (void*)right, element_size);
        SHOWPROC(beg, end, left, UNACTIVE, right, ACTIVE, element_size, "putting to left was done", pivot);

        left += element_size;
        while (Compare(left, pivot) <= 0) {                                                            //NOT <= keep your attention on place of if (left >= right). you must to realize this example: {3,3,3,3,3,4,3,3,3,1}. If arrow has many similar element but one of values prevails this placement can slow down program or speed up it if there is no prevailing element. It can work only with Compare(right, pivot)>=0, otherwise we cant be sure that after left pointer walk there is element less than pivot at first field.
            SHOWPROC(beg, end, left, UNACTIVE, right, ACTIVE, element_size, "finding bad right", pivot);
            if (left >= right) {
                WriteIn((void*)left, pivot, element_size);
                SHOWPROC(beg, end, left, UNACTIVE, right, UNACTIVE, element_size, "end condition, put pivot to left", pivot);
                free(pivot);
                return (void*)left;
            }
            left += element_size;
        }
        SHOWPROC(beg, end, left, ACTIVE, right, ACTIVE, element_size, "bad left was found", pivot);

        WriteIn((void*)right, (void*)left, element_size);
        SHOWPROC(beg, end, left, ACTIVE, right, UNACTIVE, element_size, "putting to right was done", pivot);
        right -= element_size;
    }
}


void Swap(void* without_type_p_a, void* without_type_p_b, size_t element_size) {
    assert(without_type_p_a != NULL);
    assert(without_type_p_b != NULL);

    char* p_a = (char*)without_type_p_a;
    char* p_b = (char*)without_type_p_b;
    char mem;
    //printf("<%d, %d> - ", *(int*)without_type_p_a, *(int*)without_type_p_b);

    if (p_a != p_b) {
        for (size_t i = 0; i < element_size; i++) {
            mem = *p_a;
            *p_a = *p_b;
            *p_b = mem;

            p_a++;
            p_b++;
        }
    }
    //printf("<%d, %d>\n", *(int*)without_type_p_a, *(int*)without_type_p_b);
}


void WriteIn(void* without_type_p_a, void* without_type_p_b, size_t element_size) {
    assert(without_type_p_a != NULL);
    assert(without_type_p_b != NULL);

    char* p_a = (char*)without_type_p_a;
    char* p_b = (char*)without_type_p_b;
    //printf("<%d, %d> - ", *(int*)without_type_p_a, *(int*)without_type_p_b);

    if (p_a != p_b) {
        for (size_t i = 0; i < element_size; i++) {
            *p_a = *p_b;

            p_a++;
            p_b++;
        }
    }
    //printf("<%d, %d>\n", *(int*)without_type_p_a, *(int*)without_type_p_b);
}


void* SetPivot(void* p_value, size_t element_size) {
    void* pivot = malloc(element_size);
    if (pivot == NULL) {
        fprintf(stderr,"memory error");
        exit(1);
    }

    WriteIn(pivot, p_value, element_size);

    return pivot;
}


int ToLowComparator(void* without_type_p_a, void* without_type_p_b){
    assert(without_type_p_a != NULL);
    assert(without_type_p_b != NULL);

    int* p_a = (int*)without_type_p_a;
    int* p_b = (int*)without_type_p_b;
    //printf("<%d, %d>\n", *(int*)without_type_p_a, *(int*)without_type_p_b);

    if (*p_a > *p_b)
        return 1;
    else if (*p_b > *p_a)
        return -1;
    return 0;
}
