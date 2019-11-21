#!/bin/bash

function ensureOutputExists()
{
	parent_folder_regex="^(.*)(\/[^\/]*$)"
        if [[ $1 =~ $parent_folder_regex ]] ; then
        	parent_folders="${BASH_REMATCH[1]}"
                mkdir -p "$parent_folders"
	else
		echo "Failed to identify parent folders for $1"
	fi
}

function extractFile()
{
	regex_startsWithBuild="^build"
	if [[ "$1" =~ $regex_startsWithBuild ]]; then
		echo "ignoring: $1"
		return
	fi
	
	echo "moving: $1"

	destination="build/$1"
	
	ensureOutputExists "$destination"
	
        mv "$1" "$destination"
}


for i in $( find * | grep -E "(^|\/)$1$" ); do
	extractFile "$i"

done
