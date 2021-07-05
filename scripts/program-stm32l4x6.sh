#!/bin/bash

openocd -f interface/stlink.cfg -f target/stm32l4x.cfg -c "reset_config srst_only srst_nogate connect_assert_srst"  -c"program $1 0x08000000 verify reset exit"

exit 0
