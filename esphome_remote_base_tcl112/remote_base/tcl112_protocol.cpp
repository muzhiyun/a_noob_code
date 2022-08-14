#include "tcl112_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TCL112_TAG = "remote.tcl112";

const uint8_t TCL112_HEAT = 1;
const uint8_t TCL112_DRY = 2;
const uint8_t TCL112_COOL = 3;
const uint8_t TCL112_FAN = 7;
const uint8_t TCL112_AUTO = 8;

const uint8_t TCL112_FAN_AUTO = 0;
const uint8_t TCL112_FAN_LOW = 2;
const uint8_t TCL112_FAN_MED = 3;
const uint8_t TCL112_FAN_HIGH = 5;

const uint8_t TCL112_VSWING_MASK = 0x38;
const uint8_t TCL112_POWER_MASK = 0x04;

const uint8_t TCL112_HALF_DEGREE = 0b00100000;

const uint16_t TCL112_HEADER_MARK = 3100;
const uint16_t TCL112_HEADER_SPACE = 1650;
const uint16_t TCL112_BIT_MARK = 500;
const uint16_t TCL112_ONE_SPACE = 1100;
const uint16_t TCL112_ZERO_SPACE = 350;
const uint32_t TCL112_GAP = TCL112_HEADER_SPACE;

static const uint16_t TCL112_FREQ = 38000;

void TCL112Protocol::encode(RemoteTransmitData *dst, const TCL112Data &data) {
  ESP_LOGD(TCL112_TAG, "TCL112:encode");
  ESP_LOGD(TCL112_TAG, "encode TCL112: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X",
          data.remote_state[0],  data.remote_state[1],  data.remote_state[2],  data.remote_state[3],  data.remote_state[4],  data.remote_state[5],
          data.remote_state[6],  data.remote_state[7],  data.remote_state[8],  data.remote_state[9],  data.remote_state[10],  data.remote_state[11],
          data.remote_state[12],  data.remote_state[13]);
  //dst->reserve(TCL112_BITS);
  dst->set_carrier_frequency(TCL112_FREQ);

  // Header
  dst->mark(TCL112_HEADER_MARK);
  dst->space(TCL112_HEADER_SPACE);

  // Data
  for (uint8_t i :  data.remote_state) {
    for (uint8_t j = 0; j < 8; j++) {
      dst->mark(TCL112_BIT_MARK);
      bool bit = i & (1 << j);
      dst->space(bit ? TCL112_ONE_SPACE : TCL112_ZERO_SPACE);
    }
  }
  // Footer
  dst->mark(TCL112_BIT_MARK);
  dst->space(TCL112_GAP);
  ESP_LOGD(TCL112_TAG, "TCL112Protocol::encode finish");

}

optional<TCL112Data> TCL112Protocol::decode(RemoteReceiveData src) {
    TCL112Data data{
      .mode = 0,
      .toggle = 0,
      .address = 0,
      .command = 0,
      .remote_state = {0},
  };
    
// Validate header
  if (!src.expect_item(TCL112_HEADER_MARK, TCL112_HEADER_SPACE)) {
    ESP_LOGVV(TCL112_TAG, "Header fail");
   return {};
  }
  ESP_LOGV(TCL112_TAG, "TCL112:decode %ld",src.size());

  // Read all bytes.
  for (int i = 0; i < TCL112_STATE_LENGTH; i++) {
    // Read bit
    for (int j = 0; j < 8; j++) {
      if (src.expect_item(TCL112_BIT_MARK, TCL112_ONE_SPACE)) {
        data.remote_state[i] |= 1 << j;
        ESP_LOGV(TCL112_TAG, "i=%02X Received: %02X",i,data.remote_state[i]);
      } else if (!src.expect_item(TCL112_BIT_MARK, TCL112_ZERO_SPACE)) {
        ESP_LOGD(TCL112_TAG, "Byte %d bit %d fail", i, j);
        return {};
      }
    }
  }
  // Validate footer
  if (!src.expect_mark(TCL112_BIT_MARK)) {
    ESP_LOGD(TCL112_TAG, "Footer fail");
    return {};
  }

  uint8_t checksum = 0;
  // Calculate & set the checksum for the current internal state of the remote.
  // Stored the checksum value in the last byte.
  for (uint8_t checksum_byte = 0; checksum_byte < TCL112_STATE_LENGTH - 1; checksum_byte++)
    checksum +=  data.remote_state[checksum_byte];
  if (checksum !=  data.remote_state[TCL112_STATE_LENGTH - 1]) {
    ESP_LOGD(TCL112_TAG, "Checksum fail");
    return {};
  
  }

  ESP_LOGV(TCL112_TAG, "decode Received: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X",
            data.remote_state[0],  data.remote_state[1],  data.remote_state[2],  data.remote_state[3],  data.remote_state[4],  data.remote_state[5],
            data.remote_state[6],  data.remote_state[7],  data.remote_state[8],  data.remote_state[9],  data.remote_state[10],  data.remote_state[11],
            data.remote_state[12],  data.remote_state[13]);
   return data;

}

void TCL112Protocol::dump(const TCL112Data &data) {
  ESP_LOGD(TCL112_TAG, "Received TCL112: 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X",
            data.remote_state[0],  data.remote_state[1],  data.remote_state[2],  data.remote_state[3],  data.remote_state[4],  data.remote_state[5],
            data.remote_state[6],  data.remote_state[7],  data.remote_state[8],  data.remote_state[9],  data.remote_state[10],  data.remote_state[11],
            data.remote_state[12],  data.remote_state[13]);
}

}  // namespace remote_base
}  // namespace esphome
