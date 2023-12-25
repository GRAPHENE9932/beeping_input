#include "to_morse_encoder.h"
#include "from_morse_encoder.h"

#include <stdio.h>

float segment_duration = 0.0f;

#define DOT_DURATION 0.03f
#define DASH_DURATION 0.175f
#define PAUSE_DURATION 0.01f
#define BIG_PAUSE_DURATION 0.3f
#define BEGINNING_DURATION 0.02f

float cur_signal_dur = 0.0f;
float cur_silence_dur = 0.0f;
char signal_begun = 0;

void initialize_encoder(float segment_duration_arg) {
    segment_duration = segment_duration_arg;
}

void feed_segment(char segment) {
    if (segment) {
        cur_signal_dur += segment_duration;
    }
    else {
        cur_silence_dur += segment_duration;
    }

    if (!signal_begun) {
        if (cur_signal_dur >= BEGINNING_DURATION) {
            signal_begun = 1;
        }
    }


    if (signal_begun) {
        if (cur_silence_dur >= BIG_PAUSE_DURATION) {
            feed_symbol(' ');
            fflush(stdout);
            cur_signal_dur = 0.0;
            cur_silence_dur = 0.0f;
        }
        else if (cur_silence_dur >= PAUSE_DURATION) {
            signal_begun = 0;
            cur_silence_dur = 0.0f;
            
            if (cur_signal_dur >= DASH_DURATION) {
                feed_symbol('-');
                fflush(stdout);
            }
            else if (cur_signal_dur >= DOT_DURATION) {
                feed_symbol('.');
                fflush(stdout);
            }

            cur_signal_dur = 0.0;
        }
    }
}