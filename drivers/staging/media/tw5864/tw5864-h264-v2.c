#include "tw5864.h"
#include "tw5864-bs-v2.h"
#include "tw5864-h264-v2.h"
#if 0

typedef enum nal_priority           nal_priority_e;
typedef enum nal_unit_type          nal_unit_type_e;
typedef enum frame_type             frame_type_e;
typedef enum slice_type             slice_type_e;
typedef struct h264_nal             h264_nal_t;
typedef struct nal_info             H264_NAL_INFO;
typedef struct h264_nal_head        h264_nal_bitstream_t;
typedef struct ext_h264_nal_head    ext_h264_nal_bitstream_t;
typedef struct h264_sps             h264_sps_t;
typedef struct h264_pps             h264_pps_t;
typedef struct h264_slice_header    h264_slice_header_t;
#endif

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
    /* log2(GOP_SIZE), taken 5 */
#define i_log2_max_frame_num 5
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
    bs_write(s, 1, 0 /* b_frame_mbs_only */);
    bs_write(s, 1, 0 /* b_mb_adaptive_frame_field */);
    bs_write(s, 1, 0 /* b_direct8x8_inference */);
    bs_write(s, 1, 0);
    bs_write(s, 1, 0);
    bs_rbsp_trailing(s);
    return bs_len(s);
}

