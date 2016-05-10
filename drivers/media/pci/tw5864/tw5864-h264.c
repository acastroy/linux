/*
 *  TW5864 driver - H.264 headers generation functions
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

#include "tw5864.h"
#include "tw5864-bs.h"

static u8 marker[] = { 0x00, 0x00, 0x00, 0x01 };

/* log2 of max GOP size, taken 8 as V4L2-advertised max GOP size is 255 */
#define LOG2_MAX_FRAME_NUM 8
#define LOG2_MAX_POC_LSB LOG2_MAX_FRAME_NUM

static int tw5864_h264_gen_sps_rbsp(u8 *buf, size_t size, int width, int height)
{
	struct bs bs, *s;
	const int mb_width = width / 16;
	const int mb_height = height / 16;

	s = &bs;
	bs_init(s, buf, size);
	bs_write(s, 8, 0x42 /* profile == 66, baseline */);
	bs_write(s, 1, 1); /* constraint_set0_flag */
	bs_write(s, 1, 1); /* constraint_set1_flag */
	bs_write(s, 6, 0);
	bs_write(s, 8, 0x1E /* level */);
	bs_write_ue(s, 0 /* SPS id */);
	bs_write_ue(s, LOG2_MAX_FRAME_NUM - 4);
	bs_write_ue(s, 0 /* POC type */);
	bs_write_ue(s, LOG2_MAX_POC_LSB - 4);

	bs_write_ue(s, 1 /* num_ref_frames */);
	bs_write(s, 1, 0 /* gaps_in_frame_num_value_allowed_flag */);
	bs_write_ue(s, mb_width - 1);
	bs_write_ue(s, mb_height - 1);
	bs_write(s, 1, 1 /* frame_mbs_only_flag */);
	bs_write(s, 1, 0 /* direct_8x8_inference_flag */);
	bs_write(s, 1, 0);
	bs_write(s, 1, 0);
	bs_rbsp_trailing(s);
	return bs_len(s);
}

static int tw5864_h264_gen_pps_rbsp(u8 *buf, size_t size, int qp)
{
	struct bs bs, *s;

	s = &bs;
	bs_init(s, buf, size);
	bs_write_ue(s, 0 /* pic_parameter_set_id */);
	bs_write_ue(s, 0 /* seq_parameter_set_id */);
	bs_write(s, 1, 0 /* entropy_coding_mode_flag */);
	bs_write(s, 1, 0 /* pic_order_present_flag */);
	bs_write_ue(s, 0 /* num_slice_groups_minus1 */);
	bs_write_ue(s, 0 /* i_num_ref_idx_l0_active_minus1 */);
	bs_write_ue(s, 0 /* i_num_ref_idx_l1_active_minus1 */);
	bs_write(s, 1, 0 /* weighted_pred_flag */);
	bs_write(s, 2, 0 /* weighted_bipred_idc */);
	bs_write_se(s, qp - 26 /* pic_init_qp_minus26 */);
	bs_write_se(s, qp - 26 /* pic_init_qs_minus26 */);
	bs_write_se(s, 0 /* chroma_qp_index_offset */);
	bs_write(s, 1, 0 /* deblocking_filter_control_present_flag */);
	bs_write(s, 1, 0 /* constrained_intra_pred_flag */);
	bs_write(s, 1, 0 /* redundant_pic_cnt_present_flag */);
	bs_rbsp_trailing(s);
	return bs_len(s);
}

static int tw5864_h264_gen_slice_head(u8 *buf, size_t size,
				      unsigned int idr_pic_id,
				      unsigned int frame_seqno_in_gop,
				      int *tail_nb_bits, u8 *tail)
{
	struct bs bs, *s;
	int is_i_frame = frame_seqno_in_gop == 0;
	int i_poc_lsb = frame_seqno_in_gop;

	s = &bs;
	bs_init(s, buf, size);
	bs_write_ue(s, 0 /* i_first_mb */);
	bs_write_ue(s, is_i_frame ? 2 : 5 /* slice type - I or P */);
	bs_write_ue(s, 0 /* PPS id */);
	bs_write(s, LOG2_MAX_FRAME_NUM, frame_seqno_in_gop);
	if (is_i_frame)
		bs_write_ue(s, idr_pic_id);

	bs_write(s, LOG2_MAX_POC_LSB, i_poc_lsb);

	if (!is_i_frame)
		bs_write1(s, 0 /*b_num_ref_idx_override */);

	/* ref pic list reordering */
	if (!is_i_frame)
		bs_write1(s, 0 /* b_ref_pic_list_reordering_l0 */);

	if (is_i_frame) {
		bs_write1(s, 0); /* no output of prior pics flag */
		bs_write1(s, 0); /* long term reference flag */
	} else {
		bs_write1(s, 0); /* adaptive_ref_pic_marking_mode_flag */
	}

	bs_write_se(s, 0 /* i_qp_delta */);

	if (s->bits_left != 8) {
		*tail = ((s->ptr[0]) << s->bits_left);
		*tail_nb_bits = 8 - s->bits_left;
	} else {
		*tail = 0;
		*tail_nb_bits = 0;
	}

	return bs_len(s);
}

void tw5864_h264_put_stream_header(u8 **buf, size_t *space_left, int qp,
				   int width, int height)
{
	int nal_len;

	/* SPS */
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	**buf = 0x67; /* SPS NAL header */
	*buf += 1;
	*space_left -= 1;

	nal_len = tw5864_h264_gen_sps_rbsp(*buf, *space_left, width, height);
	*buf += nal_len;
	*space_left -= nal_len;

	/* PPS */
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	**buf = 0x68; /* PPS NAL header */
	*buf += 1;
	*space_left -= 1;

	nal_len = tw5864_h264_gen_pps_rbsp(*buf, *space_left, qp);
	*buf += nal_len;
	*space_left -= nal_len;
}

void tw5864_h264_put_slice_header(u8 **buf, size_t *space_left,
				  unsigned int idr_pic_id,
				  unsigned int frame_seqno_in_gop,
				  int *tail_nb_bits, u8 *tail)
{
	int nal_len;

	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	/* Frame NAL header */
	**buf = (frame_seqno_in_gop == 0) ? 0x25 : 0x21;
	*buf += 1;
	*space_left -= 1;

	nal_len = tw5864_h264_gen_slice_head(*buf, *space_left, idr_pic_id,
					     frame_seqno_in_gop, tail_nb_bits,
					     tail);
	*buf += nal_len;
	*space_left -= nal_len;
}
