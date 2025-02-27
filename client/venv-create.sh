#!/bin/bash

# Copyright (c) 2023, Roman Koch, koch.roman@gmail.com
# All rights reserved
#
# SPDX-License-Identifier: MIT

if ! command -v python3 >/dev/null 2>&1; then
    echo "python3 could not be found"
    exit
fi

venv_path=.venv

if [ -d "${venv_path}" ]; then
    echo "warning: venv ${venv_path} exists"
else
    python3 -m venv $venv_path
    if [ $? -ne 0 ]; then
        echo "error: can't create venv ${venv_path}"
    fi
fi

if [ $? -ne 0 ]; then
    echo "error: can't create venv"
else
    . $venv_path/bin/activate && python3 -m pip install --upgrade pip && python3 -m pip install -r requirements.txt && deactivate
fi
