#!/bin/bash
#
#

# 下载阿里云的yum源
curl -so /etc/yum.repos.d/Centos-7.repo http://mirrors.aliyun.com/repo/Centos-7.repo

# 安装要用到的包
yum -y install bash-completion vim lrzsz chrony git yum-utils epel-release

# 下载阿里云的epel源
curl -o /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo

# 更新整个系统
yum -y update

# 关闭不用的 selinux 
sed -ri 's/(SELINUX=).*/\1disabled/g' /etc/selinux/config

# 设置时区为亚洲上海
\rm /etc/localtime
ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

# 修改系统限制用户打开文件数与创建进程数
cat > /etc/security/limits.d/20-nproc.conf <<EOF
# Default limit for number of user's processes to prevent
# accidental fork bombs.
# See rhbz #432903 for reasoning.

*          soft    nproc     unlimited
root       soft    nproc     unlimited
EOF

cat >> /etc/security/limits.conf <<EOF
*     -   nproc    65535
*     -   nofile    655350
EOF

# 关闭所有默认系统服务
systemctl list-unit-files --state enabled|awk '{print $1}'|xargs -i systemctl disable {}

# 开启用到的服务(计划任务, 系统日志, ssh远程登录, 时间同步)
systemctl enable crond rsyslog sshd chronyd network