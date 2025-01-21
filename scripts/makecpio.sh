cp image/root/bin/init image/initrd/init
#cp image/root/bin/shell image/initrd/bin/shell
cp image/root/bin/ image/initrd -r
cd image/initrd
find . | cpio -o -H newc > ../init.cpio