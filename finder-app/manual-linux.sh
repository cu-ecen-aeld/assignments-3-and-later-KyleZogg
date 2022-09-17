#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "\n\n### Checking out version ${KERNEL_VERSION}\n\n"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    #we are inside linux-stable, Should be DONE

    #Step1: do a deep clean with this command from the lecture notes
    echo "about to do deep clean"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper

    #Step2: do the defconfig build
    #will configure for our "virt" arm dev board we will simulate in QEMU
    echo "about to build devconfig"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig

    #now build the vmlinux target, the "-j4" will allow using up to 4 cores for the compile for speed
    echo "about to build vmlinux"
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all

    #build the modules
    #maybe supposed to skip this step for assignment 3.2
    echo "about to build modules"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules

    #build the device tree
    echo "about to build device tree"
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
    #end Kyle Edits
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
#This should be DONE
echo ""
echo ""
echo "Start Creating rootfs"
echo ""
echo ""

#create the base directory rootfs
cd "$OUTDIR"
mkdir rootfs
cd rootfs

#create the rest of the directories
mkdir bin dev etc home home/conf lib proc sbin sys tmp
mkdir usr
cd usr
mkdir bin lib sbin
cd ..
mkdir va
mkdir var
cd var
mkdir log
cd ..

#make all the contents owned by root
cd "$OUTDIR"
cd rootfs
sudo chown -R root:root *
#end Kyle Edits

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    #this should be DONE
    #these lines from lecture vid "linux root filesystem 9:57"
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
echo "make and install busybox"
#origional line right below this
#sudo make ARCH=arm64 CROSS_COMPILE=arm-unknown-linux-gnueabi- install 
#sudo make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- install
#sudo screws with the path so you need to add the full path to the compiler here (THIS WORKED)
sudo make ARCH=arm64 CROSS_COMPILE=/home/kyle/Documents/linux_1/install-lnx/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu- install

#cp the binary to $OUTDIR/rootfs/bin
sudo cp busybox $OUTDIR/rootfs/bin

echo "Library dependencies"

cd $OUTDIR/rootfs
echo $PWD
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"



# TODO: Add library dependencies to rootfs
#this should be DONE
#this is at 11:10 in linux root filesystem
echo "this is the print-sysroot line"
${CROSS_COMPILE}gcc -print-sysroot

echo "this is the cd SYSROOT line"
SYSROOT=/home/kyle/Documents/linux_1/install-lnx/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin/../aarch64-none-linux-gnu/libc
echo $SYSROOT
cd $SYSROOT
ls -l lib/ld-linux-aarch64.so.1

#actually copy in the library dependencies to rootfs here
cd $OUTDIR/rootfs
#get these lines from the two grep commands above
sudo cp -a $SYSROOT/lib/ld-linux-aarch64.so.1 lib

sudo cp -a $SYSROOT/lib64/ld-2.31.so lib
sudo cp -a $SYSROOT/lib64/libc.so.6 lib
sudo cp -a $SYSROOT/lib64/libc-2.31.so lib
sudo cp -a $SYSROOT/lib64/libm.so.6 lib
sudo cp -a $SYSROOT/lib64/libm-2.31.so lib
sudo cp -a $SYSROOT/lib64/libresolv.so.2 lib
sudo cp -a $SYSROOT/lib64/libresolv-2.31.so lib

# TODO: Make device nodes
#stay in $OUTDIR/rootfs
echo "make device nodes"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

#THIS IS WHERE IN THE VID IT SAID TO INSTALL THE MODULES BUT WE WON"T DO THAT HERE
#BUT THIS IS HOW IT WOULD BE DONE
#make -j4 ARCH=arm CROSS_COMPILE=aarch64-none-linux-gnu- INSTALL_MOD_PATH={$OUTDIR}/rootfs modules_install

# TODO: Clean and build the writer utility

    #cd to the location of the utility  
echo "clean and make the writer utility"
cd $FINDER_APP_DIR
make clean 
make CROSS_COMPILE

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
sudo cp writer $OUTDIR/rootfs/home
sudo cp finder.sh $OUTDIR/rootfs/home
sudo cp conf/username.txt $OUTDIR/rootfs/home/conf
sudo cp finder-test.sh $OUTDIR/rootfs/home
sudo cp autorun-qemu.sh $OUTDIR/rootfs/home

# TODO: Chown the root directory
cd $OUTDIR/rootfs
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
cd $OUTDIR/rootfs
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd ..
gzip initramfs.cpio
#mkimage -A arm64 -O linux -T ramdisk -d initramfs.cpio.gz Image

#Booting the kernel
#Wrong image type 3, expected 2

