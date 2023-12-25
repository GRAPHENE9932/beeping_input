#include "recogniser.h"

#include <soundio/soundio.h>

#include <stdio.h>

void read_callback(struct SoundIoInStream* instream, int frame_count_min, int frame_count_max) {
    float sample_rate = instream->sample_rate;
    float seconds_per_frame = 1.0f / sample_rate;
    struct SoundIoChannelArea* areas;
    int frames_left = frame_count_max;
    int err;

    while(frames_left > 0) {
        int frame_count = frames_left;
        if ((err = soundio_instream_begin_read(instream, &areas, &frame_count))) {
            fprintf(stderr, "Failed to begin the read: \"%s\".\n", soundio_strerror(err));
            return;
        }
        if (!frame_count) {
            break;
        }

        // printf("Frames: %d\n", frame_count);
        for (int frame = 0; frame < frame_count; frame++) {
            float* ptr = (float*)(areas[0].ptr + areas[0].step * frame);
            feed_sample(*ptr);
        }

        if ((err = soundio_instream_end_read(instream))) {
            fprintf(stderr, "Failed to end the read: \"%s\".\n", soundio_strerror(err));
            return;
        }
        frames_left -= frame_count;
    }
}

void overflow_callback(struct SoundIoInStream* in_stream) {
    printf("overflow_callback\n");
}

int main(int argc, char** argv) {
    struct SoundIo* soundio = soundio_create();
    if (!soundio) {
        fprintf(stderr, "Failed to create the soundio object.\n");
        return 1;
    }

    int err;
    if ((err = soundio_connect(soundio)) != 0) {
        fprintf(stderr, "Failed to connect the soundio: \"%s\".\n", soundio_strerror(err));
        return 1;
    }

    soundio_flush_events(soundio);

    int default_micro_index = soundio_default_input_device_index(soundio);
    if (default_micro_index < 0) {
        fprintf(stderr, "No input device found.\n");
        return 1;
    }

    struct SoundIoDevice* device = soundio_get_input_device(soundio, default_micro_index);
    if (!device) {
        fprintf(stderr, "Failed to get the input device.\n");
        return 1;
    }
    fprintf(stdout, "Input device: %s\n", device->name);
    soundio_device_sort_channel_layouts(device);

    struct SoundIoInStream* instream = soundio_instream_create(device);
    if (!instream) {
        fprintf(stderr, "Failed to create the input stream.\n");
        return 1;
    }
    instream->read_callback = read_callback;
    instream->software_latency = 0.5;
    instream->layout = *soundio_channel_layout_get_default(1);
    instream->sample_rate = 44100;
    instream->overflow_callback = overflow_callback;
    instream->format = SoundIoFormatFloat32NE;
    if ((err = soundio_instream_open(instream) != 0)) {
        fprintf(stderr, "Failed to open the input stream: \"%s\".\n", soundio_strerror(err));
        return 1;
    }

    recogniser_init(instream->sample_rate);

    if ((err = soundio_instream_start(instream)) != 0) {
        fprintf(stderr, "Failed to start the input stream: \"%s\".\n", soundio_strerror(err));
    }

    while (true) {
        soundio_wait_events(soundio);
    } 

    soundio_instream_destroy(instream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
}