#if 0
int  gen_h264_sps(isil_h264_encode_control_t *h264_encode_control, isil_video_frame_tcb_t *frame)
{
    h264_nal_t              *nal;
    h264_nal_bitstream_t    *h264_nal_sps_head;
    H264_NAL_INFO           *nal_info;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD            *nal_sps_head;
    int len;
#endif
    isil_video_packet_tcb_queue_t	*video_packet_queue;
    isil_video_packet_tcb_t	*packet;

    nal = &frame->nal;
    video_packet_queue = &frame->video_packet_queue;
    packet = video_packet_queue->curr_producer;
    nal->sps_payload = &packet->data[frame->frame_len];
    memset(nal->sps_payload, 0, SLICE_HEAD_BUF_SIZE);
    h264_nal_sps_head = (h264_nal_bitstream_t*)nal->sps_payload;
#ifdef  ADD_DVM_NAL_HEAD
    nal_sps_head = &h264_nal_sps_head->head;
#endif
    nal_info = &h264_nal_sps_head->nal;

#ifdef  ADD_DVM_NAL_HEAD
    nal_sps_head->e_nal_type = DVM_NAL_SPS;
#endif
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->nal_ref_idc = nal->i_ref_idc = NAL_PRIORITY_HIGHEST;
    nal_info->nal_unit_type = nal->i_type = NAL_SPS;
    nal_info->forbidden_zero_bit = 0;

    nal->sps_paysize = sizeof(h264_nal_bitstream_t);
    nal->sps_paysize += h264_gen_sps_rbsp(h264_nal_sps_head->rbsp, h264_encode_control->sps);
    frame->frame_len += nal->sps_paysize;

#ifdef  ADD_DVM_NAL_HEAD
    len = nal->sps_paysize - sizeof(DVM_NAL_HEAD);
    nal_sps_head->i_nal_size_0_7 = (len & 0xff);
    nal_sps_head->i_nal_size_8_15 = ((len>>8) & 0xff);
    nal_sps_head->i_nal_size_16_23 = ((len>>16) & 0xff);
#endif
    return nal->sps_paysize;
}
#endif
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
#if 0
int gen_h264_pps(isil_h264_encode_control_t *h264_encode_control, isil_video_frame_tcb_t *frame)
{
    h264_nal_t              *nal;
    h264_nal_bitstream_t    *h264_nal_pps_head;
    H264_NAL_INFO           *nal_info;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD            *nal_pps_head;
    int     len;
#endif
    isil_video_packet_tcb_queue_t	*video_packet_queue;
    isil_video_packet_tcb_t	*packet;

    nal = &frame->nal;
    video_packet_queue = &frame->video_packet_queue;
    packet = video_packet_queue->curr_producer;
    nal->pps_payload = &packet->data[frame->frame_len]; 
    memset(nal->pps_payload, 0, 64);
    h264_nal_pps_head = (h264_nal_bitstream_t*)nal->pps_payload;
#ifdef  ADD_DVM_NAL_HEAD
    nal_pps_head = &h264_nal_pps_head->head;
#endif
    nal_info = &h264_nal_pps_head->nal;

#ifdef  ADD_DVM_NAL_HEAD
    nal_pps_head->e_nal_type = DVM_NAL_PPS;
#endif
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->nal_ref_idc = nal->i_ref_idc = NAL_PRIORITY_HIGHEST;
    nal_info->nal_unit_type = nal->i_type = NAL_PPS;
    nal_info->forbidden_zero_bit = 0;

    nal->pps_paysize = sizeof(h264_nal_bitstream_t);
    nal->pps_paysize += h264_gen_pps_rbsp(h264_nal_pps_head->rbsp, h264_encode_control->pps);
    frame->frame_len += nal->pps_paysize;

#ifdef  ADD_DVM_NAL_HEAD
    len = nal->pps_paysize - sizeof(DVM_NAL_HEAD);
    nal_pps_head->i_nal_size_0_7 = (len & 0xff);
    nal_pps_head->i_nal_size_8_15 = ((len>>8) & 0xff);
    nal_pps_head->i_nal_size_16_23 = ((len>>16) & 0xff);
#endif
    return nal->pps_paysize;
}
#endif
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
    if(i_poc_type == 0) {  /* always true, conditional can be dropped TODO */
        int i_poc_lsb = (frame_seqno_in_gop << 1);  /* why multiplied by two? TODO try without multiplication */
        bs_write(s, i_log2_max_poc_lsb, i_poc_lsb);
    }
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

    bs_write_se(s, QP_VALUE);

    if (s->i_left != 8){
        *tail = ((s->p[0])<<s->i_left);
        *tail_nb_bits = 8-s->i_left;
    } else {
        *tail = 0;
        *tail_nb_bits = 0;
    }

    return bs_len(s);
}
#if 0
int gen_h264_slicehead(isil_h264_encode_control_t *h264_encode_control, isil_video_frame_tcb_t *frame, int frame_type)
{
    h264_nal_t              *nal;
    h264_nal_bitstream_t    *h264_nal_slice_head;
    H264_NAL_INFO           *nal_info;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD            *nal_slice_head;
    int     len;
#endif
    isil_video_packet_tcb_queue_t	*video_packet_queue;
    isil_video_packet_tcb_t	*packet;
    u8  *ch;
    u32 temp_value;

    nal = &frame->nal;
    video_packet_queue = &frame->video_packet_queue;
    packet = video_packet_queue->curr_producer;
    nal->slice_payload = &packet->data[frame->frame_len]; 
    memset(nal->slice_payload, 0, 64);
    h264_nal_slice_head = (h264_nal_bitstream_t*)nal->slice_payload;
#ifdef  ADD_DVM_NAL_HEAD
    nal_slice_head = &h264_nal_slice_head->head;
#endif
    nal_info = &h264_nal_slice_head->nal;

    switch(frame_type){
        case H264_FRAME_TYPE_P:
#ifdef  ADD_DVM_NAL_HEAD
            nal_slice_head->e_nal_type = DVM_NAL_SLICE_P;
            nal_slice_head->i_nal_size_0_7 = 0;    
            nal_slice_head->i_nal_size_8_15 = 0;
            nal_slice_head->i_nal_size_16_23 = 0;
#endif
            nal_info->nal_unit_type = nal->i_type = NAL_SLICE;
            break;
        case H264_FRAME_TYPE_I:
#ifdef  ADD_DVM_NAL_HEAD
            nal_slice_head->e_nal_type = DVM_NAL_SLICE_I;
            nal_slice_head->i_nal_size_0_7 = 0;    
            nal_slice_head->i_nal_size_8_15 = 0;
            nal_slice_head->i_nal_size_16_23 = 0;
#endif
            nal_info->nal_unit_type = nal->i_type = NAL_SLICE;
            break;
        default:
#ifdef  ADD_DVM_NAL_HEAD
            nal_slice_head->e_nal_type = DVM_NAL_SLICE_IDR;
            nal_slice_head->i_nal_size_0_7 = 0;    
            nal_slice_head->i_nal_size_8_15 = 0;
            nal_slice_head->i_nal_size_16_23 = 0;
#endif
            nal_info->nal_unit_type = nal->i_type = NAL_SLICE_IDR;
            break;            
    }
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->forbidden_zero_bit = 0;
    nal_info->nal_ref_idc = nal->i_ref_idc = NAL_PRIORITY_LOW;

    nal->slice_paysize = sizeof(h264_nal_bitstream_t);
    nal->slice_paysize += h264_gen_slice_head(h264_nal_slice_head->rbsp, h264_encode_control->slice_head, nal);
    frame->frame_len += nal->slice_paysize;
    ch =  &packet->data[frame->frame_len];
    switch(frame->frame_len&0x1) {
        case 0:
            nal->i_temp_bitalign_content <<= 8;
            break;
        default:
            frame->frame_len--;
            ch--;
            temp_value = *ch;
            temp_value <<= 8;
            temp_value |= (nal->i_temp_bitalign_content);
            nal->i_temp_bitalign_content = temp_value;
            nal->i_temp_bitalign_number += 8;
            break;
    }

    frame->i_init_qp = h264_encode_control->pps->i_pic_init_qp;
    return nal->slice_paysize;
}

