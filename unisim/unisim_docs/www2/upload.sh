#!/bin/bash
HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)

FORCE=n
PUBLIC=n
while (( "$#" )); do
	case "$1" in
		--force)
			FORCE=y
			;;
		public)
			PUBLIC=y
			;;
		private)
			PUBLIC=n
			;;
	esac
	shift
done

mkdir -p ${HERE}/site
mkdir -p ${HERE}/old

echo "Cleaning"
"${MY_DIR}/build.sh" clean || exit -1

echo "Starting building web site..."
if [ "${PUBLIC}" = "y" ]; then
	"${MY_DIR}/build.sh" all public || exit -1
else
	"${MY_DIR}/build.sh" all private || exit -1
fi

if [ ${FORCE} = "y" ]; then
	echo "Starting full upload..."
	rm -rf old/*
	rm -rf remote/video_thumbs
	rm -rf remote/style
	rm -rf remote/images_thumbs
	rm -rf remote/glyphes
	rm -rf remote/bare
	rm -f remote/*.html
	CHANGED=y
	PASS=2
else
	echo "Starting incremental upload..."
	CHANGED=n
	PASS=1
fi

OLD_FILE_LIST=$(cd ${HERE}/old; find . -name "*")
FILE_LIST=$(cd ${HERE}/site; find . -name "*")

until [ ${PASS} -eq 3 ]; do
	if [ ${PASS} -eq 2 ]; then
		if [ ${CHANGED} = "n" ]; then
			echo "Web site is already up to date..."
			break;
		fi

		echo "Mounting sshfs..."
		if [ ${PUBLIC} = "y" ]; then
			SSH_URL=unisimvp@unisim-vp.org:www/site
			echo "Upload to public site is forbidden for now"
			exit -1
		else
			SSH_URL=unisimvp@unisim-vp.org:www/private
		fi
		sshfs ${SSH_URL} "${HERE}/remote"
		STATUS=$?
		if [ ${STATUS} -eq 0 ]; then
			echo "Remote directory successfully mounted\n"
		else
			echo "Mounting remote directory failed\n"
			exit -1
		fi
	fi

	for FILE in ${OLD_FILE_LIST}; do
		if [ -f ${HERE}/old/${FILE} ]; then
			if [ ! -f ${HERE}/site/${FILE} ]; then
				echo "File \"${FILE}\" removed"
				CHANGED=y
				DIRNAME=$(dirname ${FILE})
				if [ ! -d ${HERE}/site/${DIRNAME} ]; then
					echo "Directory ${DIRNAME} removed"
					RMDIR=y
				else
					RMDIR=n
				fi

				if [ ${PASS} -eq 2 ]; then
					echo "Removing ${FILE}..."
					rm -f ${HERE}/remote/${FILE}
					if [ $? -eq 0 ]; then
						rm -f ${HERE}/old/${FILE}
					fi
				fi

				if [ ${PASS} -eq 2 ] && [ ${RMDIR} = "y" ]; then
					echo "Removing directory ${DIRNAME}..."
					rmdir ${HERE}/remote/${DIRNAME}
					if [ $? -eq 0 ]; then
						rmdir ${HERE}/old/${DIRNAME}
					fi
				fi
			fi
		fi
	done

	for FILE in ${FILE_LIST}; do
		if [ -f ${HERE}/site/${FILE} ]; then
			DIRNAME=`dirname ${FILE}`

			if ! [ -d ${HERE}/old/${DIRNAME} ]; then
				CHANGED=y
				MKDIR=y
				echo "Directory \"${DIRNAME}\" created"
			else
				MKDIR=n
			fi

			if [ ${PASS} -eq 2 ] && [ ${MKDIR} = "y" ]; then
				echo "Creating \"${DIRNAME}\"..."
				mkdir -p ${HERE}/remote/${DIRNAME}
				mkdir -p ${HERE}/old/${DIRNAME}
			fi

			if [ -f ${HERE}/old/${FILE} ]; then
				diff ${HERE}/old/${FILE} ${HERE}/site/${FILE} &> /dev/null
				DIFF=$?
				if ! [ ${DIFF} -eq 0 ]; then
					CHANGED=y
					COPY=y
					echo "File \"${FILE}\" changed"
				else
					COPY=n
				fi
			else
				CHANGED=y
				COPY=y
				echo "New file \"${FILE}\" detected"
			fi

			if [ ${PASS} -eq 2 ] && [ ${COPY} = "y" ]; then
				echo "Uploading ${FILE}..."
				cp -f ${HERE}/site/${FILE} ${HERE}/remote/${FILE}
				if [ $? -eq 0 ]; then
					cp -f ${HERE}/site/${FILE} ${HERE}/old/${FILE}
				fi
			fi
		fi
	done

	PASS=$((${PASS} + 1))
done

if [ "${CHANGED}" = "y" ]; then
	printf "umounting sshfs..."
	STATUS=1
	until [ ${STATUS} -eq 0 ]; do
		sleep 1
		fusermount -u "${HERE}/remote" &> /dev/null
		STATUS=$?
		printf "."
	done
	printf "done\n"
fi

echo "Cleaning"
"${MY_DIR}/build.sh" clean || exit -1
echo "Congratulation. Bye !"
