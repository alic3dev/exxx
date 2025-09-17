#include <exxx.h>

#include <cer0.h>
#include <clic3.h>

#include <CoreAudio/CoreAudio.h>

#include <stdio.h>

const char exxx_octave_maximum = 5;
const char exxx_octave_minimum = 0;

const unsigned int exxx_channel_left = 0;
const unsigned int exxx_channel_right = 1;

const float exxx_sample_rate = 44100.0f;

struct exxx_input_data exxx_input_data = {
  .baseDir = "./assets/cho/",
  .boundary = "boundary.json",
  .parks = "parks.json",
  .streams = "streams.json",
  .contours = "contours.json",
  .railroads = "railroads.json",
  .structures = "structure_existing_area.json",
  .lakes = "lakes.json",
  .roads = "roads.json",
  .surfaces = "surfaces.json"
};

int main(
  int length_parameters,
  char** parameters
) {
  char* path_data = clic3_char_arrays_concatenate(
    exxx_input_data.baseDir,
    exxx_input_data.structures
  );

  FILE* data_file = fopen(
    path_data,
    "r"
  );

  if (
    data_file == (void*)0
  ) {
    fprintf(
      stderr,
      "failed_to_open->{%s};\n",
      path_data
    );

    free(path_data);

    return 1;
  }

  free(path_data);

  unsigned int length_points = 0;
  float* points = malloc(
    sizeof(float) *
    length_points
  );

  unsigned int length_buffer = 0;
  char* buffer = malloc(
    sizeof(char) *
    length_buffer
  );

  char data_character = fgetc(data_file);

  do {
    if (
      (data_character >= '0' && data_character <= '9') ||
      data_character == '.'
    ) {
      length_buffer++;

      buffer = realloc(
        buffer,
        sizeof(char) *
        length_buffer
      );

      buffer[
        length_buffer - 1
      ] = data_character;
    } else if (length_buffer > 0) {
      float float_buffer;
      
      unsigned char status_buffer_conversion = clic3_char_array_to_float(
        buffer,
        &float_buffer
      );

      if (
        status_buffer_conversion == 0 &&
        float_buffer != 0.0f
      ) {
        length_points = (
          length_points + 1
        );

        points = realloc(
          points,
          sizeof(float) *
          length_points
        );

        points[
          length_points - 1
        ] = float_buffer;
      }

      length_buffer = 0;

      buffer = realloc(
        buffer,
        sizeof(char) *
        length_buffer
      );
    }

    data_character = fgetc(
      data_file
    );
  } while (
    data_character != EOF &&
    feof(data_file) == 0
  );

  free(buffer);

  fclose(data_file);

  float* table_notes = cer0_note_table_create(
    exxx_octave_minimum,
    exxx_octave_maximum,
    cer0_frequency_root_standard
  );

  struct exxx_data_client* data_client = exxx_data_client_initialize(
    table_notes,
    points,
    length_points
  );

  AudioDeviceID device_default = exxx_device_default_get();

  AudioDeviceIOProcID id_proc_io = exxx_audio_initialize(
    device_default,
    data_client
  );

  if (
    id_proc_io == (void*)0
  ) {
    exxx_destroy(
      device_default,
      id_proc_io
    );
    
    return 1;
  }

  exxx_loop_input(
    data_client,
    table_notes
  );

  free(points);

  exxx_destroy(
    device_default,
    id_proc_io
  );

  return 0;
}

