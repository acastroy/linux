#include "tw5864.h"
#include "tw5864-h264-v2.h"

uint8_t marker[] = {0x00, 0x00, 0x00, 0x01};

void tw5864_h264_put_stream_header(u8 **buf, size_t *space_left, int qp, int width, int height)
{
	int nal_len;

	/* SPS */
	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	**buf = 0x67;  /* SPS NAL header */
	*buf += 1;
	*space_left -= 1;

	/* FIXME HARDCODE Unhardcode dimensions, QP */
	nal_len = h264_gen_sps_rbsp(*buf);
	*buf += nal_len;
	*space_left -= nal_len;


	/* PPS */
	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	**buf = 0x68;  /* PPS NAL header */
	*buf += 1;
	*space_left -= 1;

	nal_len = h264_gen_pps_rbsp(*buf);
	*buf += nal_len;
	*space_left -= nal_len;
}

void tw5864_h264_put_slice_header(u8 **buf, size_t *space_left, unsigned int idr_pic_id, unsigned int frame_seqno_in_gop, int *tail_nb_bits, u8 *tail)
{
	int nal_len;

	WARN_ON_ONCE(*space_left < 4);
	memcpy(*buf, marker, sizeof(marker));
	*buf += 4;
	*space_left -= 4;

	**buf = (frame_seqno_in_gop == 0) ? 0x25 : 0x21;  /*  NAL header */
	*buf += 1;
	*space_left -= 1;

	nal_len = h264_gen_slice_head(*buf, idr_pic_id, frame_seqno_in_gop, tail_nb_bits, tail);
	*buf += nal_len;
	*space_left -= nal_len;
}
