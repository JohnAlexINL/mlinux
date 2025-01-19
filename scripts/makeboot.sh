# make the filesystem
mkdir -p image/root
dd if=/dev/zero of=image/boot bs=1M count=512
sudo mkfs -t fat image/boot
syslinux --install image/boot

# mount the new filesystem
sudo mount image/boot image/root

# clone needed files into the root filesystem
sudo mkdir -p image/root/boot
sudo mkdir -p image/root/bin
sudo cp image/bzImage image/root/boot/bzImage
sudo cp image/init.cpio image/root/boot/init.cpio
sudo cp -r bin image/root/bin

# unmount it when we're finished populating it
sudo umount image/root