extern "C" {

typedef struct {
	short r, i;
} cint16_t;

static short filter_taps_fm[] = {
    0,
    0,
    -22,
    -22,
    184,
    184,
    295,
    295,
    -507,
    -507,
    -1150,
    -1150,
    571,
    571,
    2672,
    2672,
    261,
    261,
    -4361,
    -4361,
    -2383,
    -2383,
    5214,
    5214,
    5406,
    5406,
    -4339,
    -4339,
    -8139,
    -8139,
    1696,
    1696,
    9245,
    9245,
    1696,
    1696,
    -8139,
    -8139,
    -4339,
    -4339,
    5406,
    5406,
    5214,
    5214,
    -2383,
    -2383,
    -4361,
    -4361,
    261,
    261,
    2672,
    2672,
    571,
    571,
    -1150,
    -1150,
    -507,
    -507,
    295,
    295,
    184,
    184,
    -22,
    -22
};

// max size of x buf 561600
void fir_q15_execute_fm(short* x, float* y) {
	static cint16_t taps[32] = {0};
#pragma HLS array_partition variable=taps complete

	for (int i = 0; i < 140400; i++) {
		// push x[0]
		for (unsigned int i = 0; i < 32-1; i++) {
			taps[i] = taps[i+1];
		}
		taps[31].r = x[i*2];
		taps[31].i = x[i*2 + 1];

		// dotprod_32
		short sum_r = 0;
		short sum_i = 0;
		for (unsigned int i = 1; i < 16; i++) {
			int tmp_r = (int)(taps[i].r + taps[32 - i].r) * (int)(filter_taps_fm[i * 2]);
			int tmp_i = (int)(taps[i].i + taps[32 - i].i) * (int)(filter_taps_fm[i * 2]);

			sum_r += (short)(tmp_r >> 15);
			sum_i += (short)(tmp_i >> 15);
		}
		int tmpf_r = (int)(taps[16].r) * (int)(filter_taps_fm[32]);
		int tmpf_i = (int)(taps[16].i) * (int)(filter_taps_fm[32]);

		sum_r += (short)(tmpf_r >> 15);
		sum_i += (short)(tmpf_i >> 15);

		y[i*2 + 0] = (float)sum_r / 32767.0f; // real
		y[i*2 + 1] = (float)sum_i / -32767.0f; // imag
	}
}


}
