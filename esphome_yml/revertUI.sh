#fro revert https://github.com/esphome/esphome/commit/af66753c1b49f230e57685d44ae2ef2eb7ddc60e

# https://github.com/esphome/esphome/commit/9ad9d64ac7ac2414a0539374b346b60165362178


git clone https://github.com/esphome/esphome/ -t v1.14.5



[rdp_proxy_udp_oa]
type = udp
local_ip = 192.168.2.149
local_port = 3389
remote_port = 6002

[rdp_proxy_tcp]
type = tcp
local_ip = 192.168.2.155
local_port = 3389
remote_port = 6001