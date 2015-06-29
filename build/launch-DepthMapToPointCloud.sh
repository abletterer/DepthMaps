#!/bin/sh
bindir=$(pwd)
cd /Users/bletterer/Projets/DepthMaps/DepthMapToPointCloud/
export DYLD_LIBRARY_PATH=:$DYLD_LIBRARY_PATH

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		 -batch -command=$bindir/gdbscript  /Users/bletterer/Projets/DepthMaps/build/DepthMapToPointCloud 
	else
		"/Users/bletterer/Projets/DepthMaps/build/DepthMapToPointCloud"  
	fi
else
	"/Users/bletterer/Projets/DepthMaps/build/DepthMapToPointCloud"  
fi
