#!/bin/bash
LAST_MOD=$(stat -c %Y modules/logic.c)

echo "Polling watcher started: monitoring modules/logic.c"

while true; do
    CURRENT_MOD=$(stat -c %Y modules/logic.c)
    if [ "$LAST_MOD" != "$CURRENT_MOD" ]; then
        echo "Change detected, recompiling..."
        make module
        LAST_MOD=$CURRENT_MOD
    fi
    sleep 1
done