int gen_ext_nal_audio_header(char *buf, int audio_type, int len)
{
    ext_h264_nal_bitstream_t    *ext_nal_audio_head = (ext_h264_nal_bitstream_t*)buf;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD                *nal_audio_head;
#endif
    H264_NAL_INFO               *nal_info;
    int ret ;

#ifdef  ADD_DVM_NAL_HEAD 
    nal_audio_head = &ext_nal_audio_head->head;
#endif
    nal_info = &ext_nal_audio_head->nal;

#ifdef  ADD_DVM_NAL_HEAD
    ret = sizeof(ext_h264_nal_bitstream_t) + len - sizeof(DVM_NAL_HEAD);
    nal_audio_head->i_nal_size_0_7 = (ret&0xff);    
    nal_audio_head->i_nal_size_8_15 = ((ret>>8)&0xff);
    nal_audio_head->i_nal_size_16_23 = ((ret>>16)&0xff);
#endif
    switch(audio_type){
        default:
        case ISIL_AUDIO_PCM:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_PCM;
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_PCM;
            break;
        case ISIL_AUDIO_ALAW:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_ALAW; 
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_ALAW;
            break;
        case ISIL_AUDIO_ULAW:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_ULAW; 
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_ULAW;
            break;
        case ISIL_AUDIO_ADPCM_32K:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_IMAADPCM; 
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_IMAADPCM;
            break;
        case ISIL_AUDIO_ADPCM_48K:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_IMAADPCM; 
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_IMAADPCM;
            break;
        case ISIL_AUDIO_ADPCM_16K:
#ifdef  ADD_DVM_NAL_HEAD
            nal_audio_head->e_nal_type = DVM_NAL_AUDIO_IMAADPCM; 
#endif 
            ext_nal_audio_head->ext_type = NAL_TYPE_EX_A_IMAADPCM;
            break;
    }
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->forbidden_zero_bit = 0;
    nal_info->nal_ref_idc = NAL_PRIORITY_LOW;
    nal_info->nal_unit_type = NAL_TYPE_EX_AUDIO;

    ret = sizeof(ext_h264_nal_bitstream_t) + len;
    return ret;
}

int  gen_ext_mjpeg_header(char *buf, int len)
{
    ext_h264_nal_bitstream_t    *ext_nal_mjpeg_head = (ext_h264_nal_bitstream_t*)buf;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD                *nal_mjpeg_head;
#endif
    H264_NAL_INFO               *nal_info;
    int ret ;

#ifdef  ADD_DVM_NAL_HEAD 
    nal_mjpeg_head = &ext_nal_mjpeg_head->head;
#endif
    nal_info = &ext_nal_mjpeg_head->nal;
#ifdef  ADD_DVM_NAL_HEAD
    ret = sizeof(ext_h264_nal_bitstream_t) + len - sizeof(DVM_NAL_HEAD);
    nal_mjpeg_head->i_nal_size_0_7 = (ret&0xff);
    nal_mjpeg_head->i_nal_size_8_15 = ((ret>>8)&0xff);
    nal_mjpeg_head->i_nal_size_16_23 = ((ret>>16)&0xff);
    nal_mjpeg_head->e_nal_type = DVM_NAL_MJPEG;
#endif
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->forbidden_zero_bit = 0;
    nal_info->nal_ref_idc = NAL_PRIORITY_LOW;
    nal_info->nal_unit_type = NAL_TYPE_EX_PIC;

    ext_nal_mjpeg_head->ext_type = NAL_TYPE_EX_P_MJPEG;
    ret = sizeof(ext_h264_nal_bitstream_t) + len;
    return ret;
}

int  gen_ext_mv_header(char *buf, int len)
{
    ext_h264_nal_bitstream_t    *ext_nal_mv_head = (ext_h264_nal_bitstream_t*)buf;
#ifdef  ADD_DVM_NAL_HEAD
    DVM_NAL_HEAD                *nal_mv_head;
#endif
    H264_NAL_INFO               *nal_info;
    int ret ;

#ifdef  ADD_DVM_NAL_HEAD 
    nal_mv_head = &ext_nal_mv_head->head;
#endif
    nal_info = &ext_nal_mv_head->nal;

#ifdef  ADD_DVM_NAL_HEAD
    ret = sizeof(ext_h264_nal_bitstream_t) + len - sizeof(DVM_NAL_HEAD);
    nal_mv_head->i_nal_size_0_7 = (ret&0xff);
    nal_mv_head->i_nal_size_8_15 = ((ret>>8)&0xff);
    nal_mv_head->i_nal_size_16_23 = ((ret>>16)&0xff);
    nal_mv_head->e_nal_type = DVM_NAL_MV;
#endif
#if NAL_FILL_ZERO_NUM
    nal_info->zero1 = 0;
    nal_info->zero2 = 0;
#endif
    nal_info->zero3 = 0;
    nal_info->zero4 = 0;
    nal_info->zero5 = 0;
    nal_info->constant_value_1 = 1;
    nal_info->forbidden_zero_bit = 0;
    nal_info->nal_ref_idc = NAL_PRIORITY_LOW;
    nal_info->nal_unit_type = NAL_TYPE_EX_MV;

    ext_nal_mv_head->ext_type = NAL_TYPE_EX_MV_D1;
    ret = sizeof(ext_h264_nal_bitstream_t) + len;
    return ret;
}

#endif
