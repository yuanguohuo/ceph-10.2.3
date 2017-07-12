#!/bin/bash


DIR=ceph-10.2.3-install
TGZ=$DIR.tar.gz

rm -fr $DIR $TGZ
mkdir $DIR

cp RPMS/x86_64/ceph-10.2.3-0.el7.centos.x86_64.rpm              $DIR
cp RPMS/x86_64/ceph-base-10.2.3-0.el7.centos.x86_64.rpm         $DIR
cp RPMS/x86_64/ceph-common-10.2.3-0.el7.centos.x86_64.rpm       $DIR
cp RPMS/x86_64/ceph-mds-10.2.3-0.el7.centos.x86_64.rpm          $DIR
cp RPMS/x86_64/ceph-mon-10.2.3-0.el7.centos.x86_64.rpm          $DIR
cp RPMS/x86_64/ceph-osd-10.2.3-0.el7.centos.x86_64.rpm          $DIR
cp RPMS/x86_64/ceph-radosgw-10.2.3-0.el7.centos.x86_64.rpm      $DIR
cp RPMS/x86_64/ceph-selinux-10.2.3-0.el7.centos.x86_64.rpm      $DIR
cp RPMS/x86_64/libcephfs1-10.2.3-0.el7.centos.x86_64.rpm        $DIR
cp RPMS/x86_64/librados2-10.2.3-0.el7.centos.x86_64.rpm         $DIR
cp RPMS/x86_64/libradosstriper1-10.2.3-0.el7.centos.x86_64.rpm  $DIR
cp RPMS/x86_64/librbd1-10.2.3-0.el7.centos.x86_64.rpm           $DIR
cp RPMS/x86_64/librgw2-10.2.3-0.el7.centos.x86_64.rpm           $DIR
cp RPMS/x86_64/python-cephfs-10.2.3-0.el7.centos.x86_64.rpm     $DIR
cp RPMS/x86_64/python-rados-10.2.3-0.el7.centos.x86_64.rpm      $DIR
cp RPMS/x86_64/python-rbd-10.2.3-0.el7.centos.x86_64.rpm        $DIR
cp RPMS/x86_64/librados2-devel-10.2.3-0.el7.centos.x86_64.rpm   $DIR
#cp RPMS/x86_64/ceph-debuginfo-10.2.3-0.el7.centos.x86_64.rpm    $DIR

cp yuanguo_install.sh                                           $DIR

tar czvf $TGZ $DIR
rm -fr $DIR
