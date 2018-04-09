#!/bin/bash
# build the project using cmake
# only works on linux, use cmake-gui + visual studio for windows

function init {
	mkdir -p build && cd build
	cmake -G "Unix Makefiles"  ..
}

function build {
	make
}

function run {
	./calculator_game
}

function clean {
	rm -rfv build
}

if [ $# -lt 1 ]; then
	echo 'usage: autorun [clean|init|build|run]'
else
	if [ $1 == 'clean' ]; then
		clean
	elif [ $1 == 'init' ]; then
		init
	elif [ $1 == 'build' ]; then
		init
		build
	elif [ $1 == 'run' ]; then

		# not always required, looking for a better soln
		init
		build
		echo '*** RUNNING ***' && run
	else
		echo 'did nothing...'
	fi
fi

