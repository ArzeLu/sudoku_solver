#ifndef CONSTANTS_H
#define CONSTANTS_H

#define n 3                // The dimensions of the little 3 by 3 boxes
#define N 9
#define NUM_CELLS 81
#define INITIAL_MASK 0x3FE // Flip the bits in the right hand side from 1 to 9. 0000001111111110

/// Convenience for finding the correct row given a cell index.
/// e.g. index is 31, then it's on the 3rd row.
#define ROW_POSITION { \
    0, 0, 0, 0, 0, 0, 0, 0, 0, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, \
    2, 2, 2, 2, 2, 2, 2, 2, 2, \
    3, 3, 3, 3, 3, 3, 3, 3, 3, \
    4, 4, 4, 4, 4, 4, 4, 4, 4, \
    5, 5, 5, 5, 5, 5, 5, 5, 5, \
    6, 6, 6, 6, 6, 6, 6, 6, 6, \
    7, 7, 7, 7, 7, 7, 7, 7, 7, \
    8, 8, 8, 8, 8, 8, 8, 8, 8  \
}

/// Convenience for finding the correct column given a cell index.
/// e.g. index is 31, then it's on the 4th column.
#define COL_POSITION { \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8, \
    0, 1, 2, 3, 4, 5, 6, 7, 8  \
}

/// Convenience for finding the correct box given a cell index.
/// e.g. index is 31, then it's in the 4th box.
#define BOX_POSITION { \
    0, 0, 0, 1, 1, 1, 2, 2, 2, \
    0, 0, 0, 1, 1, 1, 2, 2, 2, \
    0, 0, 0, 1, 1, 1, 2, 2, 2, \
    3, 3, 3, 4, 4, 4, 5, 5, 5, \
    3, 3, 3, 4, 4, 4, 5, 5, 5, \
    3, 3, 3, 4, 4, 4, 5, 5, 5, \
    6, 6, 6, 7, 7, 7, 8, 8, 8, \
    6, 6, 6, 7, 7, 7, 8, 8, 8, \
    6, 6, 6, 7, 7, 7, 8, 8, 8  \
}

/// The sequential order of indices along the rows.
#define ROW_TRAVERSAL { \
    {0,  1,  2,  3,  4,  5,  6,  7,  8 }, \
    {9, 10, 11, 12, 13, 14, 15, 16, 17 }, \
    {18, 19, 20, 21, 22, 23, 24, 25, 26}, \
    {27, 28, 29, 30, 31, 32, 33, 34, 35}, \
    {36, 37, 38, 39, 40, 41, 42, 43, 44}, \
    {45, 46, 47, 48, 49, 50, 51, 52, 53}, \
    {54, 55, 56, 57, 58, 59, 60, 61, 62}, \
    {63, 64, 65, 66, 67, 68, 69, 70, 71}, \
    {72, 73, 74, 75, 76, 77, 78, 79, 80}  \
}

/// The sequential order of indices along the columns.
#define COL_TRAVERSAL { \
    {0,  9, 18, 27, 36, 45, 54, 63, 72 }, \
    {1, 10, 19, 28, 37, 46, 55, 64, 73 }, \
    {2, 11, 20, 29, 38, 47, 56, 65, 74 }, \
    {3, 12, 21, 30, 39, 48, 57, 66, 75 }, \
    {4, 13, 22, 31, 40, 49, 58, 67, 76 }, \
    {5, 14, 23, 32, 41, 50, 59, 68, 77 }, \
    {6, 15, 24, 33, 42, 51, 60, 69, 78 }, \
    {7, 16, 25, 34, 43, 52, 61, 70, 79 }, \
    {8, 17, 26, 35, 44, 53, 62, 71, 80 }  \
}

/// The sequential order of indices in the boxes.
/// The indices go from left to right,
/// then top to bottom.
#define BOX_TRAVERSAL { \
    {0,  1,  2,  9,  10, 11, 18, 19, 20}, \
    {3,  4,  5,  12, 13, 14, 21, 22, 23}, \
    {6,  7,  8,  15, 16, 17, 24, 25, 26}, \
    {27, 28, 29, 36, 37, 38, 45, 46, 47}, \
    {30, 31, 32, 39, 40, 41, 48, 49, 50}, \
    {33, 34, 35, 42, 43, 44, 51, 52, 53}, \
    {54, 55, 56, 63, 64, 65, 72, 73, 74}, \
    {57, 58, 59, 66, 67, 68, 75, 76, 77}, \
    {60, 61, 62, 69, 70, 71, 78, 79, 80}  \
}

#endif