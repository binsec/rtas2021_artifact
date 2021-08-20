#!/bin/bash
HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)

PUBLIC=n
CMD=
DEFAULT_THEME=
while (( "$#" )); do
	case "$1" in
		public)
			PUBLIC=y
			;;
		private)
			PUBLIC=n
			;;
		all | clean | help)
			CMD="$1"
			;;
		*)
			DEFAULT_THEME="$1"
			;;
	esac
	shift
done

function usage
{
	echo "Usage: `basename $0` <all | clean | help> [default theme]"
	THEMES=$(cd ${MY_DIR}/themes; ls)
	echo "theme can be one of the followings:"
	echo "${THEMES}"
}

IMAGES_DIR="${MY_DIR}/images"
VIDEOS_DIR="${MY_DIR}/videos"
DOWNLOADS_DIR="${MY_DIR}/downloads"
HOME_NEWS="${MY_DIR}/content/index/news.html"
ALL_NEWS="${MY_DIR}/content/news/news.html"
WHATS_NEW="${MY_DIR}/template/whats_new.html"

if [ -z "${DEFAULT_THEME}" ]; then
	DEFAULT_THEME=default
fi

case "${CMD}" in
	help)
		usage
		exit 0
		;;
	all)
		echo "Using theme ${DEFAULT_THEME} as default theme"

		# create web site directory structure
		mkdir -p ${HERE}/site
		mkdir -p ${HERE}/site/style
		mkdir -p ${HERE}/site/images
		mkdir -p ${HERE}/site/videos
		mkdir -p ${HERE}/site/images_thumbs
		mkdir -p ${HERE}/site/videos_thumbs
		mkdir -p ${HERE}/site/glyphes
		mkdir -p ${HERE}/site/downloads

		# list all news materials
		NEWS_LIST=`cd ${MY_DIR}/news; ls -r *.html 2> /dev/null`

		# list all image materials
		IMAGES=`cd ${IMAGES_DIR}; ls *.png *.jpg 2> /dev/null`

		# list all video materials
		VIDEOS=`cd ${VIDEOS_DIR}; ls *.avi 2> /dev/null`

		# list all glyph materials
		GLYPHES=`cd ${MY_DIR}/glyphes; ls *.png *.ico *.jpg 2> /dev/null`

		###############################################################################
		#                                     NEWS                                    #
		###############################################################################

		if [ -n "${NEWS_LIST}" ]; then
			HAVE_NEWS=1
			MAX_HOME_NEWS=10

			# define generated files
			mkdir -p "${MY_DIR}/content/news"
			rm -f "${HOME_NEWS}"
			rm -f "${ALL_NEWS}"
			rm -f "${WHATS_NEW}"

			echo "News" > "${MY_DIR}/content/news/title.txt"

			cat << EOF > "${MY_DIR}/content/news/content.html"
<div class="content-item">
	<div class="content-item-title"><h2>News</h2></div>
	<div class="content-item-body">
		<div class="news-list">
#include "news.html"
		</div>
	</div>
</div>
EOF

			declare -a MONTH=("January" "February" "March" "April" "May" "June" "July" "August" "September" "October" "November" "December")

			echo "Building ${ALL_NEWS} and ${WHATS_NEW}..."

			printf "" > "${ALL_NEWS}"

			NUM_NEWS=0
			for NEWS in ${NEWS_LIST}; do
				printf '\t<tr>\n' >> ${ALL_NEWS}
				printf '\t\t<div class="news-date">\n' >> ${ALL_NEWS}
				YEAR=`printf ${NEWS} | cut -f 1 -d -`
				MONTH_NUMBER=`printf ${NEWS} | cut -f 2 -d - | sed 's/0*//'`
				MONTH_NUMBER=$((${MONTH_NUMBER} - 1))
				MONTH_NAME=${MONTH[${MONTH_NUMBER}]}
				DAY_NUMBER=`printf ${NEWS} | cut -f 3 -d - | cut -f 1 -d .`
				printf "\t\t\t${MONTH_NAME} ${DAY_NUMBER}, ${YEAR}\n" >> ${ALL_NEWS}
				printf "\t\t</div>\n" >> ${ALL_NEWS}
				printf "\t\t<div class=\"news-description\">\n" >> ${ALL_NEWS}
				printf "\t\t\t" >> ${ALL_NEWS}
				cat ${MY_DIR}/news/${NEWS} >> ${ALL_NEWS}
				printf "\n\t\t</div>\n" >> ${ALL_NEWS}
				printf "\t<div class=\"news-separator\"></div>\n" >> ${ALL_NEWS}
				if ! [ -f ${WHATS_NEW} ]; then
					cp ${MY_DIR}/news/${NEWS} ${WHATS_NEW}
				fi
				NUM_NEWS=$((${NUM_NEWS} + 1))
				if [ ${NUM_NEWS} -eq ${MAX_HOME_NEWS} ]; then
					cp ${ALL_NEWS} ${HOME_NEWS}
				fi
			done

			if [ ${NUM_NEWS} -lt ${MAX_HOME_NEWS} ]; then
				cp ${ALL_NEWS} ${HOME_NEWS}
			fi
		else
			HAVE_NEWS=0
		fi

		###############################################################################
		#                                    GLYPHES                                  #
		###############################################################################

		for GLYPH in ${GLYPHES}; do
			if [ -f "${MY_DIR}/glyphes/${GLYPH}" ]; then
				echo "Copying glyphes/${GLYPH}"
				cp "${MY_DIR}/glyphes/${GLYPH}" "${HERE}/site/glyphes/${GLYPH}"
			fi
		done

		###############################################################################
		#                                   IMAGES                                    #
		###############################################################################

		for IMAGE in ${IMAGES}; do
			if [ -f "${IMAGES_DIR}/${IMAGE}" ]; then
				echo "Copying images/${IMAGE}"
				cp "${IMAGES_DIR}/${IMAGE}" "${HERE}/site/images/${IMAGE}"
			fi
		done

		###############################################################################
		#                                  VIDEOS                                     #
		###############################################################################

		for VIDEO in ${VIDEOS}; do
			if [ -f "${VIDEOS_DIR}/${VIDEO}" ]; then
				echo "Copying videos/${VIDEO}"
				cp "${VIDEOS_DIR}/${VIDEO}" "${HERE}/site/videos/${VIDEO}"
			fi
		done

		###############################################################################
		#                                IMAGE THUMBS                                 #
		###############################################################################

		for IMAGE in ${IMAGES}; do
			if [ -f "${IMAGES_DIR}/${IMAGE}" ]; then
				echo "Generating thumbnail for image ${IMAGE}..."
				convert -thumbnail 256 "${IMAGES_DIR}/${IMAGE}" "${HERE}/site/images_thumbs/${IMAGE}"
			fi
		done

		###############################################################################
		#                                VIDEO THUMBS                                 #
		###############################################################################

		for VIDEO in ${VIDEOS}; do
			if [ -f "${VIDEOS_DIR}/${VIDEO}" ]; then
				echo "Generating thumbnail for video ${VIDEO}..."
				VIDEO_NAME=`echo ${VIDEO} | sed -e 's/\..*$//g'`
				mplayer -frames 1 -vo png:z=5 -zoom -xy 256 "${VIDEOS_DIR}/${VIDEO}" &> /dev/null
				composite -gravity center "${MY_DIR}/glyphes/play.png" 00000001.png "${HERE}/site/videos_thumbs/${VIDEO_NAME}.png"
				rm -f 00000001.png
				#mv 00000001.png "${HERE}/site/videos_thumbs/${VIDEO_NAME}.png"
			fi
		done

		###############################################################################
		#                                 IMAGE GALLERY                               #
		###############################################################################

		if [ -n "${IMAGES}" ]; then
			HAVE_IMAGE_GALLERY=1
			IMAGE_GALLERY_NUM_IMAGES_PER_PAGE=15
			IMAGE_GALLERY_NUM_IMAGES_PER_ROW=3
			NUM_IMAGES=`echo "${IMAGES}" | wc -w`
			NUM_IMAGE_GALLERY_PAGES=$(((${NUM_IMAGES} + ${IMAGE_GALLERY_NUM_IMAGES_PER_PAGE} - 1) / ${IMAGE_GALLERY_NUM_IMAGES_PER_PAGE}))

			if [ ${NUM_IMAGE_GALLERY_PAGES} -le 7 ]; then
				STRING_PAGE="Page "
			elif [ ${NUM_IMAGE_GALLERY_PAGES} -le 15 ]; then
				STRING_PAGE="P. "
			else
				STRING_PAGE=""
			fi

			IMAGE_GALLERY_COL=1
			IMAGE_GALLERY_PAGE_NUM=1
			IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE=1
			for IMAGE in ${IMAGES}; do
				if [ -f "${IMAGES_DIR}/${IMAGE}" ]; then
					echo "Adding image ${IMAGE} to image gallery..."
					IMAGE_NAME=`echo ${IMAGE} | sed -e 's/\..*$//g'`

					if [ ${IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE} -eq 1 ]; then
						if [ ${NUM_IMAGE_GALLERY_PAGES} -gt 1 ]; then
							# build a navigation bar (in reverse order)
							PAGE_NUM=${NUM_IMAGE_GALLERY_PAGES}
							IMAGE_GALLERY_NAV="</td></tr></table>";
							if ! [ ${IMAGE_GALLERY_PAGE_NUM} -eq ${NUM_IMAGE_GALLERY_PAGES} ]; then
								NEXT_PAGE_NUM=$((${IMAGE_GALLERY_PAGE_NUM} + 1))
								IMAGE_GALLERY_NAV="<a href=\"image-gallery-${NEXT_PAGE_NUM}.html\"><img src=\`\`THEME/nav-next.png\`\` alt=\"&gt;\"></a>${IMAGE_GALLERY_NAV}"
							fi
							IMAGE_GALLERY_NAV="<td id=\"image-gallery-nav-next\">${IMAGE_GALLERY_NAV}"
							while [ ${PAGE_NUM} -gt 0 ]; do
								if [ ${PAGE_NUM} -eq ${IMAGE_GALLERY_PAGE_NUM} ]; then
									IMAGE_GALLERY_NAV="<td>${STRING_PAGE}${PAGE_NUM}</td>${IMAGE_GALLERY_NAV}"
								else
									IMAGE_GALLERY_NAV="<td><a href=\"image-gallery-${PAGE_NUM}.html\">${STRING_PAGE}${PAGE_NUM}</a></td>${IMAGE_GALLERY_NAV}"
								fi
								PAGE_NUM=$((${PAGE_NUM} - 1))
							done
							IMAGE_GALLERY_NAV="</td>${IMAGE_GALLERY_NAV}"
							if ! [ ${IMAGE_GALLERY_PAGE_NUM} -eq 1 ]; then
								PREV_PAGE_NUM=$((${IMAGE_GALLERY_PAGE_NUM} - 1))
								IMAGE_GALLERY_NAV="<a href=\"image-gallery-${PREV_PAGE_NUM}.html\"><img src=\`\`THEME/nav-prev.png\`\` alt=\"&lt;\"></a>${IMAGE_GALLERY_NAV}"
							fi
							IMAGE_GALLERY_NAV="<table class=\"image-gallery-nav\"><tr><td id=\"image-gallery-nav-prev\">${IMAGE_GALLERY_NAV}"
						else
							IMAGE_GALLERY_NAV=
						fi

						IMAGE_GALLERY_DIR=${MY_DIR}/content/image-gallery-${IMAGE_GALLERY_PAGE_NUM}
						rm -rf ${IMAGE_GALLERY_DIR}
						mkdir -p ${IMAGE_GALLERY_DIR}
						IMAGE_GALLERY=${IMAGE_GALLERY_DIR}/content.html
						IMAGE_GALLERY_STYLE=${IMAGE_GALLERY_DIR}/style.css
						echo "Generating image gallery in ${IMAGE_GALLERY}..."

						printf "Screenshot gallery" > ${IMAGE_GALLERY_DIR}/title.txt
						if [ ${NUM_IMAGE_GALLERY_PAGES} -gt 1 ]; then
							printf " - Page ${IMAGE_GALLERY_PAGE_NUM} of ${NUM_IMAGE_GALLERY_PAGES}" >> ${IMAGE_GALLERY_DIR}/title.txt
						fi

						printf "<div class=\"content-item\">\n" > ${IMAGE_GALLERY}
						printf "<div class=\"content-item-title\">\n" >> ${IMAGE_GALLERY}
						printf "<h2>\n" >> ${IMAGE_GALLERY}
						printf "#include \"title.txt\"\n" >> ${IMAGE_GALLERY}
						printf "</h2>\n" >> ${IMAGE_GALLERY}
						printf "</div>\n" >> ${IMAGE_GALLERY}
						printf "<div class=\"content-item-body\">" >> ${IMAGE_GALLERY}
						printf "${IMAGE_GALLERY_NAV}\n" >> ${IMAGE_GALLERY} 
						printf "<table class=\"image-gallery\">\n" >> ${IMAGE_GALLERY}
					fi

					if [ ${IMAGE_GALLERY_COL} -eq 1 ]; then
						printf "\t<tr>\n" >> ${IMAGE_GALLERY}
					fi
					printf "\t\t<td>\n" >> ${IMAGE_GALLERY} 
					printf "\t\t\t<a href=\"image-gallery-${IMAGE_GALLERY_PAGE_NUM}-view-image-${IMAGE_NAME}.html\"><img src=\`\`IMAGE_THUMBS/${IMAGE}\`\` alt=\"${IMAGE} thumbnail\"></a>\n" >> ${IMAGE_GALLERY}
					printf "\t\t\t<p>\n" >> ${IMAGE_GALLERY}
					if [ -f "${MY_DIR}/images/${IMAGE_NAME}.txt" ]; then
						cat "${MY_DIR}/images/${IMAGE_NAME}.txt" >> ${IMAGE_GALLERY}
					else
						printf "${IMAGE_NAME}" >> ${IMAGE_GALLERY}
					fi
					printf "\t\t\t</p>\n" >> ${IMAGE_GALLERY}
					printf "\t\t</td>\n" >> ${IMAGE_GALLERY} 
					IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE=$((${IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE} + 1))
					if [ ${IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE} -gt ${IMAGE_GALLERY_NUM_IMAGES_PER_PAGE} ]; then
						IMAGE_GALLERY_COL=1
						IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE=1
						IMAGE_GALLERY_PAGE_NUM=$((${IMAGE_GALLERY_PAGE_NUM} + 1))
						printf "\t</tr>\n" >> ${IMAGE_GALLERY} 
						printf "</table>\n" >> ${IMAGE_GALLERY} 
						printf "</div>\n" >> ${IMAGE_GALLERY} 
						printf "</div>\n" >> ${IMAGE_GALLERY} 
						continue
					fi

					IMAGE_GALLERY_COL=$((${IMAGE_GALLERY_COL} + 1))
					if [ ${IMAGE_GALLERY_COL} -gt ${IMAGE_GALLERY_NUM_IMAGES_PER_ROW} ]; then
						IMAGE_GALLERY_COL=1
						printf "\t</tr>\n" >> ${IMAGE_GALLERY}
						continue
					fi
				fi
			done

			while [ ${IMAGE_GALLERY_COL} -le ${IMAGE_GALLERY_NUM_IMAGES_PER_ROW} ]; do
				printf "<td></td>" >> ${IMAGE_GALLERY}
				IMAGE_GALLERY_COL=$((${IMAGE_GALLERY_COL} + 1))
			done

			if ! [ ${IMAGE_GALLERY_COL} -eq 1 ]; then
				if [ ${IMAGE_GALLERY_COL} -le ${IMAGE_GALLERY_NUM_IMAGES_PER_ROW} ]; then
					printf "\t</tr>\n" >> ${IMAGE_GALLERY} 
				fi
			fi

			if ! [ ${IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE} -eq 1 ]; then
				if [ ${IMAGE_GALLERY_NUM_IMAGE_WITHIN_PAGE} -le ${IMAGE_GALLERY_NUM_IMAGES_PER_PAGE} ]; then
					printf "</table>\n" >> ${IMAGE_GALLERY}
					printf "</div>\n" >> ${IMAGE_GALLERY} 
					printf "</div>\n" >> ${IMAGE_GALLERY} 
				fi
			fi
		else
			HAVE_IMAGE_GALLERY=0
		fi

		###############################################################################
		#                              VIDEO GALLERY                                  #
		###############################################################################

		if [ -n "${VIDEOS}" ]; then
			HAVE_VIDEO_GALLERY=1
			VIDEO_GALLERY_NUM_VIDEOS_PER_PAGE=15
			VIDEO_GALLERY_NUM_VIDEOS_PER_ROW=3
			NUM_VIDEOS=`echo "${VIDEOS}" | wc -w`
			NUM_VIDEO_GALLERY_PAGES=$(((${NUM_VIDEOS} + ${VIDEO_GALLERY_NUM_VIDEOS_PER_PAGE} - 1) / ${VIDEO_GALLERY_NUM_VIDEOS_PER_PAGE}))

			if [ ${NUM_VIDEO_GALLERY_PAGES} -le 7 ]; then
				STRING_PAGE="Page "
			elif [ ${NUM_VIDEO_GALLERY_PAGES} -le 15 ]; then
				STRING_PAGE="P. "
			else
				STRING_PAGE=""
			fi

			VIDEO_GALLERY_COL=1
			VIDEO_GALLERY_PAGE_NUM=1
			VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE=1
			for VIDEO in ${VIDEOS}; do
				if [ -f "${VIDEOS_DIR}/${VIDEO}" ]; then
					echo "Adding video ${VIDEO} to video gallery..."
					VIDEO_NAME=`echo ${VIDEO} | sed -e 's/\..*$//g'`

					if [ ${VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE} -eq 1 ]; then
						if [ ${NUM_VIDEO_GALLERY_PAGES} -gt 1 ]; then
							# build a navigation bar (in reverse order)
							PAGE_NUM=${NUM_VIDEO_GALLERY_PAGES}
							VIDEO_GALLERY_NAV="</td></tr></table>";
							if ! [ ${VIDEO_GALLERY_PAGE_NUM} -eq ${NUM_VIDEO_GALLERY_PAGES} ]; then
								NEXT_PAGE_NUM=$((${VIDEO_GALLERY_PAGE_NUM} + 1))
								VIDEO_GALLERY_NAV="<a href=\"video-gallery-${NEXT_PAGE_NUM}.html\"><img src=\`\`THEME/nav-next.png\`\`></a>${VIDEO_GALLERY_NAV}"
							fi
							VIDEO_GALLERY_NAV="<td id=\"video-gallery-nav-next\">${VIDEO_GALLERY_NAV}"
							while [ ${PAGE_NUM} -gt 0 ]; do
								if [ ${PAGE_NUM} -eq ${VIDEO_GALLERY_PAGE_NUM} ]; then
									VIDEO_GALLERY_NAV="<td>${STRING_PAGE}${PAGE_NUM}</td>${VIDEO_GALLERY_NAV}"
								else
									VIDEO_GALLERY_NAV="<td><a href=\"video-gallery-${PAGE_NUM}.html\">${STRING_PAGE}${PAGE_NUM}</a></td>${VIDEO_GALLERY_NAV}"
								fi
								PAGE_NUM=$((${PAGE_NUM} - 1))
							done
							VIDEO_GALLERY_NAV="</td>${VIDEO_GALLERY_NAV}"
							if ! [ ${VIDEO_GALLERY_PAGE_NUM} -eq 1 ]; then
								PREV_PAGE_NUM=$((${VIDEO_GALLERY_PAGE_NUM} - 1))
								VIDEO_GALLERY_NAV="<a href=\"video-gallery-${PREV_PAGE_NUM}.html\"><img src=\`\`THEME/nav-prev.png\`\`></a>${VIDEO_GALLERY_NAV}"
							fi
							VIDEO_GALLERY_NAV="<table class=\"video-gallery-nav\"><tr><td id=\"video-gallery-nav-prev\">${VIDEO_GALLERY_NAV}"
						else
							VIDEO_GALLERY_NAV=
						fi

						VIDEO_GALLERY_DIR=${MY_DIR}/content/video-gallery-${VIDEO_GALLERY_PAGE_NUM}
						rm -rf ${VIDEO_GALLERY_DIR}
						mkdir -p ${VIDEO_GALLERY_DIR}
						VIDEO_GALLERY=${VIDEO_GALLERY_DIR}/content.html
						VIDEO_GALLERY_STYLE=${VIDEO_GALLERY_DIR}/style.css
						echo "Generating video gallery in ${VIDEO_GALLERY}..."

						printf "Video gallery" > ${VIDEO_GALLERY_DIR}/title.txt
						if [ ${NUM_VIDEO_GALLERY_PAGES} -gt 1 ]; then
							printf " - Page ${VIDEO_GALLERY_PAGE_NUM} of ${NUM_VIDEO_GALLERY_PAGES}" >> ${VIDEO_GALLERY_DIR}/title.txt
						fi

						printf "<div class=\"content-item\">\n" > ${VIDEO_GALLERY}
						printf "<div class=\"content-item-title\">\n" >> ${VIDEO_GALLERY}
						printf "<h2>\n" >> ${VIDEO_GALLERY}
						printf "#include \"title.txt\"\n" >> ${VIDEO_GALLERY}
						printf "</h2>\n" >> ${VIDEO_GALLERY}
						printf "</div>\n" >> ${VIDEO_GALLERY}
						printf "<div class=\"content-item-body\">" >> ${VIDEO_GALLERY}
						printf "${VIDEO_GALLERY_NAV}\n" >> ${VIDEO_GALLERY} 
						printf "<table class=\"video-gallery\">\n" >> ${VIDEO_GALLERY}
					fi

					if [ ${VIDEO_GALLERY_COL} -eq 1 ]; then
						printf "\t<tr>\n" >> ${VIDEO_GALLERY}
					fi
					printf "\t\t<td>\n" >> ${VIDEO_GALLERY} 
					printf "\t\t\t<a href=\"video-gallery-${VIDEO_GALLERY_PAGE_NUM}-view-video-${VIDEO_NAME}.html\"><img src=\`\`VIDEO_THUMBS/${VIDEO_NAME}.png\`\` alt=\"${VIDEO} thumbnail\"></a>\n" >> ${VIDEO_GALLERY}
					printf "\t\t\t<p>\n" >> ${VIDEO_GALLERY}
					if [ -f "${MY_DIR}/videos/${VIDEO_NAME}.txt" ]; then
						cat "${MY_DIR}/videos/${VIDEO_NAME}.txt" >> ${VIDEO_GALLERY}
					else
						printf "${VIDEO_NAME}" >> ${VIDEO_GALLERY}
					fi
					printf "\t\t\t</p>\n" >> ${VIDEO_GALLERY}
					printf "\t\t</td>\n" >> ${VIDEO_GALLERY} 
					VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE=$((${VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE} + 1))
					if [ ${VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE} -gt ${VIDEO_GALLERY_NUM_VIDEOS_PER_PAGE} ]; then
						VIDEO_GALLERY_COL=1
						VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE=1
						VIDEO_GALLERY_PAGE_NUM=$((${VIDEO_GALLERY_PAGE_NUM} + 1))
						printf "\t</tr>\n" >> ${VIDEO_GALLERY} 
						printf "</table>\n" >> ${VIDEO_GALLERY} 
						printf "</div>\n" >> ${VIDEO_GALLERY} 
						printf "</div>\n" >> ${VIDEO_GALLERY} 
						continue
					fi

					VIDEO_GALLERY_COL=$((${VIDEO_GALLERY_COL} + 1))
					if [ ${VIDEO_GALLERY_COL} -gt ${VIDEO_GALLERY_NUM_VIDEOS_PER_ROW} ]; then
						VIDEO_GALLERY_COL=1
						printf "\t</tr>\n" >> ${VIDEO_GALLERY}
						continue
					fi
				fi
			done

			if ! [ ${VIDEO_GALLERY_COL} -eq 1 ]; then
				if [ ${VIDEO_GALLERY_COL} -le ${VIDEO_GALLERY_NUM_VIDEOS_PER_ROW} ]; then
					printf "\t</tr>\n" >> ${VIDEO_GALLERY} 
				fi
			fi

			if ! [ ${VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE} -eq 1 ]; then
				if [ ${VIDEO_GALLERY_NUM_VIDEO_WITHIN_PAGE} -le ${VIDEO_GALLERY_NUM_VIDEOS_PER_PAGE} ]; then
					printf "</table>\n" >> ${VIDEO_GALLERY}
					printf "</div>\n" >> ${VIDEO_GALLERY} 
					printf "</div>\n" >> ${VIDEO_GALLERY} 
				fi
			fi
		else
			HAVE_VIDEO_GALLERY=0
		fi

		###############################################################################
		#                                    Site map                                 #
		###############################################################################

		CONTENTS=`cd ${MY_DIR}/content; ls`
		SITE_MAP="${MY_DIR}/content/sitemap/sitemap.html"
		mkdir -p "${MY_DIR}/content/sitemap"

		echo "Sitemap" > "${MY_DIR}/content/sitemap/title.txt"

		cat << EOF > "${MY_DIR}/content/sitemap/content.html"
<div class="content-item">
	<div class="content-item-title">
		<h2>
#include "title.txt"
		</h2>
	</div>
	<div class="content-item-body">
#include "sitemap.html"
	</div>
</div>
EOF

		# list all contents
		printf "<h3>Contents</h3>" > ${SITE_MAP}
		printf "<ul>" >> ${SITE_MAP}
		for CONTENT_DIR in ${CONTENTS}; do
			regex="^[0-9][0-9]*.*"   # filter error pages (e.g. 404.html)
			if ! [[ ${CONTENT_DIR} =~ ${regex} ]]; then
				regex="^.*-view-.*"   # filter *-view-*
				if ! [[ ${CONTENT_DIR} =~ ${regex} ]]; then
					echo "Adding ${CONTENT_DIR}.html to site map..."
					CONTENT_TITLE="`cat ${MY_DIR}/content/${CONTENT_DIR}/title.txt`"
					printf "<li>${CONTENT_TITLE}: <a class=\"online-document\" href=\"${CONTENT_DIR}.html\">view</a></li>\n" >> ${SITE_MAP}
				fi
			fi
		done
		printf "</ul>" >> ${SITE_MAP}

		# list all image views
		printf "<h3>Screenshots</h3>" >> ${SITE_MAP}
		printf "<ul>" >> ${SITE_MAP}
		for IMAGE in ${IMAGES}; do
			if [ -f "${IMAGES_DIR}/${IMAGE}" ]; then
				IMAGE_NAME=`echo ${IMAGE} | sed -e 's/\..*$//g'`
				printf "<li>" >> ${SITE_MAP}
				if [ -f "${MY_DIR}/images/${IMAGE_NAME}.txt" ]; then
					cat "${MY_DIR}/images/${IMAGE_NAME}.txt" >> ${SITE_MAP}
				else
					printf "${IMAGE}"
				fi
				printf ": <a class=\"online-document\" href=\"sitemap-view-image-${IMAGE_NAME}.html\">view</a></li>\n" >> ${SITE_MAP}
			fi
		done
		printf "</ul>" >> ${SITE_MAP}

		# list all video views
		printf "<h3>Videos</h3>" >> ${SITE_MAP}
		printf "<ul>" >> ${SITE_MAP}
		for VIDEO in ${VIDEOS}; do
			if [ -f "${VIDEOS_DIR}/${VIDEO}" ]; then
				VIDEO_NAME=`echo ${VIDEO} | sed -e 's/\..*$//g'`
				printf "<li>" >> ${SITE_MAP}
				if [ -f "${MY_DIR}/videos/${VIDEO_NAME}.txt" ]; then
					cat "${MY_DIR}/videos/${VIDEO_NAME}.txt" >> ${SITE_MAP}
				else
					printf "${VIDEO}"
				fi
				printf ": <a class=\"online-document\" href=\"sitemap-view-video-${VIDEO_NAME}.html\">view</a></li>\n" >> ${SITE_MAP}
			fi
		done
		printf "</ul>" >> ${SITE_MAP}

		###############################################################################
		#                                 IMAGE VIEW                                  #
		###############################################################################

		rm -rf ${MY_DIR}/content/*-view-image-*

		CONTENTS="`cd ${MY_DIR}/content; ls`"
		for CONTENT_DIR in ${CONTENTS}; do
			CONTENT_TITLE="`cat ${MY_DIR}/content/${CONTENT_DIR}/title.txt`"
			IMAGE_NAMES=`cat ${MY_DIR}/content/${CONTENT_DIR}/*.html | sed -n "s/.*href=\"${CONTENT_DIR}-view-image-\(.*\)\.html.*/\1/Ip" | sort -u`

			for IMAGE_NAME in ${IMAGE_NAMES}; do
				IMAGE="${IMAGE_NAME}.png"
				if [ ! -f "${IMAGES_DIR}/${IMAGE}" ]; then
					IMAGE="${IMAGE_NAME}.jpg"
				fi
				if [ -f "${IMAGES_DIR}/${IMAGE}" ]; then
					echo "Generating view for image ${IMAGE} from ${CONTENT_DIR}.html..."
					VIEW_DIR=${MY_DIR}/content/${CONTENT_DIR}-view-image-${IMAGE_NAME}
					mkdir -p "${VIEW_DIR}"
					printf "<div class=\"image-view\">\n" > "${VIEW_DIR}/content.html"
					if [ -f "${MY_DIR}/images/${IMAGE_NAME}.txt" ]; then
						printf "<h1>" >> "${VIEW_DIR}/content.html"
						cat "${MY_DIR}/images/${IMAGE_NAME}.txt" >> "${VIEW_DIR}/content.html"
						printf "</h1>\n" >> "${VIEW_DIR}/content.html"
						printf "Screenshot: " > "${VIEW_DIR}/title.txt"
						cat "${MY_DIR}/images/${IMAGE_NAME}.txt" >> "${VIEW_DIR}/title.txt"
					else
						printf "<h1>${IMAGE}</h1>\n" >> "${VIEW_DIR}/content.html"
						printf "Screenshot: ${IMAGE}" > "${VIEW_DIR}/title.txt"
					fi
					printf "<table>\n" >> "${VIEW_DIR}/content.html"
					printf "\t<tr>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Download <a class=\"download-file\" href=\`\`IMAGES/${IMAGE}\`\`>${IMAGE}</a></td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Enter <a class=\"online-document\" href=\"image-gallery-1.html\">Screenshot Gallery</a></td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Back to <a class=\"online-document\" href=\"${CONTENT_DIR}.html\">${CONTENT_TITLE}</a></td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t</tr>\n" >> "${VIEW_DIR}/content.html"
					printf "</table>\n" >> "${VIEW_DIR}/content.html"
					printf "\t<a href=\"${CONTENT_DIR}.html\"><img src=\`\`IMAGES/${IMAGE}\`\` alt=\"${IMAGE}\"></a>\n" >> "${VIEW_DIR}/content.html"
					printf "\t</div>\n" >> "${VIEW_DIR}/content.html"

					cat << EOF > "${VIEW_DIR}/style.css"
.content
{
	width:100%;
}
.aside
{
	display:none;
}
EOF
				fi
			done
		done

		###############################################################################
		#                                 VIDEO VIEW                                  #
		###############################################################################

		CONTENTS=$(cd ${MY_DIR}/content; ls)
		for CONTENT_DIR in ${CONTENTS}; do
			CONTENT_TITLE="`cat ${MY_DIR}/content/${CONTENT_DIR}/title.txt`"
			VIDEO_NAMES=`cat ${MY_DIR}/content/${CONTENT_DIR}/*.html | sed -n "s/.*href=\"${CONTENT_DIR}-view-video-\(.*\)\.html.*/\1/Ip" | sort -u`

			for VIDEO_NAME in ${VIDEO_NAMES}; do
				VIDEO="${VIDEO_NAME}.avi"
				if [ -f "${VIDEOS_DIR}/${VIDEO}" ]; then
					echo "Generating view for video ${VIDEO} from ${CONTENT_DIR}.html..."
					VIEW_DIR=${MY_DIR}/content/${CONTENT_DIR}-view-video-${VIDEO_NAME}
					mkdir -p "${VIEW_DIR}"
					printf "<div class=\"video-view\">\n" > "${VIEW_DIR}/content.html"
					if [ -f "${MY_DIR}/videos/${VIDEO_NAME}.txt" ]; then
						printf "<h1>" >> "${VIEW_DIR}/content.html"
						cat "${MY_DIR}/videos/${VIDEO_NAME}.txt" >> "${VIEW_DIR}/content.html"
						printf "</h1>\n" >> "${VIEW_DIR}/content.html"
						printf "Video: " > "${VIEW_DIR}/title.txt"
						cat "${MY_DIR}/videos/${VIDEO_NAME}.txt" >> "${VIEW_DIR}/title.txt"
					else
						printf "<h1>${VIDEO_NAME}</h1>\n" >> "${VIEW_DIR}/content.html"
						printf "Video: ${VIDEO_NAME}" > "${VIEW_DIR}/title.txt"
					fi
					printf "<table class=\"video-view-nav\">\n" >> "${VIEW_DIR}/content.html"
					printf "\t<tr>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Download <a class=\"download-video\" href=\`\`VIDEOS/${VIDEO}\`\`>video</a>" >> "${VIEW_DIR}/content.html"
					if [ -f "${MY_DIR}/videos/${VIDEO_NAME}_codec.txt" ]; then
						printf " [" >> "${VIEW_DIR}/content.html"
						cat "${MY_DIR}/videos/${VIDEO_NAME}_codec.txt" >> "${VIEW_DIR}/content.html"
						printf " ]" >> "${VIEW_DIR}/content.html"
					fi
					printf "</td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Enter <a class=\"online-document\" href=\"video-gallery-1.html\">Video Gallery</a></td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t\t<td>Back to <a class=\"online-document\" href=\"${CONTENT_DIR}.html\">${CONTENT_TITLE}</a></td>\n" >> "${VIEW_DIR}/content.html"
					printf "\t</tr>\n" >> "${VIEW_DIR}/content.html"
					printf "</table>\n" >> "${VIEW_DIR}/content.html"
					printf "<object type=\"video/x-msvideo\" data=\`\`VIDEOS/${VIDEO}\`\` width=\"1024\" height=\"792\">\n" >> "${VIEW_DIR}/content.html"
					# I use "SRC" instead of "src" because firefox under Windows does not like when "src" has a value
					printf "\t<param name=\"SRC\" value=\`\`VIDEOS/${VIDEO}\`\`>\n" >> "${VIEW_DIR}/content.html"
					printf "\t<param name=\"autoplay\" value=\"true\">\n" >> "${VIEW_DIR}/content.html"
					printf "\t<param name=\"autoStart\" value=\"1\">\n" >> "${VIEW_DIR}/content.html"
					printf "\t#include \"no_video_plugin.html\"\n" >> "${VIEW_DIR}/content.html"
					printf "</object>\n" >> "${VIEW_DIR}/content.html"
					printf "\t</div>\n" >> "${VIEW_DIR}/content.html"
					printf ".aside\n" > "${VIEW_DIR}/style.css"
					printf "{\n" >> "${VIEW_DIR}/style.css"
					printf "\tdisplay:none;\n" >> "${VIEW_DIR}/style.css"
					printf "}\n" >> "${VIEW_DIR}/style.css"
				fi
			done
		done

		###############################################################################
		#                                DOWNLOADS                                    #
		###############################################################################

		DOWNLOADS=`cd ${DOWNLOADS_DIR}; find . \( -name "*.tar.gz" -o -name "*.tar.bz2" -o -name "*.zip" -o -name "*.exe" -o -name "*.deb" -o -name "*.rpm" -o -name "*.pdf" \) 2> /dev/null`

		for DOWNLOAD in ${DOWNLOADS}; do
			if [ -f "${DOWNLOADS_DIR}/${DOWNLOAD}" ]; then
				echo "Copying downloads/${DOWNLOAD}"
				mkdir -p `dirname "${HERE}/site/downloads/${DOWNLOAD}"`
				cp "${DOWNLOADS_DIR}/${DOWNLOAD}" "${HERE}/site/downloads/${DOWNLOAD}"
			fi
		done

		###############################################################################
		#                           CONTENT and STYLESHEET                            #
		###############################################################################

		# list all content materials (do not move above since some contents have been generated for news and image/video galleries/views)
		THEMES=`cd ${MY_DIR}/themes; ls`
		CONTENTS=`cd ${MY_DIR}/content; ls`

		echo "Copying base stylesheet..."
		cp ${MY_DIR}/template/base.css ${HERE}/site/style/base.css || exit 1

		for THEME in ${THEMES}; do
			if [ -d "${MY_DIR}/themes/${THEME}" ]; then
				if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
					THEME_ROOT=
				else
					THEME_ROOT="${THEME}"
					mkdir -p ${HERE}/site/${THEME}
				fi

				mkdir -p ${HERE}/site/${THEME_ROOT}/style

				THEME_STYLE=
				CONTENT_STYLE=

				if [ -f "${MY_DIR}/themes/${THEME}/theme.css" ]; then
					echo "Copying stylesheet for theme ${THEME}"
					cp "${MY_DIR}/themes/${THEME}/theme.css" "${HERE}/site/${THEME_ROOT}/style/theme.css" || exit 1
					THEME_STYLE="style/theme.css"
				fi

				cp ${MY_DIR}/themes/${THEME}/*.png ${HERE}/site/${THEME_ROOT}/style

				for CONTENT_DIR in ${CONTENTS}; do
					if [ -f "${MY_DIR}/content/${CONTENT_DIR}/style.css" ]; then
						echo "Copying stylesheet for ${CONTENT_DIR}.html..."
						mkdir -p "${HERE}/site/${THEME_ROOT}/style/${CONTENT_DIR}"
						cp "${MY_DIR}/content/${CONTENT_DIR}/style.css" "${HERE}/site/${THEME_ROOT}/style/${CONTENT_DIR}/style.css" || exit 1
						CONTENT_STYLE="style/${CONTENT_DIR}/style.css"
					else
						CONTENT_STYLE=
					fi

					echo "Building ${CONTENT_DIR}.html for theme ${THEME}..."

					THEME_NAV=
					for THEME2 in ${THEMES}; do
						if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
							if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
								THEME_NAV="${THEME_NAV} <a href=\"${CONTENT_DIR}.html\">${THEME2}</a>"
							else
								THEME_NAV="${THEME_NAV} <a href=\"${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
							fi
						else
							if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
								THEME_NAV="${THEME_NAV} <a href=\"../${CONTENT_DIR}.html\">${THEME2}</a>"
							else
								THEME_NAV="${THEME_NAV} <a href=\"../${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
							fi
						fi
					done

					if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
						SITE_PREFIX=
					else
						SITE_PREFIX="../"
					fi

					PAGE_TITLE="`cat ${MY_DIR}/content/${CONTENT_DIR}/title.txt`"

					if [ -f "${MY_DIR}/content/${CONTENT_DIR}/description.txt" ]; then
						META_DESCRIPTION="`cat ${MY_DIR}/content/${CONTENT_DIR}/description.txt`"
					else
						META_DESCRIPTION=
					fi

					if [ -f "${MY_DIR}/content/${CONTENT_DIR}/keywords.txt" ]; then
						META_KEYWORDS="`cat ${MY_DIR}/content/${CONTENT_DIR}/keywords.txt`"
					else
						META_KEYWORDS=
					fi

					BASE_STYLE="${SITE_PREFIX}style/base.css"

					if ! [ -z PAGE_TITLE ]; then
						HAVE_PAGE_TITLE=1
					else
						HAVE_PAGE_TITLE=0
					fi

					if ! [ -z META_DESCRIPTION ]; then
						HAVE_META_DESCRIPTION=1
					else
						HAVE_META_DESCRIPTION=0
					fi

					if ! [ -z META_KEYWORDS ]; then
						HAVE_META_KEYWORDS=1
					else
						HAVE_META_KEYWORDS=0
					fi

					if ! [ -z BASE_STYLE ]; then
						HAVE_BASE_STYLE=1
					else
						HAVE_BASE_STYLE=0
					fi

					if ! [ -z THEME_STYLE ]; then
						HAVE_THEME_STYLE=1
					else
						HAVE_THEME_STYLE=0
					fi

					if ! [ -z CONTENT_STYLE ]; then
						HAVE_CONTENT_STYLE=1
					else
						HAVE_CONTENT_STYLE=0
					fi

					cpp \
						"-DHAVE_PAGE_TITLE=${HAVE_PAGE_TITLE}" \
						"-DPAGE_TITLE=${PAGE_TITLE}" \
						"-DHAVE_META_DESCRIPTION=${HAVE_META_DESCRIPTION}" \
						"-DMETA_DESCRIPTION=${META_DESCRIPTION}" \
						"-DHAVE_META_KEYWORDS=${HAVE_META_KEYWORDS}" \
						"-DMETA_KEYWORDS=${META_KEYWORDS}" \
						"-DHAVE_BASE_STYLE=${HAVE_BASE_STYLE}" \
						"-DBASE_STYLE=${BASE_STYLE}" \
						"-DHAVE_THEME_STYLE=${HAVE_THEME_STYLE}" \
						"-DTHEME_STYLE=${THEME_STYLE}" \
						"-DHAVE_CONTENT_STYLE=${HAVE_CONTENT_STYLE}" \
						"-DCONTENT_STYLE=${CONTENT_STYLE}" \
						"-DTHEME_NAV=${THEME_NAV}" \
						"-DIMAGES=${SITE_PREFIX}images" \
						"-DVIDEOS=${SITE_PREFIX}videos" \
						"-DGLYPHES=${SITE_PREFIX}glyphes" \
						"-DIMAGE_THUMBS=${SITE_PREFIX}images_thumbs" \
						"-DVIDEO_THUMBS=${SITE_PREFIX}videos_thumbs" \
						"-DDOWNLOADS=${SITE_PREFIX}downloads" \
						"-DTHEME=${SITE_PREFIX}style" \
						"-DHAVE_NEWS=${HAVE_NEWS}" \
						-undef \
						-P \
						-I${MY_DIR} \
						-I${MY_DIR}/template \
						-I${MY_DIR}/content/${CONTENT_DIR} \
						${MY_DIR}/template/template.html | sed -e "s/\\\\doubleshash/\/\//g" -e "s/\`\`/\"/g" -e "s/\`/\'/g" > ${HERE}/site/${THEME_ROOT}/${CONTENT_DIR}.html || exit -1
				done

				if [ "${PUBLIC}" = "y" ]; then
					cat << EOF > "${HERE}/site/${THEME_ROOT}/.htaccess"
ErrorDocument 404 /site/404.html
EOF
				else
					cat << EOF > "${HERE}/site/${THEME_ROOT}/.htaccess"
SSLRequireSSL

AuthUserFile /homez.87/unisimvp/www/private/.htpasswd
AuthName "Please login"
AuthType Basic
<limit GET POST>
require valid-user
</Limit>

ErrorDocument 404 /private/${THEME_ROOT}/404.html
EOF
				fi
			fi
		done

		echo "The web site is in ${HERE}/site"

		cd "${HERE}"
		;;
	clean)
		# clean everything
		rm -rf "${HERE}/site"
		rm -rf ${MY_DIR}/content/*-view-video-*
		rm -rf ${MY_DIR}/content/*-view-image-*
		rm -rf ${MY_DIR}/content/video-gallery-*
		rm -rf ${MY_DIR}/content/image-gallery-*
		rm -f "${HOME_NEWS}"
		rm -rf "${MY_DIR}/content/news"
		rm -f "${WHATS_NEW}"
		rm -rf "${MY_DIR}/content/sitemap"
		;;
	*)
		usage
		exit -1
		;;
esac
