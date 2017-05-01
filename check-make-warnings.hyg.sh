#!/bin/bash

nohuplog=$1

if [ -z "$nohuplog" ] ; then
    echo "Error: Argument is missing."
    echo "Usage: $0 {nohup-log}"
    exit
fi


ret=`cat $nohuplog | grep warning  |           \
    grep -v "^libtool: install: warning" |     \
    grep -v "^cephfs.c:2988:7:" |              \
    grep -v "^cephfs.c:2987:15:" |             \
    grep -v "^cephfs.c:2986:7:"  |             \
    grep -v "^cephfs.c:6491:7:"  |             \
    grep -v "^cephfs.c:6490:15:" |             \
    grep -v "^cephfs.c:6489:7:" |              \
    grep -v "^cephfs.c:6706:7:" |              \
    grep -v "^cephfs.c:6705:15:" |             \
    grep -v "^cephfs.c:6704:7:"  |             \
    grep -v "^./include/ceph_fs.h:379:0:"        |            \
    grep -v "^./include/ceph_fs.h:378:0:"        |            \
    grep -v "^./include/ceph_fs.h:377:0:"        |            \
    grep -v "^./include/ceph_fs.h:376:0:"        |            \
    grep -v "^./include/ceph_fs.h:375:0:"        |            \
    grep -v "^./include/ceph_fs.h:374:0:"        |            \
    grep -v "^./include/ceph_fs.h:373:0:"        |            \
    grep -v "^civetweb/src/civetweb.c:4008:12:"  |            \
    grep -v "^civetweb/src/civetweb.c:3507:48:"  |            \
    grep -v "make\[4\]: warning: -jN"            |            \
    grep -v "make\[2\]: warning: -jN"            |            \
    grep -v "Makefile.am:44:"                    |            \
    grep -v "src/Makefile-env.am:66:"            |            \
    grep -v "src/kv/Makefile.am:21:"             |            \
    grep -v "src/kv/Makefile.am:24:"             |            \
    grep -v "src/test/Makefile.am:99:"           |            \
    grep -v "src/tracing/Makefile.am:5:"         |            \
    grep -v "src/pybind/Makefile.am:5:"          |            \
    grep -v "src/pybind/Makefile.am:7:"          |            \
    grep -v "utilities/env_mirror.cc:150:10: warning: unused variable 'bs' \[-Wunused-variable\]"                              |   \
    grep -v "utilities/env_mirror.cc:164:14: warning: unused variable 'bs' \[-Wunused-variable\]"                              |   \
    grep -v "rgw/rgw_op.cc:1812:37: warning: comparison between signed and unsigned integer expressions \[-Wsign-compare\]"`



if [ -z "$ret" ] ; then
    echo "Success"
    exit 0
else
    echo "Failure"
    echo $ret
    exit 1
fi
