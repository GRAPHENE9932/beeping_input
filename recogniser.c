#include "recogniser.h"
#include "to_morse_encoder.h"

#include <kiss_fft.h>

#include <stdio.h>
#include <math.h>

#define SEGMENT_DURATION 0.01f
#define CUTOFF_LEVEL 0.5f
#define MIN_FREQ 200.0f
#define MAX_FREQ 4000.0f
int samples_per_segment = 0;

kiss_fft_cfg cfg = NULL;

int cur_off_in_segment = 0;
kiss_fft_cpx* segment_in = NULL;
kiss_fft_cpx* segment_out = NULL;

void recogniser_init(int samples_per_second) {
    samples_per_segment = samples_per_second * SEGMENT_DURATION;
    segment_in = malloc(sizeof(kiss_fft_cpx) * samples_per_segment);
    segment_out = malloc(sizeof(kiss_fft_cpx) * samples_per_segment);

    initialize_encoder(SEGMENT_DURATION);
}

float calculate_frequency(int index_of_sample_in_segment) {
    return (float)index_of_sample_in_segment / SEGMENT_DURATION;
}

float calculate_average(void) {
    float sum = 0.0f;
    int amount = 0;
    for (size_t i = 0; i < samples_per_segment; i++) {
        float frequency = calculate_frequency(i);
        if (frequency < MIN_FREQ || frequency > MAX_FREQ) {
            continue;
        }

        if (segment_out[i].r < 0.0f) {
            sum -= segment_out[i].r;
        }
        else {
            sum += segment_out[i].r;
        }
        amount++;
    }

    return sum / amount;
}

void process_segment(void) {
    if (cfg == NULL) {
        cfg = kiss_fft_alloc(samples_per_segment, 0, NULL, NULL);
    }
    kiss_fft(cfg, segment_in, segment_out);
    float average = calculate_average();

    feed_segment(average > CUTOFF_LEVEL);
    /*if (average > CUTOFF_LEVEL) {
        printf("BEEP\n");
    }
    else {
        printf("....\n");
    }*/
}

void feed_sample(float sample) {
    if (cur_off_in_segment == samples_per_segment) {
        cur_off_in_segment = 0;
        process_segment();
    }

    segment_in[cur_off_in_segment].r = sample;
    segment_in[cur_off_in_segment++].i = 0.0f;
}