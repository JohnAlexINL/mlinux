cd kernel
# make defconfig
make -j$(nproc)
cd ..
cp kernel/arch/x86/boot/bzImage image/bzImage