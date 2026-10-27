#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_COUNT 10000
#define TRIALS 100
#define MAX_KEY 1000000

typedef struct {
    int key1;
    int key2;
    int orig_idx;
} Data;

typedef struct {
    long long comparisons;
    long long movements;
} Metrics;

int rand_range(int max_val) {
    long long r = ((long long)rand() << 15) | rand();
    return (int)(r % (max_val + 1));
}

void generate_random_data(Data arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i].key1 = rand_range(MAX_KEY);
        arr[i].key2 = rand_range(MAX_KEY);
        arr[i].orig_idx = i;
    }
}

void copy_data(Data dest[], const Data src[], int n) {
    memcpy(dest, src, sizeof(Data) * n);
}

int is_sorted(const Data arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i].key1 > arr[i + 1].key1) return 0;
    }
    return 1;
}

int is_stable(const Data arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i].key1 == arr[i + 1].key1 && arr[i].orig_idx > arr[i + 1].orig_idx) {
            return 0;
        }
    }
    return 1;
}

Metrics selection_sort(Data arr[], int n) {
    Metrics m = { 0, 0 };
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            m.comparisons++;
            if (arr[j].key1 < arr[min_idx].key1) {
                min_idx = j;
            }
        }
        Data temp = arr[i];
        arr[i] = arr[min_idx];
        arr[min_idx] = temp;
        m.movements += 3;
    }
    return m;
}

Metrics bubble_sort(Data arr[], int n) {
    Metrics m = { 0, 0 };
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            m.comparisons++;
            if (arr[j].key1 > arr[j + 1].key1) {
                Data temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                m.movements += 3;
            }
        }
    }
    return m;
}

