bool MYqsort(void* beg, void* end, size_t element_size,
             void* Partition(void*, void*, size_t, int (*)(void*, void*)),
             int Compare(const void*, const void*));

void* LomutoPartition(void* beg, void* end, size_t element_size,
                      int Compare(const void*, const void*));

void* HoarePartition(void* beg, void* end, size_t element_size,
                     int Compare(const void*, const void*));

void* UpgradedHoarePartition(void* beg, void* end, size_t element_size,
                             int Compare(const void*, const void*));

void Swap(void* p_a, void* p_b, size_t element_size); // TODO typedef for char as 1-byte data type

void WriteIn(void* p_into, const void* p_from, size_t element_size);

void* SetPivot(void* p_value, size_t element_size);


bool MYqsort(void* beg, void* end, size_t element_size,
             void* Partition(void*, void*, size_t, int (*)(const void*, const void*)),
             int Compare(const void*, const void*)) {
    assert(beg != NULL);
    assert(end != NULL);
    assert(Partition != NULL);

    if (beg >= end)
        return 1;

    char* pivot = (char*)Partition(beg, end, element_size, Compare);

    MYqsort(beg, (void*)(pivot - element_size), element_size, Partition, Compare);
    MYqsort((void*)(pivot + element_size), end, element_size, Partition, Compare);
}


void* LomutoPartition(void* beg, void* end, size_t element_size,
                      int Compare(const void*, const void*)) {
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


void* HoarePartition(void* beg, void* end, size_t element_size,
                     int Compare(const void*, const void*)) {
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


void* UpgradedHoarePartition(void* beg, void* end, size_t element_size,
                             int Compare(const void*, const void*)) {
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
}


void Swap(void* without_type_p_a, void* without_type_p_b, size_t element_size) {
    assert(without_type_p_a != NULL);
    assert(without_type_p_b != NULL);

    char* p_a = (char*)without_type_p_a;
    char* p_b = (char*)without_type_p_b;
    char mem = 0;
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


void WriteIn(void* without_type_p_into, const void* without_type_p_from, size_t element_size) {
    assert(without_type_p_into != NULL);
    assert(without_type_p_from != NULL);

    char* p_into = (char*)without_type_p_into;
    const char* p_from = (const char*)without_type_p_from;
    //printf("<%d, %d> - ", *(int*)without_type_p_a, *(int*)without_type_p_b);

    if (p_into != p_from) {
        for (size_t i = 0; i < element_size; i++) {
            *p_into = *p_from;

            p_into++;
            p_from++;
        }
    }
    //printf("<%d, %d>\n", *(int*)without_type_p_into, *(int*)without_type_p_from);
}


void* SetPivot(void* p_value, size_t element_size) {
    assert(p_value);

    void* pivot = malloc(element_size);
    if (pivot == NULL) {
        fprintf(stderr,"memory error");
        exit(1);
    }

    WriteIn(pivot, p_value, element_size);

    return pivot;
}
