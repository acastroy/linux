#include "tw5864.h"
#include "tw5864-bs-v2.h"
#include "tw5864-h264-v2.h"

/****************************************************************************************/
/****************************************************************************************/
int h264_gen_sps_rbsp(void *p_data)
{
    bs_t    bs, *s;

    s = &bs;
    bs_init(s, p_data, 64);
    bs_write(s, 8, 0x42  /* profile == 66, baseline */);
    bs_write(s, 8, 0 /* constraints */);
    bs_write(s, 8, 0x1E /* level */);
    bs_write_ue(s, 0 /* SPS id */);
    /* log2(GOP_SIZE), taken 7 */
#define i_log2_max_frame_num 7
    bs_write_ue(s, i_log2_max_frame_num - 4);
#define i_poc_type 0
    bs_write_ue(s, i_poc_type);
#define i_log2_max_poc_lsb i_log2_max_frame_num
    bs_write_ue(s, i_log2_max_poc_lsb - 4);

    bs_write_ue(s, 1 /* i_num_ref_frames */);
    bs_write(s, 1, 0 /* b_gaps_in_frame_num_value_allowed */);
#define i_mb_width (720 >> 4)
    bs_write_ue(s, i_mb_width - 1);
#define i_mb_height (480 >> 4)  /* TODO PAL */
    bs_write_ue(s, i_mb_height - 1);
    bs_write(s, 1, 1 /* b_frame_mbs_only */);
    bs_write(s, 1, 0 /* b_direct8x8_inference */);
    bs_write(s, 1, 0);
    bs_write(s, 1, 0);
    bs_rbsp_trailing(s);
    return bs_len(s);
}

int h264_gen_pps_rbsp(void *p_data)
{
    bs_t    bs, *s;

    s = &bs;
    bs_init(s, p_data, 64);
    bs_write_ue(s, 0 /* PPS id */);
    bs_write_ue(s, 0 /* SPS id */);
    bs_write(s, 1, 0 /* b_cabac */);
    bs_write(s, 1, 0 /* b_pic_order */);
    bs_write_ue(s, (1 /* i_num_slice_groups */) - 1);
    bs_write_ue(s, (1 /* i_num_ref_idx_l0_active */) - 1);
    bs_write_ue(s, (1 /* i_num_ref_idx_l1_active */) - 1);
    bs_write(s, 1, 0 /* b_weighted_pred */);
    bs_write(s, 2, 0 /* b_weighted_bipred */);
    bs_write_se(s, QP_VALUE - 26);
    bs_write_se(s, QP_VALUE - 26);
    bs_write_se(s, 0 /* i_chroma_qp_index_offset */);
    bs_write(s, 1, 0 /* b_deblocking_filter_control */);
    bs_write(s, 1, 0 /* b_constrained_intra_pred */);
    bs_write(s, 1, 0 /* b_redundant_pic_cnt */);
    bs_rbsp_trailing(s);
    return bs_len(s);
}

int h264_gen_slice_head(void *p_data, unsigned int idr_pic_id, unsigned int frame_seqno_in_gop, int *tail_nb_bits, u8 *tail)
{
    bs_t    bs, *s;

    s = &bs;
    bs_init(s, p_data, 64);
    bs_write_ue(s, 0 /* i_first_mb */);
    bs_write_ue(s, (frame_seqno_in_gop == 0) ? 2 : 5 /* slice type - I or P */);
    bs_write_ue(s, 0 /* PPS id */);
    bs_write(s, i_log2_max_frame_num, frame_seqno_in_gop);
    if(frame_seqno_in_gop == 0)
        bs_write_ue(s, idr_pic_id);

    int i_poc_lsb = (frame_seqno_in_gop << 1);  /* why multiplied by two? TODO try without multiplication */
    bs_write(s, i_log2_max_poc_lsb, i_poc_lsb);

    if(!(frame_seqno_in_gop == 0))
        bs_write1(s, 0 /*b_num_ref_idx_override */);

    /* ref pic list reordering */
    if (!(frame_seqno_in_gop == 0))
        bs_write1(s, 0 /* b_ref_pic_list_reordering_l0 */);

    if (frame_seqno_in_gop == 0) {
        bs_write1(s, 0);  /* no output of prior pics flag */
        bs_write1(s, 0);  /* long term reference flag */
    } else
        bs_write1(s, 0);  /* adaptive_ref_pic_marking_mode_flag */

    bs_write_se(s, 0 /* qp delta? */);

    if (s->i_left != 8){
        *tail = ((s->p[0])<<s->i_left);
        *tail_nb_bits = 8-s->i_left;
    } else {
        *tail = 0;
        *tail_nb_bits = 0;
    }

    return bs_len(s);
}
