#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 30
#define RADIUS 3

//#define _DEBUG

#define N 1000
double x[N], y[N];
int pt[N], in_hull[N], hull[N], hull_size = 0;
Vector2 hull_midpoint;

#define cross(c, a, b) \
    ((x[a]-x[c])*(y[b]-y[c]) - (y[a]-y[c])*(x[b]-x[c]))

#define is_left(a, b, c) \
    (cross(c, a, b) > 0.0)

// Получить массив всех объектов левее вектора по точкам a и b.
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

// "Нарисовать" первую прямую quickhull
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

// Найти самую удаленную от вектора ab точку (считаем по площади)
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

// Основной алгоритм. "Разделяй и властвуй", ищем удаленную, "рисуем" две линии, рассматриваем те, что вышли за эти линии.
void qhull(int *pt, int sz, int a, int b) {
    if (sz == 2) {
        in_hull[pt[0]] = 1;
        in_hull[pt[1]] = 1;
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

// Замена main из изначалаьной программы. "Нарисовать прямую", запустить quickhull для верхней и нижней половин.
void calculate_hull(int *pt, int sz) {
    hull_size = 0;
    
    for (int i = 0; i < sz; ++i) in_hull[i] = 0;
    
    int minpt, maxpt;
    init(pt, sz, &minpt, &maxpt);
    
    int uppercnt = sz, lowercnt = sz;
    int* upper = get_left(&uppercnt, pt, minpt, maxpt);
    int* lower = get_left(&lowercnt, pt, maxpt, minpt);
    
    qhull(upper, uppercnt, minpt, maxpt);
    qhull(lower, lowercnt, maxpt, minpt);
    
    free(upper);
    free(lower);
    
    // Собрать массив hull
    for (int i = 0; i < sz; ++i) {
        if (in_hull[i]) {
            hull[hull_size++] = i;
        }
    }
}

int cmp(const void* input1, const void* input2) {
    int p1 = *((int*)input1);
    int p2 = *((int*)input2);
    Vector2 vec1;
    vec1.x = x[p1] - hull_midpoint.x;
    vec1.y = y[p1] - hull_midpoint.y;
    
    Vector2 vec2;
    vec2.x = x[p2] - hull_midpoint.x;
    vec2.y = y[p2] - hull_midpoint.y;
    
    double th1 = atan2(vec1.y, vec1.x);
    double th2 = atan2(vec2.y, vec2.x);
    
    if (th1 < th2) return -1;
    if (th1 > th2) return 1;
    return 0;
}

void calc_midpoint() {
    double xsum = 0.0, ysum = 0.0;
    for (int i = 0; i < hull_size; ++i) {
        xsum += x[hull[i]];
        ysum += y[hull[i]];
    }
    xsum /= hull_size;
    ysum /= hull_size;
    hull_midpoint.x = xsum;
    hull_midpoint.y = ysum;
}

// Нарисовать оболочку. Сортируем точки в полярных координатах относительно центра (выпуклой оболочки)
void draw_hull() {
    calc_midpoint();
#ifdef _DEBUG
    char c[20];
    sprintf(c, "%.3f %.3f", hull_midpoint.x, hull_midpoint.y);
    
    TraceLog(LOG_INFO, c);
    
    for (int i = 0; i < hull_size; ++i) {
        printf("%d ", hull[i]);
    }
    
    printf("\n");
#endif    
    qsort(hull, hull_size, sizeof(int), cmp);
#ifdef _DEBUG    
    for (int i = 0; i < hull_size; ++i) {
        printf("%d ", hull[i]);
    }
#endif
    
    for (int i = 0; i < hull_size - 1; ++i) {
        DrawLine(x[hull[i]], y[hull[i]], x[hull[i + 1]], y[hull[i + 1]], BLACK);
    }
    DrawLine(x[hull[hull_size - 1]], y[hull[hull_size - 1]], x[hull[0]], y[hull[0]], BLACK);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //-------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "QuickHull implementation in raylib");
    SetTargetFPS(FPS);
    
    //--------------------------------------------------------------------------------------
    
    int sz = 0;

    // Main app loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {    
        // Update
        //----------------------------------------------------------------------------------
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 point = GetMousePosition();
            if (sz < N) {
                pt[sz] = sz;
                x[sz] = point.x;
                y[sz] = point.y;
                sz++;
            }
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && sz > 2) {
            calculate_hull(pt, sz);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            sz = 0;
            hull_size = 0;
        }
     
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);

            for (int i = 0; i < sz; ++i) {
                DrawCircle(x[i], y[i], RADIUS, RED);
            }
            if (hull_size > 0) {
                draw_hull();
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
