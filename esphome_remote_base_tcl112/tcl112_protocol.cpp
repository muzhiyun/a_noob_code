#include "tcl112_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TCL112_TAG = "remote.tcl112";


const uint16_t TCL112_STATE_LENGTH = 14;
const uint16_t TCL112_BITS = TCL112_STATE_LENGTH * 8;

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

static const uint16_t TCL112_FREQ = 36000;

void TCL112Protocol::encode(RemoteTransmitData *dst, const TCL112Data &data) {
  dst->reserve(TCL112_BITS);
  dst->set_carrier_frequency(TCL112_FREQ);

  // Encode header
  dst->item(TCL112_HEADER_MARK, TCL112_HEADER_SPACE);

  int32_t next{0};

  // Encode startbit+mode
  uint8_t header{static_cast<uint8_t>((1 << 3) | data.mode)};
  ESP_LOGVV(TAG, "TCL112Protocol::encode fail");

}

optional<TCL112Data> TCL112Protocol::decode(RemoteReceiveData src) {
    TCL112Data data{
      .mode = 0,
      .toggle = 0,
      .address = 0,
      .command = 0,
  };


// Validate header
  if (!src.expect_item(TCL112_HEADER_MARK, TCL112_HEADER_SPACE)) {
    ESP_LOGVV(TAG, "Header fail");
   return {};
  }

  uint8_t remote_state[TCL112_STATE_LENGTH] = {0};
  // Read all bytes.
  for (int i = 0; i < TCL112_STATE_LENGTH; i++) {
    // Read bit
    for (int j = 0; j < 8; j++) {
      if (src.expect_item(TCL112_BIT_MARK, TCL112_ONE_SPACE)) {
        remote_state[i] |= 1 << j;
      } else if (!src.expect_item(TCL112_BIT_MARK, TCL112_ZERO_SPACE)) {
        ESP_LOGVV(TAG, "Byte %d bit %d fail", i, j);
        return {};
      }
    }
  }
  // Validate footer
  if (!src.expect_mark(TCL112_BIT_MARK)) {
    ESP_LOGVV(TAG, "Footer fail");
    return {};
  }

  uint8_t checksum = 0;
  // Calculate & set the checksum for the current internal state of the remote.
  // Stored the checksum value in the last byte.
  for (uint8_t checksum_byte = 0; checksum_byte < TCL112_STATE_LENGTH - 1; checksum_byte++)
    checksum += remote_state[checksum_byte];
  if (checksum != remote_state[TCL112_STATE_LENGTH - 1]) {
    ESP_LOGVV(TAG, "Checksum fail");
    return {};
  }

  ESP_LOGV(TAG, "Received: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X",
           remote_state[0], remote_state[1], remote_state[2], remote_state[3], remote_state[4], remote_state[5],
           remote_state[6], remote_state[7], remote_state[8], remote_state[9], remote_state[10], remote_state[11],
           remote_state[12], remote_state[13]);

}

void TCL112Protocol::dump(const TCL112Data &data) {
  ESP_LOGD(TCL112_TAG, "Received TCL112: mode=0x%X, address=0x%02X, command=0x%02X, toggle=0x%X", data.mode, data.address,
           data.command, data.toggle);
}

}  // namespace remote_base
}  // namespace esphome