void exxx_loop_input(
  struct exxx_data_client* data_client,
  float* table_notes
) {
  unsigned char quitting = 0;

  unsigned char length_input_user = 100;
  char input_user[length_input_user];

  while (
    quitting != 1
  ) {
    printf(
      ":"
    );

    scanf(
      "%s",
      input_user
    );

    input_user[
      length_input_user - 1
    ] = '\0';

    if (
      clic3_char_arrays_within(
        input_user,
        3,
        "q",
        "Q",
        "quit"
      ) != -1
    ) {
      quitting = 1;
    } else if (
      input_user[0] == 's' ||
      input_user[0] == 'S'
    ) {
      unsigned int signal_input;
      
      unsigned char status_input_user_conversion = clic3_char_array_to_unsigned_int(
        input_user + 1,
        &signal_input
      );

      if (
        signal_input >= 0 &&
        signal_input < cer0_length_signals
      ) {
        data_client->signal = signal_input;

        printf(
          "signal->{%s};\n",
          cer0_signal_name_lookup[
            signal_input
          ]
        );
      } else {
        printf(
          "signal_invalid->{%s};\n",
          input_user + 1
        );
      }
    } else if (
      input_user[0] == 'v' ||
      input_user[0] == 'V'
    ) {
      float volume_input;

      unsigned char status_input_user_conversion = clic3_char_array_to_float(
        input_user + 1,
        &volume_input
      );

      if (
        status_input_user_conversion == 0 &&
        volume_input >= 0.0f
      ) {
        data_client->volume = volume_input;

        printf(
          "volume->{%f};\n",
          volume_input
        );
      } else {
        fprintf(
          stderr,
          "volume_invalid->{%s};\n",
          input_user + 1
        );
      }
    } else if (
      clic3_char_arrays_within(
        input_user,
        3,
        "h",
        "H",
        "help"
      ) != -1
    ) {
      printf(
        "exxx: commands\n"
        "  #    note[0-%i]\n"
        "  s#   signal_set\n",
        data_client->length_table_notes - 1
      );

      for (
        unsigned short int index_signal = 0;
        index_signal < cer0_length_signals;
        ++index_signal
      ) {

        printf(
          "    [%i]->{%s}\n",
          index_signal,
          cer0_signal_name_lookup[
            index_signal
          ]
        );
      }

      printf(
        "  h     help\n"
        "  q     quit\n"
      );
    } else {
      unsigned int int_input_user;

      unsigned char status_input_user_conversion = clic3_char_array_to_unsigned_int(
        input_user,
        &int_input_user
      );

      if (
        status_input_user_conversion == 0 &&
        int_input_user < data_client->length_table_notes
      ) {
        data_client->frequency = table_notes[
          int_input_user
        ];

        printf(
          "frequency->{%f};\n",
          data_client->frequency
        );
      } else {
        fprintf(
          stderr,
          "input.unknown->{%s};\n"
          "--['h'.for->{help}];\n",
          input_user
        );
      }
    }

    printf(
      "\n"
    );
  }
}

struct exxx_data_client* exxx_data_client_initialize(
  float* table_notes,
  float* points,
  unsigned int length_points
) {
  static struct exxx_data_client data_client;

  data_client.fileIndex = 0;

  data_client.table_notes = table_notes;
  data_client.length_table_notes = cer0_note_table_length(
    exxx_octave_minimum,
    exxx_octave_maximum
  );
  data_client.frequency = table_notes[33];
  data_client.value_phase = 0.0f;
  data_client.amplitude = 1.0f;
  data_client.volume = 0.4f;
  data_client.frame = 0;

  data_client.points = points;
  data_client.length_points = length_points;

  printf(
    "frequency->{%f};\n",
    data_client.frequency
  );
  printf(
    "value_phase->{%f};\n",
    data_client.value_phase
  );
  printf(
    "amplitude->{%f};\n",
    data_client.amplitude
  );
  printf(
    "volume->{%f};\n",
    data_client.volume
  );

  return &data_client;
}

AudioDeviceID exxx_device_default_get(void) {
  AudioObjectPropertyAddress audio_object_property_address = {
    kAudioHardwarePropertyDefaultSystemOutputDevice,
    kAudioObjectPropertyScopeOutput,
    kAudioObjectPropertyElementMain
  };

  AudioDeviceID id_device = kAudioDeviceUnknown;
  unsigned int id_device_size = sizeof(id_device);

  AudioObjectGetPropertyData(
    kAudioObjectSystemObject,
    &audio_object_property_address,
    0,
    (void*)0,
    &id_device_size,
    &id_device
  );

  return id_device;
}

