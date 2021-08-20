if test "x$1" = x || test "x$2" = x; then
	echo "Usage: mkinitrd.sh <out file prefix> <directory to copy into image>"
	echo "example: ./mkinitrd.sh foo 1024 bar"
	echo "         Creates a compressed initial ramdisk 'foo.gz'."
	echo "         The content of directory 'bar' is transfered into the image before compression"
	exit
fi
size=$(((`sudo du $2 -sb | cut -f 1` + 524288) / 1024))
echo $size
dd if=/dev/zero of=$1 count=$size bs=1024
sudo losetup /dev/loop0 $1
sudo mke2fs -m 0 /dev/loop0
mkdir -p ~/mnt || true
sudo mount /dev/loop0 ~/mnt
sudo cp -af $2/* ~/mnt/
sudo umount ~/mnt
sudo losetup -d /dev/loop0
rm -f $1.gz
gzip $1

