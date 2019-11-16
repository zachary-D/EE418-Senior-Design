#!/bin/bash

buildDirectory=""

#CURRENTLY BROKEN - DO NOT SET TO TRUE
#When true, sets up the workspace in the current folder instead of creating a new directory
noWorkspaceFolder=false

# Builds a project directory (xfig or fig2dev)
# $1 = the glob to expand for the folder we should enter (i.e. 'xfig' or 'fig2dev')
function configureBuild()
{
	cd "$1"*

	./configure

	configExitCode="$?"

	if [[ "$configExitCode" != "0" ]]; then 
		echo "ERROR: $1 configuration failed!"
		exit 2
	fi

	make

	makeExitCode="$?"
	
	if [[ "$makeExitCode" != "0" ]]; then
		echo "ERROR: $1 build failed!"
		exit 3
	fi

	buildDirectory=$(pwd)

	echo "$buildDirectory"

	cd ..
}


#sourceCode='xfig+fig2dev-3.2.7b.tar.xz'
#sourceCode='xfig-full-3.2.7a.tar.xz'
sourceCode="git@github.com:zachary-D/EE418-Senior-Design"

#if our default source code doesn't exist, get it
#if [[ ! -f "$sourceCode" ]]; then
#	wget https://newcontinuum.dl.sourceforge.net/project/mcj/xfig%2Bfig2dev-3.2.7b.tar.xz
#fi

#Check for an alternately-specified source code repository
if [[ "$2" != "" ]]; then
	sourceCode="$2"
	echo "Using $sourceCode as the source code repository"
	sleep 3
fi

if [[ $noWorkspaceFolder == false ]]; then
	
	#Makes sure they specified a workspace name
	if [[ "$1" == "" ]]; then
		echo "Please specify a workspace name - ./configure_workspace.sh <name>"
		exit 1
	fi
	
	
	#Create a new workspace and extract the source code in it
	workspace_name="workspace_$1"
	mkdir "$workspace_name"
	cd "$workspace_name"
	git clone "$sourceCode" .
	
	#Check to see if they already have a branch to check out
	branchExists=$(git ls-remote --heads $sourceCode "$1" | wc -l)
		

	if [[ "$branchExists" == "1" ]]; then
		#If the branch exists
		git checkout "$1"
		git pull
	else
		#If the branch does not exist
		git branch "$1"
		git checkout "$1"
		git push --set-upstream origin "$1"
	fi
else
	git clone "$sourceCode"
fi

git config user.email "$1@no.email"
git config user.name "$1"

#building xfig requires fig2dev, so add the location where it will be to the PATH
export PATH="$PATH:$(pwd)/fig2dev*/fig2dev/"
make
cd ~


echo "Finished setting up workspace."
