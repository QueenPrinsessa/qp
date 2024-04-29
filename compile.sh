sh gen_make.sh

config=debug
platform=linux
renderer=vulkan
dev=0

while getopts c:p:d:r:h flag
do
	case "${flag}" in
		c) config=${OPTARG};;
		p) platform=${OPTARG};;
		d) dev=1;;
		r) renderer=${OPTARG};;
		h) renderer=headless;;
	esac
done

if [ "$dev" -eq 0 ]
then
	make config="${config}_${platform}_${renderer}_x64"
else
	make config="${config}_${platform}_${renderer}_dev_x64"
fi
