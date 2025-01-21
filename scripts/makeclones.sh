# cp /usr/lib/grub/x86_64-efi/* system/grub
git clone https://github.com/torvalds/linux ./kernel --depth=1
git clone https://github.com/Modula-dev/nex.git --depth=1 src/nex
wget -O ./image/root/bin/busybox https://github.com/shutingrz/busybox-static-binaries-fat/raw/refs/heads/main/busybox-x86_64-linux-gnu