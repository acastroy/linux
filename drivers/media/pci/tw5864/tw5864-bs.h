/*
 *  TW5864 driver - Exponential-Golomb coding functions
 *
 *  These functions are used for generation of H.264 bitstream headers.
 *
 *  This file is derived from tw5864 reference driver by manufacturers, which
 *  itself apparently was derived from x264 project.
 *
 *  Copyright (C) 2015 Bluecherry, LLC <maintainers@bluecherrydvr.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

/* Bitstream writing context */
struct bs {
	u8 *buf; /* pointer to buffer beginning */
	u8 *buf_end; /* pointer to buffer end */
	u8 *ptr; /* pointer to current byte in buffer */
	unsigned int bits_left; /* number of available bits in current byte */
};

static inline void bs_init(struct bs *s, void *buf, int size)
{
	s->buf = buf;
	s->ptr = buf;
	s->buf_end = s->ptr + size;
	s->bits_left = 8;
}

static inline int bs_len(struct bs *s)
{
	return s->ptr - s->buf;
}

static inline void bs_write(struct bs *s, int count, u32 bits)
{
	if (s->ptr >= s->buf_end - 4)
		return;
	while (count > 0) {
		if (count < 32)
			bits &= (1 << count) - 1;
		if (count < s->bits_left) {
			*s->ptr = (*s->ptr << count) | bits;
			s->bits_left -= count;
			break;
		}
		*s->ptr = (*s->ptr << s->bits_left) |
			(bits >> (count - s->bits_left));
		count -= s->bits_left;
		s->ptr++;
		s->bits_left = 8;
	}
}

static inline void bs_write1(struct bs *s, u32 bit)
{
	if (s->ptr < s->buf_end) {
		*s->ptr <<= 1;
		*s->ptr |= bit;
		s->bits_left--;
		if (s->bits_left == 0) {
			s->ptr++;
			s->bits_left = 8;
		}
	}
}

static inline void bs_write_ue(struct bs *s, u32 val)
{
	if (val == 0) {
		bs_write1(s, 1);
	} else {
		val++;
		bs_write(s, 2 * fls(val) - 1, val);
	}
}

static inline void bs_write_se(struct bs *s, int val)
{
	bs_write_ue(s, val <= 0 ? -val * 2 : val * 2 - 1);
}

static inline void bs_write_te(struct bs *s, int x, int val)
{
	if (x == 1)
		bs_write1(s, 1 & ~val);
	else if (x > 1)
		bs_write_ue(s, val);
}

static inline void bs_rbsp_trailing(struct bs *s)
{
	bs_write1(s, 1);
	if (s->bits_left != 8)
		bs_write(s, s->bits_left, 0x00);
}
