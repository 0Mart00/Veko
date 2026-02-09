#!/bin/bash

echo "Watcher started: monitoring modules/logic.c"

while inotifywait -e modify modules/logic.c; do
    echo "Change detected, recompiling..."
    make module
    if [ $? -eq 0 ]; then
        echo "Recompile successful."
    else
        echo "Recompile failed! Check your C syntax."
    fi
done