AudioDeviceIOProcID exxx_audio_initialize(
  AudioDeviceID device_default,
  void* data_client
) {
  AudioDeviceIOProcID id_proc_io;

  OSStatus status_device_start = AudioDeviceStart(
    device_default,
    (void*)0
  );

  if (
    status_device_start != 0
  ) {
    fprintf(
      stderr,
      "failed_to_start->{device:[%i]:%i};\n",
      device_default,
      status_device_start
    );

    return (void*)0;
  }

  OSStatus status_io_proc_id_create = AudioDeviceCreateIOProcID(
    device_default,
    exxx_proc_io,
    (void*)(data_client),
    &id_proc_io
  );

  if (
    status_io_proc_id_create != 0
  ) {
    fprintf(
      stderr,
      "failed_to_create->{io_proc_id};\n"
    );

    return (void*)0;
  }

  OSStatus status_device_io_proc_start = AudioDeviceStart(
    device_default,
    id_proc_io
  );

  if (
    status_device_io_proc_start != 0
  ) {
    fprintf(
      stderr,
      "failed_to_start->{device_io_proc:[%i]:%i};\n",
      device_default,
      status_device_io_proc_start
    );
  
    return (void*)0;
  }

  return id_proc_io;
}

OSStatus exxx_proc_io(
  AudioObjectID device_in,
  const AudioTimeStamp* time_now_in,
  const AudioBufferList* exxx_input_data,
  const AudioTimeStamp* time_input_in,
  AudioBufferList* data_out,
  const AudioTimeStamp* time_output_in,
  void* data_client_in
) {
  struct exxx_data_client* data_client = data_client_in;
  unsigned long int frame = data_client->frame;

  float amplitude = data_client->amplitude;
  float volume = data_client->volume;
  float value_phase = data_client->value_phase;

  unsigned int length_buffers = data_out->mNumberBuffers;

  for (
    unsigned int index_buffer = 0;
    index_buffer < length_buffers;
    ++index_buffer
  ) {
    AudioBuffer audio_buffer_current = data_out->mBuffers[
      index_buffer
    ];

    float* buffer_out = audio_buffer_current.mData;

    unsigned int length_buffer_out = (
      audio_buffer_current.mDataByteSize /
      sizeof(float)
    );

    data_client->frequency = (
      fmod(
        data_client->points[
          data_client->frame %
          data_client->length_points
        ],
        1000.0f
      )
    );

    float frequency = data_client->frequency;
    float increment_phase = cer0_phase_get_increment(
      exxx_sample_rate,
      frequency
    );

    unsigned int count_channel_out = audio_buffer_current.mNumberChannels;

    for (
      unsigned int index_buffer_out = 0;
      index_buffer_out < length_buffer_out;
      ++index_buffer_out
    ) {
      unsigned int channel_current = (
        index_buffer_out % 2
      );

      float value_signal = (
        cer0_signal_function_lookup[
          data_client->signal
        ](
          value_phase
        )
      );

      buffer_out[
        index_buffer_out
      ] = (
        value_signal *
        amplitude *
        volume
      );

      if (
        channel_current == count_channel_out - 1
      ) {
        value_phase = cer0_phase_advance(
          value_phase,
          increment_phase
        );

        data_client->value_phase = value_phase;
      }
    }
  }

  data_client->frame = (
    data_client->frame +
    1
  ) % (UINT_MAX - 1);

  return 0;
}

unsigned int exxx_destroy(
  AudioDeviceID device,
  AudioDeviceIOProcID id_proc_io
) {
  unsigned int result = 0;

  if (
    id_proc_io != (void*)0
  ) {
    OSStatus status_io_proc_id_destroy = AudioDeviceDestroyIOProcID(
      device,
      id_proc_io
    );

    if (
      status_io_proc_id_destroy != 0
    ) {
      fprintf(
        stderr,
        "failed_to_destroy->{io_proc_id:[%i]};\n",
        status_io_proc_id_destroy
      );

      result = result + 1;
    }
    
    OSStatus status_device_io_proc_stop = AudioDeviceStop(
      device,
      id_proc_io
    );

    if (
      status_device_io_proc_stop != 0
    ) {
      fprintf(
        stderr,
        "failed_to_stop->{device_io_proc:[%i]:%i};\n",
        device,
        status_device_io_proc_stop
      );
      result = result + 1;
    }
  }
  
  OSStatus status_device_stop = AudioDeviceStop(
    device,
    (void*)0
  );

  if (
    status_device_stop != 0
  ) {
    fprintf(
      stderr,
      "failed_to_stop->{device:[%i]:%i};\n",
      device,
      status_device_stop
    );

    result = result + 1;
  }

  return result;
}