Metrics bubble_sort_early(Data arr[], int n) {
    Metrics m = { 0, 0 };
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            m.comparisons++;
            if (arr[j].key1 > arr[j + 1].key1) {
                Data temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                m.movements += 3;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
    return m;
}

Metrics insertion_sort_gt(Data arr[], int n) {
    Metrics m = { 0, 0 };
    for (int i = 1; i < n; i++) {
        Data current = arr[i];
        m.movements++;
        int j = i - 1;
        while (j >= 0) {
            m.comparisons++;
            if (arr[j].key1 > current.key1) {
                arr[j + 1] = arr[j];
                m.movements++;
                j--;
            }
            else {
                break;
            }
        }
        arr[j + 1] = current;
        m.movements++;
    }
    return m;
}

Metrics insertion_sort_ge(Data arr[], int n) {
    Metrics m = { 0, 0 };
    for (int i = 1; i < n; i++) {
        Data current = arr[i];
        m.movements++;
        int j = i - 1;
        while (j >= 0) {
            m.comparisons++;
            if (arr[j].key1 >= current.key1) {
                arr[j + 1] = arr[j];
                m.movements++;
                j--;
            }
            else {
                break;
            }
        }
        arr[j + 1] = current;
        m.movements++;
    }
    return m;
}

void prepare_state_datasets(const Data base[], Data sorted[], Data reversed[], Data partial[], int n) {
    copy_data(sorted, base, n);
    insertion_sort_gt(sorted, n);
    for (int i = 0; i < n; i++) sorted[i].orig_idx = i;

    for (int i = 0; i < n; i++) {
        reversed[i] = sorted[n - 1 - i];
        reversed[i].orig_idx = i;
    }

    copy_data(partial, sorted, n);
    int swap_count = n / 20; // 5% 무작위 교환 (500회)
    for (int i = 0; i < swap_count; i++) {
        int idx1 = rand_range(n - 1);
        int idx2 = rand_range(n - 1);
        Data temp = partial[idx1];
        partial[idx1] = partial[idx2];
        partial[idx2] = temp;
    }
    for (int i = 0; i < n; i++) partial[i].orig_idx = i;
}

void run_single_state_test(const char* title, const Data src[], Data work[], int n) {
    Metrics m_sel, m_bub, m_bube, m_ins;
    int ok = 1;

    copy_data(work, src, n);
    m_sel = selection_sort(work, n);
    ok &= is_sorted(work, n);

    copy_data(work, src, n);
    m_bub = bubble_sort(work, n);
    ok &= is_sorted(work, n);

    copy_data(work, src, n);
    m_bube = bubble_sort_early(work, n);
    ok &= is_sorted(work, n);

    copy_data(work, src, n);
    m_ins = insertion_sort_gt(work, n);
    ok &= is_sorted(work, n);

    printf("%s (Sort Verified: %s)\n\n", title, ok ? "PASS" : "FAIL");
    printf("Selection Sort\nComparisons : %lld\nMovements   : %lld\n\n", m_sel.comparisons, m_sel.movements);
    printf("Bubble Sort\nComparisons : %lld\nMovements   : %lld\n\n", m_bub.comparisons, m_bub.movements);
    printf("Bubble Sort (Early Termination)\nComparisons : %lld\nMovements   : %lld\n\n", m_bube.comparisons, m_bube.movements);
    printf("Insertion Sort\nComparisons : %lld\nMovements   : %lld\n\n", m_ins.comparisons, m_ins.movements);
    printf("------------------------------------\n\n");
}

int main(void) {
    srand((unsigned int)time(NULL));

    static Data orig[DATA_COUNT];
    static Data work[DATA_COUNT];
    static Data sorted[DATA_COUNT];
    static Data reversed[DATA_COUNT];
    static Data partial[DATA_COUNT];

    long long total_comp_sel = 0, total_move_sel = 0;
    long long total_comp_bub = 0, total_move_bub = 0;
    long long total_comp_bube = 0, total_move_bube = 0;
    long long total_comp_ins = 0, total_move_ins = 0;
    int all_random_sorted = 1;

    for (int t = 0; t < TRIALS; t++) {
        generate_random_data(orig, DATA_COUNT);

        copy_data(work, orig, DATA_COUNT);
        Metrics m = selection_sort(work, DATA_COUNT);
        total_comp_sel += m.comparisons;
        total_move_sel += m.movements;
        all_random_sorted &= is_sorted(work, DATA_COUNT);

        copy_data(work, orig, DATA_COUNT);
        m = bubble_sort(work, DATA_COUNT);
        total_comp_bub += m.comparisons;
        total_move_bub += m.movements;
        all_random_sorted &= is_sorted(work, DATA_COUNT);

        copy_data(work, orig, DATA_COUNT);
        m = bubble_sort_early(work, DATA_COUNT);
        total_comp_bube += m.comparisons;
        total_move_bube += m.movements;
        all_random_sorted &= is_sorted(work, DATA_COUNT);

        copy_data(work, orig, DATA_COUNT);
        m = insertion_sort_gt(work, DATA_COUNT);
        total_comp_ins += m.comparisons;
        total_move_ins += m.movements;
        all_random_sorted &= is_sorted(work, DATA_COUNT);
    }

    printf("Data Count : %d\n\n", DATA_COUNT);
    printf("Random Data (Sort Verified: %s)\n", all_random_sorted ? "PASS" : "FAIL");
    printf("Number of Trials : %d\n\n", TRIALS);

    printf("Selection Sort\n");
    printf("Total Comparisons   : %lld\n", total_comp_sel);
    printf("Average Comparisons : %lld\n", total_comp_sel / TRIALS);
    printf("Average Movements   : %lld\n\n", total_move_sel / TRIALS);

    printf("Bubble Sort\n");
    printf("Total Comparisons   : %lld\n", total_comp_bub);
    printf("Average Comparisons : %lld\n", total_comp_bub / TRIALS);
    printf("Average Movements   : %lld\n\n", total_move_bub / TRIALS);

    printf("Bubble Sort (Early Termination)\n");
    printf("Total Comparisons   : %lld\n", total_comp_bube);
    printf("Average Comparisons : %lld\n", total_comp_bube / TRIALS);
    printf("Average Movements   : %lld\n\n", total_move_bube / TRIALS);

    printf("Insertion Sort\n");
    printf("Total Comparisons   : %lld\n", total_comp_ins);
    printf("Average Comparisons : %lld\n", total_comp_ins / TRIALS);
    printf("Average Movements   : %lld\n\n", total_move_ins / TRIALS);
    printf("------------------------------------\n\n");

    prepare_state_datasets(orig, sorted, reversed, partial, DATA_COUNT);
    run_single_state_test("Sorted Data", sorted, work, DATA_COUNT);
    run_single_state_test("Reverse-Sorted Data", reversed, work, DATA_COUNT);
    run_single_state_test("Partially Sorted Data", partial, work, DATA_COUNT);

    // 중복 키가 보장된 시험 데이터(10,000개, key1 범위 0~999)로 안정성 검증
    static Data stab_orig[DATA_COUNT];
    for (int i = 0; i < DATA_COUNT; i++) {
        stab_orig[i].key1 = rand_range(999);
        stab_orig[i].key2 = (i + 1) * 10;
        stab_orig[i].orig_idx = i;
    }

    copy_data(work, stab_orig, DATA_COUNT);
    selection_sort(work, DATA_COUNT);
    int st_sel = is_stable(work, DATA_COUNT);

    copy_data(work, stab_orig, DATA_COUNT);
    bubble_sort(work, DATA_COUNT);
    int st_bub = is_stable(work, DATA_COUNT);

    copy_data(work, stab_orig, DATA_COUNT);
    insertion_sort_gt(work, DATA_COUNT);
    int st_ins_gt = is_stable(work, DATA_COUNT);

    copy_data(work, stab_orig, DATA_COUNT);
    insertion_sort_ge(work, DATA_COUNT);
    int st_ins_ge = is_stable(work, DATA_COUNT);

    printf("Stability Verification\n\n");
    printf("Selection Sort       : %s\n", st_sel ? "Stable" : "Unstable");
    printf("Bubble Sort          : %s\n", st_bub ? "Stable" : "Unstable");
    printf("Insertion Sort (>)   : %s\n", st_ins_gt ? "Stable" : "Unstable");
    printf("Insertion Sort (>=)  : %s\n", st_ins_ge ? "Stable" : "Unstable");

    return 0;
}