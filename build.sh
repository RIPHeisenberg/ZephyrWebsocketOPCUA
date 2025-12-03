#!/bin/bash

west build -b native_sim/native/64 -- -DEXTRA_CONF_FILE=overlay-nsos.conf
