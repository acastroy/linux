int h264_gen_sps_rbsp(void *p_data);
int h264_gen_pps_rbsp(void *p_data);
int h264_gen_slice_head(void *p_data, unsigned int idr_pic_id, unsigned int frame_seqno_in_gop, int *tail_nb_bits, u8 *tail);
