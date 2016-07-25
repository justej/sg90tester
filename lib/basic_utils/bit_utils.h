#ifndef BIT_UTILS_H_
#define BIT_UTILS_H_

#define resetBit(p, b)          do {(p) &= ~(b);} while (0)
#define setBit(p, b)            do {(p) |= (b);} while (0)
#define testBit(p, b)           ((p) & ~(b))

#endif // BIT_UTILS_H_