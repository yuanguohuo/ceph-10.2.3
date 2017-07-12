#!/usr/bin/bash
yum -y  install boost-devel.x86_64
yum -y  install lttng-ust-devel.x86_64
yum -y  install hdparm.x86_64
yum -y  install gdisk.x86_64
yum -y  install redhat-lsb-core.x86_64
yum -y  install python-requests.noarch
yum -y  install python-setuptools.noarch
yum -y  install fcgi-devel.x86_64
yum -y  install libbabeltrace-devel.x86_64
yum -y  install gperftools-libs.x86_64
yum -y  install selinux-policy.noarch
yum -y  install leveldb.x86_64
yum -y  install fuse-devel.x86_64
yum -y  install python-flask.noarch
yum -y  install mailcap.noarch
yum -y  install jemalloc.x86_64 jemalloc-devel.x86_64

rm -fr /etc/ceph-bak
cp -fr /etc/ceph /etc/ceph-bak

rpm -e fio-2.2.8-2.el7.x86_64

rpm -e librados2-devel-10.2.3-0.el7.centos.x86_64
rpm -e ceph-radosgw-10.2.3-0.el7.centos.x86_64
rpm -e ceph-10.2.3-0.el7.centos.x86_64
rpm -e ceph-mds-10.2.3-0.el7.centos.x86_64
rpm -e ceph-mon-10.2.3-0.el7.centos.x86_64
rpm -e ceph-osd-10.2.3-0.el7.centos.x86_64
rpm -e ceph-selinux-10.2.3-0.el7.centos.x86_64 ceph-base-10.2.3-0.el7.centos.x86_64
rpm -e ceph-common-10.2.3-0.el7.centos.x86_64
rpm -e libradosstriper1-10.2.3-0.el7.centos.x86_64
rpm -e librgw2-10.2.3-0.el7.centos.x86_64
rpm -e python-cephfs-10.2.3-0.el7.centos.x86_64
rpm -e libcephfs1-10.2.3-0.el7.centos.x86_64
rpm -e python-rbd-10.2.3-0.el7.centos.x86_64
rpm -e librbd1-10.2.3-0.el7.centos.x86_64
rpm -e python-rados-10.2.3-0.el7.centos.x86_64
rpm -e librados2-10.2.3-0.el7.centos.x86_64

yum remove librados2-1:0.80.7-3.el7.x86_64 -y

rpm -hiv librados2-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv python-rados-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv librbd1-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv python-rbd-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv libcephfs1-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv python-cephfs-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv librgw2-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv libradosstriper1-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-common-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-selinux-10.2.3-0.el7.centos.x86_64.rpm ceph-base-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-osd-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-mon-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-mds-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv ceph-radosgw-10.2.3-0.el7.centos.x86_64.rpm
rpm -hiv librados2-devel-10.2.3-0.el7.centos.x86_64.rpm

rm -fr /etc/ceph
mv /etc/ceph-bak /etc/ceph
