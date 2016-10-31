VFD=mtximage

echo compiling .....
bcc -c -ansi t.c
as86 -o ts.o ts.s

echo linking .......
ld86 -o mtx -d ts.o t.o mtxlib /usr/lib/bcc/libc.a

mount -o loop $VFD /mnt
cp mtx /mnt/boot
umount /mnt

(cd USER; mku u1; mku u2)

rm *.o 
echo done

