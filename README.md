3.4.55-rtai
===========

Sources and instructions to rebuild the debian.hybrid rtai kernel for Linuxcnc 2.6

The 3.4-9-rtai.686.pae kernel in the debian.hybrid.iso distro of Linuxcnc is based upon the 3.4.55 kernel and
an old version of the NTULINUX / ShabbyX RTAI repo

Commands assume the use of the root shell, which is the easiest way to build the kernel and RTAI

If you want to build Linuxcnc locally, then add your user to /etc/sudoers or the group sudo and proceed that way

The Linuxcnc build is a RIP build, which still leaves the original build of Linuxcnc able to be run as a system install
with the swapping of realtime directories
(eg rename /usr/realtime to /usr/realtime-rip and /usr/realtime-old to /usr/realtime then boot the original 3.4-9-rtai-686-pae kernel))

Prerequisites
*************

build-essential debhelper kernel-package libpth-dev libgtk2.0-dev tcl8.5-dev tk8.5-dev bwidget python-old-doctools python-tk python-dev libglu1-mesa-dev libgtk2.0-dev 
libgnomeprintui2.2-dev libncurses5-dev libxaw7-dev gettext libreadline-gplv2-dev lyx texlive-extra-utils imagemagick texinfo groff libmodbus-dev
libudev-dev libmodbus-dev libboost-python-dev libboost-serialization-dev libboost-thread-dev libtk-img automake autoconf libtool libusb-dev automake1.11 libtool python-setuptools

Initial setup
*************

From the cloned git directory:-

Copy linux-3.4.55 to /usr/src/linux-3.4.55

Copy symlink ~linux to /usr/src/linux

Copy RTAI to /usr/src/linux/RTAI

Copy .config to /usr/src/linux/.config

Copy hal-linux-3.4.55-x86-1.patch to /usr/src/hal-linux-3.4.55-x86-1.patch

Building
********

$ cd /usr/src/linux

$ patch -p1 < ../hal-linux-3.4.55-x86-1.patch

$ make menuconfig

        Now set local version suffix to -rtai and processor type and number of cores to match your machine

$ make -jN && make modules_install && make install

        where N is the number of cores on your machine + 1
        
'make install' will activate update-grub command
This often produces rubbish that does not work, I always add new kernels by hand, so have an entry that reads similar to this with disc numbers and UUIDs amended to suit in /boot/grub/grub.cfg

menuentry "sda3 3.4.55-rtai" --class gnu-linux --class gnu --class os {

    insmod part_msdos
    insmod ext2
    set root='(hd0,msdos3)'
    search --no-floppy --fs-uuid --set=root 135dd1b1-0c8b-46f5-9748-94eb75c21eae
    linux /boot/vmlinuz-3.4.55-rtai root=UUID=135dd1b1-0c8b-46f5-9748-94eb75c21eae
    initrd /boot/initrd.img-3.4.55-rtai
    
}


Reboot and select kernel 3.4.55-rtai

Rename the existing realtime dir

$ mv /usr/realtime /usr/realtime-old

$ cd /usr/src/RTAI && ./autogen.sh && ./configure && make menuconfig

        Now select correct number of cores and enable C99 support

$ make && make install

Fetch the latest version of the master

$ cd /usr/src
$ git clone git://git.linuxcnc.org/git/linuxcnc.git linuxcnc-dev

Build it

$ cd /usr/src/linuxcnc-dev/src && ./autogen.sh && ./configure && make && make setuid && cd ../ && . ./scripts/rip-enviroment

Then to run from that shell session 

$ linuxcnc


