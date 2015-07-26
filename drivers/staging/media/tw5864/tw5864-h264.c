#include "tw5864.h"

#include "h264bitstream/h264_stream.h"

uint8_t marker[] = {0x00, 0x00, 0x00, 0x01};

h264_stream_t *tw5864_h264_init(void)
{
	return h264_new();
}

void tw5864_h264_destroy(h264_stream_t *h)
{
	h264_free(h);
}

void tw5864_h264_put_stream_header(h264_stream_t* h, u8 **buf, size_t *space_left, int qp, int width, int height)
{
	u8 nal_buf[64] = {0, };  /* Because of h264bitstream freaking behaviour, it adds zero byte in front of generated NAL header FIXME */
	int nal_len;

	/* SPS */
	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	h->nal->nal_ref_idc = 0x03;
	h->nal->nal_unit_type = NAL_UNIT_TYPE_SPS;

	h->sps->profile_idc = 0x42;  /* == 66, baseline */
	h->sps->level_idc = 0x1E;  // 30;
	h->sps->log2_max_frame_num_minus4 = 3;//0x0b; //3;  // TODO Comment what this is
	h->sps->log2_max_pic_order_cnt_lsb_minus4 = 3;//0x0b; //3; // TODO Comment what this is
	h->sps->num_ref_frames = 0x01;
	h->sps->pic_width_in_mbs_minus1 = (width / 16) - 1;
	h->sps->pic_height_in_map_units_minus1 = (height / 16) - 1;
	h->sps->frame_mbs_only_flag = 0x01;

	nal_len = write_nal_unit(h, nal_buf, sizeof(nal_buf));
	WARN_ON_ONCE(nal_len >= sizeof(nal_buf));
	memcpy(*buf, nal_buf + 1, nal_len - 1);
	*buf += nal_len - 1;
	*space_left -= nal_len - 1;


	/* PPS */
	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	h->nal->nal_ref_idc = 0x03;
	h->nal->nal_unit_type = NAL_UNIT_TYPE_PPS;

	h->pps->pic_parameter_set_id = 0;
	h->pps->seq_parameter_set_id = 0;
	h->pps->entropy_coding_mode_flag = 0;
	h->pps->pic_order_present_flag = 0;
	h->pps->num_slice_groups_minus1 = 0;
	h->pps->num_ref_idx_l0_active_minus1 = 0;
	h->pps->num_ref_idx_l1_active_minus1 = 0;
	h->pps->weighted_pred_flag = 0;
	h->pps->weighted_bipred_idc = 0;
	h->pps->pic_init_qp_minus26 = qp - 26;
	h->pps->pic_init_qs_minus26 = qp - 26;
	h->pps->chroma_qp_index_offset = 0;
	h->pps->deblocking_filter_control_present_flag = 0;
	h->pps->constrained_intra_pred_flag = 0;
	h->pps->redundant_pic_cnt_present_flag = 0;

	nal_len = write_nal_unit(h, nal_buf, sizeof(nal_buf));
	WARN_ON_ONCE(nal_len >= sizeof(nal_buf));
	memcpy(*buf, nal_buf + 1, nal_len - 1);
	*buf += nal_len - 1;
	*space_left -= nal_len - 1;
}

void tw5864_h264_put_slice_header(h264_stream_t* h, u8 **buf, size_t *space_left, unsigned int idr_pic_id, unsigned int frame_seqno_in_gop, int *tail_nb_bits, u8 *tail)
{
	u8 nal_buf[64] = {0, };  /* Because of h264bitstream freaking behaviour, it adds zero byte in front of generated NAL header FIXME */
	int nal_len;

	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	h->nal->nal_ref_idc = 1;
	if (frame_seqno_in_gop == 0) {
		h->nal->nal_unit_type = NAL_UNIT_TYPE_CODED_SLICE_IDR;
		h->sh->slice_type = SH_SLICE_TYPE_I;
	} else {
		h->nal->nal_unit_type = NAL_UNIT_TYPE_CODED_SLICE_NON_IDR;
		h->sh->slice_type = SH_SLICE_TYPE_P;
	}
	h->sh->first_mb_in_slice = 0;
	h->sh->pic_parameter_set_id = 0;
	h->sh->frame_num = frame_seqno_in_gop;
	h->sh->idr_pic_id = idr_pic_id;
	h->sh->pic_order_cnt_lsb = frame_seqno_in_gop;
	h->sh->drpm.no_output_of_prior_pics_flag = 0;
	h->sh->drpm.long_term_reference_flag = 0;
	h->sh->slice_qp_delta = 0;

	nal_len = write_nal_unit_and_return_tail(h, nal_buf, sizeof(nal_buf), tail_nb_bits, tail);
	WARN_ON_ONCE(nal_len >= sizeof(nal_buf));
	memcpy(*buf, nal_buf + 1, nal_len - 1);
	*buf += nal_len - 1;
	*space_left -= nal_len - 1;
}
