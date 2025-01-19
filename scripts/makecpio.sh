mkdir -p image/initrd/bin
cp -r bin image/initrd
mv image/initrd/bin/init image/initrd/init
cd image/initrd
find . | cpio -o -H newc > ../init.cpio