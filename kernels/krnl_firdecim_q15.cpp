extern "C" {

typedef struct {
	short r, i;
} cint16_t;

/*
 * Low pass filter with 40db attenuation for 1488375 sample rate
 * Transition starts at 372094 and ends at 530000.
 *
 */

static short decim_taps_q15[] = {
	-134,
	-134,
	1078,
	1078,
	-4417,
	-4417,
	19864,
	19864
};

void fir_q15_execute_decim(unsigned char* x, cint16_t* y, unsigned int len) {
	static cint16_t taps[15] = {0};
#pragma HLS array_partition variable=taps complete

	for (int i = 0; i < len; i += 4) {
		// push x[0]
		for (unsigned int i = 0; i < 14; i++) {
			taps[i] = taps[i+1];
		}
		taps[14].r = ( ((short)(x[i]) - 127) * 64 );
		taps[14].i = ( ((short)(x[i + 1]) - 127) * 64 );

		// dotprod_halfband_4
		short sum_r = 0;
		short sum_i = 0;
		for (unsigned int i = 0; i < 7; i += 2) {
			int tmp_r = (int)(taps[i].r + taps[14 - i].r) * (int)(decim_taps_q15[i]);
			int tmp_i = (int)(taps[i].i + taps[14 - i].i) * (int)(decim_taps_q15[i]);

			sum_r += (short)(tmp_r >> 15);
			sum_i += (short)(tmp_i >> 15);
		}
		sum_r += taps[7].r;
		sum_i += taps[7].i;

		// save results
		y[i/4].r = sum_r;
		y[i/4].i = sum_i;

		// push x[1]
		for (unsigned int i = 0; i < 14; i++) {
			taps[i] = taps[i+1];
		}
		taps[14].r = ( ((short)(x[i + 2]) - 127) * 64 );
		taps[14].i = ( ((short)(x[i + 3]) - 127) * 64 );
	}
}

}
