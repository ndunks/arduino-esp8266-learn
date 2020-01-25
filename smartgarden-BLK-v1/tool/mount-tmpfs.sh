#/bin/bash
# Mount build and .pio
rm -rf ./.pio
rm -rf ./data
mkdir .pio data
sudo mount -t tmpfs tmpfs ./.pio
sudo mount -t tmpfs tmpfs ./data
