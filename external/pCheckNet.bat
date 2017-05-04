reg export HKCU\software\microsoft\windows\currentversion\run		test1.txt
reg export HKCU\software\microsoft\windows\currentversion\runOnce	test2.txt
reg export HKCU\software\microsoft\windows\currentversion\runOnceEx	test3.txt
reg export HKLM\software\microsoft\windows\currentversion\run		test4.txt
reg export HKLM\software\microsoft\windows\currentversion\runOnce	test5.txt
reg export HKLM\software\microsoft\windows\currentversion\runOnceEx	test6.txt
reg export HKLM\system\currentcontrolset\Services			test7.txt

ipconfig /all > test8.txt
ping 192.168.3.100  >> test8.txt
ping www.google.com  >> test8.txt
netstat -nr >> test8.txt
netstat -a >> test8.txt
tasklist >> test8.txt

type test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt test7.txt test8.txt > c:\check.txt
del test1.txt
del test2.txt
del test3.txt
del test4.txt
del test5.txt
del test6.txt
del test7.txt
del test8.txt
del pCheckNet.bat
