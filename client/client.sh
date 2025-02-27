#!/bin/bash

# Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
# All rights reserved
#
# SPDX-License-Identifier: MIT

venv_path=.venv

# --------------------------------------------------------
# parameter area ;)
# --------------------------------------------------------

# --------------------------------------------------------

if [ ! -d "${venv_path}" ]; then
    echo "error: venv ${venv_path} can't be found"
    exit
fi

. $venv_path/bin/activate && python3 client_gui.py

deactivate
