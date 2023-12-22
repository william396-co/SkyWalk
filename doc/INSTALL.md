#安装

一、安装基础组件

1. yum install gcc gcc-c++ gdb
2. yum install mysql
3. yum install mysql-devel
4. yum install readline-devel
5. yum install python-devel
   - 确保版本一致
      - python-config --libs
      - python --version
6. 安装Python/setuptools-8.3.zip
   1. unzip setuptools-8.3.zip
   2. python setup.py build
   3. python setup.py install
7. 安装Python/MySQL-python-1.2.5.zip
   1. unzip MySQL-python-1.2.5.zip
   2. python setup.py build
   3. python setup.py install
8. 安装libcurl
   1. ./configure --with-ssl
