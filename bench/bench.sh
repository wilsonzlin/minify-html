#!/usr/bin/env bash

set -e
shopt -s nullglob

for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  echo performance | sudo dd status=none of="$i"
done

results_dir="$PWD/results"
input_dir="$PWD/inputs"
iterations=100

pushd runners
for r in *; do
  pushd "$r"
  ./build
  sudo --preserve-env=MHB_HTML_ONLY,PATH MHB_ITERATIONS=$iterations MHB_INPUT_DIR="$input_dir" nice -n -20 taskset -c 1 ./run | tee "$results_dir/$r.json"
  popd
done
