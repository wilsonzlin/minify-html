#!/usr/bin/env bash

set -e
shopt -s nullglob

for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  echo performance | sudo tee /sys/devices/system/cpu/cpu"$i"/cpufreq/scaling_governor
done

node sizes.js
sudo nice -n -20 taskset -c 1 node speeds.js
sudo chown -R "$USER:$USER" results
node graph.js
