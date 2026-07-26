#ifndef UTILS_H
#define UTILS_H

/*
 * @brief store filename in dst from src with cap of maxlen
 *
 * @param dst
 * @param src
 * @param maxlen
 *
 * @return length of filename
 */
int getfilename(char *dst, const char *src, int maxlen);

#endif
