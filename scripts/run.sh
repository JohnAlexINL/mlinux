# qemu-system-x86_64 -kernel image/bzImage -initrd image/init.cpio -append "root=/dev/ram0 init=/init" -m 512M
qemu-system-x86_64 image/boot.iso