#!/bin/bash

# SPDX-License-Identifier: MIT
# SPDX-Author: Roman Koch <koch.romam@gmail.com>
# SPDX-Copyright: 2024 Roman Koch <koch.romam@gmail.com>

TARGET_DIR="../bbb-sdk/"

# Suche nach der passenden SDK-Installationsdatei
ENV_FILE=$(find "$TARGET_DIR" -type f -name "environment-setup-*" | head -n 1)

if [ -z "$ENV_FILE" ]; then
    echo "Fehler: Keine passende ENV-Datei gefunden."
    exit 1
fi

source "${ENV_FILE}"
