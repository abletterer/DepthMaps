#!/bin/sh
bindir=$(pwd)
cd /home/blettere/Projets/DepthMaps/DepthMapToPointCloud/
export LD_LIBRARY_PATH=:$LD_LIBRARY_PATH

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		 -batch -command=$bindir/gdbscript  /home/blettere/Projets/DepthMaps/build/DepthMapToPointCloud 
	else
		"/home/blettere/Projets/DepthMaps/build/DepthMapToPointCloud"  
	fi
else
	"/home/blettere/Projets/DepthMaps/build/DepthMapToPointCloud"  
fi
