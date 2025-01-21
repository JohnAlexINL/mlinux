cp image/init.cpio image/root/boot/init.cpio
cp configs/grub.cfg image/root/boot/grub/grub.cfg
grub-mkrescue -o image/boot.iso image/root