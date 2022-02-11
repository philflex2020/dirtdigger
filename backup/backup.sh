#!/bin/bash
# I want to back up most of  my files somewhere $DEST
# I want to skip certain directories but I want a list of fies that have
# changed.

# I may do this from a remote system so that I can keep track of things
# the rsync  used for this is
#rsync -v -e ssh remoteuser@X.X.X.X:/home/remoteuser/transfer/testfile.txt /home/localuser/

# I am on pi2004 with a big disk mounted on /media/pi/Elements
# I want to get to stuff on pi4001
#
# first get a list of dirs on that system
BUHOME=/media/pi/Elements
TARG=pi4001

BUFILES=`ssh pi4001 "ls -1 /"`

BLFILES="etc boot bin dev lost+found proc tmp mnt sys home sbin root var opt media usr srv lib run"
for f in $BUFILES ; do
    echo $f
done
echo " blacklist"

for f in $BLFILES ; do
    echo $f
done

onblist()
{
    for f in $BLFILES ; do
	if [ "$f" = "$1" ] ; then
	    return 1
	fi
	   
    done
    return 0
}

#output of the -i command
#YXcstpoguax  path/to/file
#|||||||||||
#`----------- the type of update being done::
# ||||||||||   <: file is being transferred to the remote host (sent).
# ||||||||||   >: file is being transferred to the local host (received).
# ||||||||||   c: local change/creation for the item, such as:
# ||||||||||      - the creation of a directory
# ||||||||||      - the changing of a symlink,
# ||||||||||      - etc.
# ||||||||||   h: the item is a hard link to another item (requires --hard-links).
# ||||||||||   .: the item is not being updated (though it might have attributes that are being modified).
# ||||||||||   *: means that the rest of the itemized-output area contains a message (e.g. "deleting").
# ||||||||||
# `---------- the file type:
#  |||||||||   f for a file,
#  |||||||||   d for a directory,
#  |||||||||   L for a symlink,
#  |||||||||   D for a device,
#  |||||||||   S for a special file (e.g. named sockets and fifos).
#  |||||||||
#  `--------- c: different checksum (for regular files)
#   ||||||||     changed value (for symlink, device, and special file)
#   `-------- s: Size is different
#    `------- t: Modification time is different
#     `------ p: Permission are different
#      `----- o: Owner is different
#       `---- g: Group is different
#        `--- u: The u slot is reserved for future use.
#         `-- a: The ACL information changed
	

for file in $BUFILES ; do
    onblist $file
    FF=$?
    #echo "return = $FF"
    if [ "$FF" = "1" ] ; then
	echo "on blacklist $file "
    else
	echo " running  $file "
	mkdir -p $BUHOME/$TARG/test
	# get chages
	rsync -r -a -i --dry-run -v -e ssh --rsync-path='sudo rsync' pi@$TARG:/$file $BUHOME/$TARG/test
	# copy files
	#rsync -r -a -v -e ssh --rsync-path='sudo rsync' pi@$TARG:/$file $BUHOME/$TARG/test

    fi
    
done
