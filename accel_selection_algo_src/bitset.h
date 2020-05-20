/*
 * Copyright (c) 2010 Emanuele Giaquinta.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define WORD unsigned long
#define WORD_BITS (sizeof(WORD)*CHAR_BIT)
#define bit_size(bits) (((bits)+WORD_BITS-1)/WORD_BITS)
#define	bit_byte(bit) ((bit) / WORD_BITS)
#define	bit_mask(bit) (1L << ((bit) % WORD_BITS))
#define bit_alloc(bits) (WORD *)calloc(bit_size(bits), sizeof(WORD))
#define	bit_set(name, bit) ((name)[bit_byte(bit)] |= bit_mask(bit))
#define	bit_clear(name, bit) ((name)[bit_byte(bit)] &= ~bit_mask(bit))
#define	bit_test(name, bit) ((name)[bit_byte(bit)] & bit_mask(bit))
#define bit_zero(name, bits) memset(name, 0, bit_size(bits) * sizeof(WORD))

#define GCC_VERSION(major,minor) (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))

#if GCC_VERSION(3,4)
#define bit_ctz(x) (__builtin_ctzl(x))
#define bit_popcount(x) (__builtin_popcountl(x))
#else
static inline int
bit_ctz(long x)
{
	int r = 0;

	x &= ~x + 1;

	if (sizeof(long) == 4) {
		if (x & 0xaaaaaaaaL) r +=  1;
		if (x & 0xccccccccL) r +=  2;
		if (x & 0xf0f0f0f0L) r +=  4;
		if (x & 0xff00ff00L) r +=  8;
		if (x & 0xffff0000L) r += 16;
	} else {
		if ( x & 0xaaaaaaaaaaaaaaaaL )	r += 1;
		if ( x & 0xccccccccccccccccL )	r += 2;
		if ( x & 0xf0f0f0f0f0f0f0f0L )	r += 4;
		if ( x & 0xff00ff00ff00ff00L )	r += 8;
		if ( x & 0xffff0000ffff0000L )	r += 16;
		if ( x & 0xffffffff00000000L )	r += 32;
	}

	return r;
}

static inline int
bit_popcount(long x)
{
	if (sizeof(long) == 4) {
		x -=  (x >> 1) & 0x55555555L;
		x  = ((x >> 2) & 0x33333333L) + (x & 0x33333333L);
		x  = ((x >> 4) + x) & 0x0f0f0f0fL;
		x *= 0x01010101L;
		return	x >> 24;
	} else {
		x -=  (x >> 1) & 0x5555555555555555L;
		x  = ((x >> 2) & 0x3333333333333333L) + (x & 0x3333333333333333L);
		x  = ((x >> 4) + x) & 0x0f0f0f0f0f0f0f0fL;
		x *= 0x0101010101010101L;
		return	x >> 56;
	}
}
#endif

static inline void
bit_alloc_n(WORD **name, int n, int bits)
{
	int i;

	name[0] = (WORD *)calloc(n * bit_size(bits), sizeof(WORD));
	for (i = 1; i < n; i++)
		name[i] = name[0] + i*bit_size(bits);
}

static inline void
bit_union(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		name1[i] |= name2[i];
}

static inline void
bit_intersection(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		name1[i] &= name2[i];
}

static inline void
bit_difference(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		name1[i] &= ~name2[i];
}

static inline void
bit_copy(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		name1[i] = name2[i];
}

static inline int
bit_equal(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		if (name1[i] != name2[i])
			return 0;
	return 1;
}

static inline int
bit_subset(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		if ((name1[i] & name2[i]) != name1[i])
			return 0;
	return 1;
}

static inline int
bit_disjoint(WORD *name1, WORD *name2, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		if (name1[i] & name2[i])
			return 0;
	return 1;
}

static inline int
bit_minimum(WORD *name, int bits)
{
	int i;

	for (i = 0; i < bit_size(bits); i++)
		if (name[i])
			return bit_ctz(name[i]) + i * WORD_BITS;
	return -1;
}

static inline int
bit_len(WORD *name, int bits)
{
	int i;
	int len = 0;

	for (i = 0; i < bit_size(bits); i++)
		if (name[i])
			len += bit_popcount(name[i]);
	return len;
}

struct bit_iter {
	WORD *v;
	WORD x;
	unsigned i;
	unsigned n;
};

static inline void
bit_iter_init(struct bit_iter *iter, WORD *A, int bits)
{
	iter->i = 0;
	iter->n = bit_size(bits);
	iter->v = A;
	iter->x = A[0];
}

static inline int
bit_iter_next(struct bit_iter *iter)
{
	int idx;

	while (!iter->x) {
		iter->i++;
		if (iter->i == iter->n)
			return -1;
		iter->x = iter->v[iter->i];
	}
	idx = bit_ctz(iter->x);
	iter->x ^= 1L << idx;
	return idx+iter->i*WORD_BITS;
}

// Local variables:
// c-basic-offset: 8
// indent-tabs-mode: t
// End:
