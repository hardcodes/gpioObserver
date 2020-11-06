#!/bin/bash
################################################################################
# doorOpenAction.sh
#
# is executed when the frontdoor is opened
#
# This program is free software; you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License along
#     with this program; if not, write to the Free Software Foundation, Inc.,
#     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#
# 20130923, Sven Putze (hardcodes)
################################################################################

# replace the @PLACEHOLDERS@ with details of your environment
CIFSSHARE="//@CIFSSERVER@/@CIFSSHARE@"
CIFSMOUNTPOINT="/home/@USER@/@MOUNPOINT@"
CIFSUSER="@USER@"
CIFSPWD="@PASSWORD@"
MOUNT="$(which mount)"
UMOUNT="$(which umount)"
LOCALPATH="/home/@USER@/videos"
RASPIVID="$(which raspivid)"
DURATION=60000
FPS=30
BITRATE=10000000
SUDO="$(which sudo)"
# needs package gpac
MP4BOX="$(which MP4Box)"
RM="$(which rm)"

# Today + hour in 24h format
NOW=$(date +"%Y%m%d-%H%M%S")
echo "################################################################################"
echo "$NOW - START"
echo "################################################################################"

# The NAS drive is often in powersafe mode and needs a few seconds to wake up.
# This way the first seconds after opening the door will be missing.
# So we record to the local drive first and copy afterwards
NOW=$(date +"%Y%m%d-%H%M%S")
H264FILENAME="$LOCALPATH/frontdoor-$NOW.H264"
MP4FILENAME="$CIFSMOUNTPOINT/videos/frontdoor-$NOW.mp4"
echo "recording to local drive"
$RASPIVID -o $H264FILENAME -t $DURATION -fps $FPS -b $BITRATE

# check if the network drive is mounted, it doesn't matter if it takes some seconds now,
# the video has been recorded
# OK, if someone removes the Raspberry Pi before the file has been copied, the video is also gone...
# ...they better don't find it ;-)
if [ -e "$CIFSMOUNTPOINT/videos" ]
then
	echo "$CIFSMOUNTPOINT is mounted, good"
else
	echo sudo $MOUNT -t cifs "$CIFSSHARE" "$CIFSMOUNTPOINT" -o user="$CIFSUSER",password='<password>',uid=$CIFSUSER,gid=$CIFSUSER,file_mode=0600
	if $SUDO $MOUNT -t cifs "$CIFSSHARE" "$CIFSMOUNTPOINT" -o user="$CIFSUSER",password="$CIFSPWD",uid=$CIFSUSER,gid=$CIFSUSER,file_mode=0600
	then
	    echo "success"
		if [ -e "$CIFSMOUNTPOINT/videos" ]
		then
			echo "converting to mp4 as $MP4FILENAME"
			NASPATH="$CIFSMOUNTPOINT/videos/"
			echo "$MP4BOX -add $H264FILENAME $MP4FILENAME"
			if $MP4BOX -add $H264FILENAME $MP4FILENAME
			then
				echo "success"
				$RM $H264FILENAME
			else
				echo "ERROR: could not convert $H264FILENAME to mpeg4"
			fi
			echo "unmounting SMB share"
			$SUDO $UMOUNT $CIFSMOUNTPOINT
		else
			echo "ERROR: drive is mounted but $NASPATH is missing"
		fi
	else
	  echo "ERROR: could not mount $CIFSSHARE on $CIFSMOUNTPOINT"
	fi
fi

# TODO: email notification in background

NOW=$(date +"%Y%m%d-%H%M%S")
echo "################################################################################"
echo "$NOW - END"
echo "################################################################################"
