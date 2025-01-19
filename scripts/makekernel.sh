# git clone https://github.com/torvalds/linux ./kernel --depth=1
cd kernel
make defconfig
make -j$(nproc)
cd ..
cp kernel/arch/x86/boot/bzImage image/bzImage