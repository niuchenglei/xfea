#!/bin/sh

case $1 in
    offline)
        echo "make $2 offline"
        make $2 -f Makefile.offline
        ;;
    *)
        echo "make $2 offline and online"
        make $2 -f Makefile.offline
        make $2 -f Makefile.fst_online
        ;;
esac

