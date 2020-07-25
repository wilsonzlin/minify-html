#!/usr/bin/env bash

set -e
shopt -s nullglob

for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  echo performance | sudo dd status=none of="$i"
done

node sizes.js
# We need sudo to use `nice` but want to keep using current `node`, so use explicit path in case sudo decides to ignore PATH.
node_path="$(which node)"
echo "Using Node.js at $node_path"
sudo --preserve-env=HTML_ONLY nice -n -20 taskset -c 1 "$node_path" speeds.js
sudo chown -R "$USER:$USER" results
node graph.js
