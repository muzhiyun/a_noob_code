#pragma once

#include "remote_base.h"

namespace esphome {
namespace remote_base {


// Temperature
const float TCL112_TEMP_MAX = 31.0;
const float TCL112_TEMP_MIN = 16.0;




struct TCL112Data {
  uint8_t mode : 3;
  uint8_t toggle : 1;
  uint8_t address;
  uint8_t command;

  bool operator==(const TCL112Data &rhs) const { return address == rhs.address && command == rhs.command; }
};

class TCL112Protocol : public RemoteProtocol<TCL112Data> {
 public:
  void encode(RemoteTransmitData *dst, const TCL112Data &data) override;
  optional<TCL112Data> decode(RemoteReceiveData src) override;
  void dump(const TCL112Data &data) override;
};

DECLARE_REMOTE_PROTOCOL(TCL112)

template<typename... Ts> class TCL112Action : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint8_t, address)
  TEMPLATABLE_VALUE(uint8_t, command)

  void encode(RemoteTransmitData *dst, Ts... x) {
    TCL112Data data{};
    data.mode = 0;
    data.toggle = this->toggle_;
    data.address = this->address_.value(x...);
    data.command = this->command_.value(x...);
    TCL112Protocol().encode(dst, data);
    this->toggle_ = !this->toggle_;
  }

 protected:
  uint8_t toggle_{0};
};

}  // namespace remote_base
}  // namespace esphome
