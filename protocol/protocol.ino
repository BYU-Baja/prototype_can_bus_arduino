#include <stdint.h>
typedef uint32_t frame_id;
#define FRAME_MAX_DATA_LENGTH 256
#define FRAME_MAX_LEN 263 // 1 + max data length + length of id + 1(data length byte) => 1(start of frame) + 256 + 4 + 1 + 1(end of frame)
#define BYTES_PER_UINT_32 4
#define DATA_BEGIN_OFFSET 6
#define START_OF_FRAME 0x00
#define END_OF_FRAME 0xFF
#define INTERNAL_TEMP_ID 0x001
#define TIME_MILLI_ID 0x002
#define RANDOM_FRAME0_ID 0x003
#define RANDOM_FRAME1_ID 0x004
#define RANDOM_FRAME2_ID 0x005
#define RANDOM_FRAME3_ID 0x013
// This is a single frame.
// The frame will be sent over the RF transiever.
struct rf_frame {
  frame_id id;
  uint8_t data_length;
  uint8_t data[FRAME_MAX_DATA_LENGTH] __attribute__((aligned(8)));
};
// Convert between uint32_t and a byte array
union uint32_uint8_converter {
  uint32_t value;
  uint8_t data[BYTES_PER_UINT_32 - 1];
};
// Convert between float and a byte array
union float_uint8_converter {
  float value;
  uint8_t data[BYTES_PER_UINT_32 - 1];
};
// Converts a frame to a buffer.
// frame - frame to convert
// buf - buffer to put the frame into. buf should be large enough to accomadate the entire frame.
// a good size is the max frame size FRAME_MAX_LEN.
// buf_length - length of the buffer when done.
void frame_to_buff(rf_frame frame, uint8_t *buf, uint16_t *buf_length) {
  buf[0] = START_OF_FRAME; // Start of frame.
  // Set id in buffer
  uint32_uint8_converter converter;
  converter.value = frame.id;
  buf[1] = converter.data[3];
  buf[2] = converter.data[2];
  buf[3] = converter.data[1];
  buf[4] = converter.data[0];
  // Insert data length
  buf[DATA_BEGIN_OFFSET - 1] = frame.data_length;
  // Insert data from frame into buffer.
  for (uint8_t i = 0; i < frame.data_length; i++) {
      buf[DATA_BEGIN_OFFSET + i] = frame.data[i];
  }
  *buf_length = DATA_BEGIN_OFFSET + frame.data_length + 1;
  buf[*buf_length - 1] = END_OF_FRAME; // put a newline at the end. This is the end of frame indicator.
}
// copied from the interwebs
// Gets the internal temperature of the arduino.
float get_temp(void)
{
  unsigned int wADC;
  float t;
  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.
  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC
  delay(20);            // wait for voltages to become stable.
  ADCSRA |= _BV(ADSC);  // Start the ADC
  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;
  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;
  // The returned temperature is in degrees Celcius.
  return (t);
}
// Sends a frame over serial.
// Creates a buffer and convertst the frame to aa buffer.
// Once the buffer is created it sends the whole thing over Serial.
void send_frame(rf_frame frame) {
  uint8_t buf[FRAME_MAX_LEN];
  uint16_t buf_len;
  frame_to_buff(frame, buf, &buf_len);
//  for (uint8_t i = 0; i < buf_len; i++) {
//    Serial.print(buf[i], HEX);
//    Serial.print(' ');
//  }
//  Serial.print('\n');
  Serial.write(buf, buf_len);
}
// Sends internal temperature data.
// Creates a frame and fills in the frame info.
void send_temp_data() {
  float temp = get_temp();
  float_uint8_converter converter;
  converter.value = temp;
  rf_frame frame;
  frame.id = INTERNAL_TEMP_ID;
  frame.data_length = 4;
  frame.data[0] = converter.data[3];
  frame.data[1] = converter.data[2];
  frame.data[2] = converter.data[1];
  frame.data[3] = converter.data[0];
  send_frame(frame);
}
// Sends milliseconds that have passed since start.
// Creates a frame and populates the info.
void send_time_data() {
  unsigned long time_milli = millis();
  uint32_uint8_converter converter;
  converter.value = (uint32_t)time_milli;
  rf_frame frame;
  frame.id = TIME_MILLI_ID;
  frame.data_length = 4;
  frame.data[0] = converter.data[3];
  frame.data[1] = converter.data[2];
  frame.data[2] = converter.data[1];
  frame.data[3] = converter.data[0];
  send_frame(frame);
}
// Sends test data.
// Creates a frame and populates it with some test data.
void send_random_data() {
  rf_frame frame;
  frame.id = RANDOM_FRAME0_ID;
  frame.data_length = 15;
  frame.data[0] = 0x01;
  frame.data[1] = 0x02;
  frame.data[2] = 0x03;
  frame.data[3] = 0x04;
  frame.data[4] = 0x05;
  frame.data[5] = 0x06;
  frame.data[6] = 0x07;
  frame.data[7] = 0x08;
  frame.data[8] = 0x09;
  frame.data[9] = 0x0a;
  frame.data[10] = 0x0b;
  frame.data[11] = 0x0c;
  frame.data[12] = 0x0d;
  frame.data[13] = 0x0e;
  frame.data[14] = 0x0f;
  send_frame(frame);
}
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}
// the loop function runs over and over again forever
// Loops every 20ms or so.
void loop() {
  send_temp_data();
  send_random_data();
  send_time_data();
}
