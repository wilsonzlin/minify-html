#!/usr/bin/env bash

set -e
shopt -s nullglob

for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  echo performance | sudo dd status=none of="$i"
done

rm -f perf.data
sudo perf record -g nice -n -20 taskset -c 1 minify-html-bench/target/release/minify-html-bench --tests tests --iterations 512
sudo chown "$USER:$USER" perf.data
