#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 10

int pt[N], in_hull[N], *upper, *lower;
double x[N], y[N];

void print_points(int *pt, int sz) {
    for (int i = 0; i < sz; ++i) {
        printf("%d: (%.3f, %.3f)\n", pt[i], x[pt[i]], y[pt[i]]);
    }
    printf("\n");
}

void print_hull(int sz) {
    printf("HULL:\n");
    for (int i = 0; i < sz; ++i) {
        if (in_hull[i])
            printf("%d: (%.3f, %.3f)\n", i, x[i], y[i]);
    }
    printf("\n");
}

#define cross(c, a, b) \
    ((x[a]-x[c])*(y[b]-y[c]) - (y[a]-y[c])*(x[b]-x[c]))

#define is_left(a, b, c) \
    (cross(c, a, b) > 0.0)

int *get_left(int* sz, int* pt, int a, int b) {
    int* left = malloc(2 * sizeof(int));
    left[0] = a;
    left[1] = b;
    int new_sz = 2;
    for (int i = 0; i < *sz; ++i) {
        if (pt[i] != a && pt[i] != b && is_left(a, b, pt[i])) {
            left = (int*)realloc(left, ++new_sz * sizeof(int));
            left[new_sz - 1] = pt[i];
        }
    }
    *sz = new_sz;
    return left;
}

void init(int *pt, int sz, int *minx, int *maxx) {
    *minx = pt[0];
    *maxx = pt[0];
    for (int i = 1; i < sz; ++i) {
        if (x[pt[i]] < x[*minx]) *minx = pt[i];
        if (x[pt[i]] > x[*maxx]) *maxx = pt[i];
    }
    in_hull[*minx] = 1;
    in_hull[*maxx] = 1;
}

int find_farthest(int* pt, int sz, int a, int b) {
    int farthest = pt[0];
    double max_distance = 0;
    
    for (int i = 0; i < sz; ++i) {
        if (pt[i] != a && pt[i] != b) {
            double distance = fabs(cross(pt[i], a, b));
            if (distance > max_distance) {
                max_distance = distance;
                farthest = pt[i];
            }
        }
    }
    
    return farthest;
}

void qhull(int *pt, int sz, int a, int b) {
    if (sz == 2) {
        in_hull[pt[0]] = 1;
        in_hull[pt[1]] = 1;
        return;
    }
    if (sz == 3) {
        in_hull[pt[2]] = 1;
        return;
    }
    
    int farthest = find_farthest(pt, sz, a, b);
    in_hull[farthest] = 1;
    
    int leftsz = sz;
    int* half = get_left(&leftsz, pt, a, farthest);
    qhull(half, leftsz, a, farthest);
    free(half);
    
    leftsz = sz;
    half = get_left(&leftsz, pt, farthest, b);
    qhull(half, leftsz, farthest, b);
    free(half);
}

int main(void) {
    for (int i = 0; i < N; ++i) {
        x[i] = rand() % 50;
        y[i] = rand() % 50;
        pt[i] = i;
    }
    print_points(pt, N);
    
    for (int i = 0; i < N; ++i) in_hull[i] = 0;
    
    int minpt, maxpt;
    init(pt, N, &minpt, &maxpt);
    
    int uppercnt = N, lowercnt = N;
    int* upper = get_left(&uppercnt, pt, minpt, maxpt);
    int* lower = get_left(&lowercnt, pt, maxpt, minpt);
    
    qhull(upper, uppercnt, minpt, maxpt);
    qhull(lower, lowercnt, maxpt, minpt);
    
    print_hull(N);
    
    free(upper);
    free(lower);
    
    return 0;
}
