mkdir -p image/initrd/bin
# cp bin image/initrd/bin -r
# echo -e '#!/bin/sh\nexec /bin/bash' > image/initrd/init
# chmod +x image/initrd/init
cp bin/shell image/initrd/init
cd image/initrd
find . | cpio -o -H newc > ../init.cpio