#define ARROW_SIZE 50
#define MAX_NUMBER 100
//TODO const

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

bool MYqsort(void* beg, void* end, size_t element_size,
             void* Partition(void*, void*, size_t, int (*)(void*, void*)), int Compare(void*, void*));

void* LomutoPartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void* HoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void* UpgradedHoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*));

void Swap(void* p_a, void* p_b, size_t element_size);

void WriteIn(void* p_a, void* p_b, size_t element_size);


/*int main() { //TODO more functions
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

    for (int i = 0; i < ARROW_SIZE; i++) {
        x = rand() % MAX_NUMBER;
        arr[i] = x;
        brr[i] = x;
    }

    MYqsort(arr, arr + ARROW_SIZE - 1, sizeof(int), LomutoPartition, ToLowComparator);
    MYqsort(brr, brr + ARROW_SIZE - 1, sizeof(int), UpgradedHoarePartition, ToLowComparator);
    //LomutoPartition(arr, arr + ARROW_SIZE - 1, sizeof(int), ToLowComparator);

    for (int q = 0; q < ARROW_SIZE; q++)
        printf("%d ", arr[q]);
    printf("\n");
    for (int q = 0; q < ARROW_SIZE; q++)
        printf("%d ", brr[q]);
    printf("\n");

    free(arr);
    free(brr);
}*/


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


void* LomutoPartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* pos = (char*)beg;
    void* pivot = malloc(element_size);
    if (pivot == NULL) {
        fprintf(stderr,"memory error");
        exit(1);
    }
    WriteIn(pivot, beg, element_size);

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
    void* pivot = malloc(element_size);
    if (pivot == NULL) {
        fprintf(stderr,"memory error");
        exit(1);
    }
    WriteIn(pivot, beg, element_size);

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

    return (void*)right;
}


void* UpgradedHoarePartition(void* beg, void* end, size_t element_size, int Compare(void*, void*)) {
    assert(beg != NULL);
    assert(end != NULL);

    char* left = (char*)beg;
    char* right = (char*)end;
    void* pivot = malloc(element_size);  //TODO func SetPivot;
    if (pivot == NULL) {
        fprintf(stderr,"memory error");
        exit(1);
    }
    WriteIn(pivot, beg, element_size);

    while (1) {
        if (Compare(right, pivot) > 0) {
            right -= element_size;
            continue;
        }
        if (left >= right) {
            WriteIn((void*)left, pivot, element_size);
            return (void*)left;
        }
        WriteIn((void*)left, (void*)right, element_size);
        left += element_size;
        while (1) {
            if (left >= right) {
                WriteIn((void*)left, pivot, element_size);
                return (void*)left;
            }
            if (Compare(left, pivot) <= 0) {
                left += element_size;
                continue;
            }
            WriteIn((void*)right, (void*)left, element_size);
            right -= element_size;
            break;
        }
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
