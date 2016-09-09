clear
clear

as86 -o bs.o bs.s
bcc  -c -ansi bc.c
ld86 -d bs.o bc.o /usr/lib/bcc/libc.a

rm *.o
dd if=a.out of=vdisk bs=16 count=27 conv=notrunc
dd if=a.out of=vdisk bs=512 seek=1 conv=notrunc

qemu-system-i386 -hda vdisk
