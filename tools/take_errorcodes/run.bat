@echo off

echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
echo 请确保以下几点对环境的要求
echo 	1. 环境变量中有Python的配置
echo 	2. .ssh/config必须配置主机别名
echo 	3. Python安装了xlrd的插件 deps/Python/
echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

set PROTOCOL=tmp_protocol
set XLS_FILE=messageBox.xlsm
set SVNHOST=https://svn.wekeystudio.local/repos/bleach/data

rem 公用配置
set HOSTALIAS=git.wekeystudio.com
set DEFAULT=develop

set /p branch=配置文件分支(%DEFAULT%):
set /p hostalias=主机别名(%HOSTALIAS%):

if "%branch%" == "" set branch=%DEFAULT%
if "%hostalias%" == "" set hostalias=%HOSTALIAS%

echo 获取最新的协议(开发环境) ...
rd /S /Q %PROTOCOL%
git archive --format=zip --remote=%hostalias%:bleach/protocol.git --prefix=%PROTOCOL%/ --output=%PROTOCOL%.zip develop
7z.exe x %PROTOCOL%.zip

echo 导出错误码文件%XLS_FILE% ...
del /S /Q new_errorcode.csv
svn.exe export %SVNHOST%/%branch%/%XLS_FILE% ./

echo 生成新的错误码数据 ...
C:\Python\python.exe take_errorcode.py %PROTOCOL% %XLS_FILE%

del /S /Q %XLS_FILE%
RD /S /Q %PROTOCOL%
del /S /Q %PROTOCOL%.zip

pause
