NET ACCOUNTS        >  netcmd-accounts.log
NET USER            >  netcmd-user.log
NET GROUP           >  netcmd-group.log
NET VIEW            >  netcmd-view.log

netsh firewall show allowedprogram             >  netsh-firewall.log
netsh firewall show config                     >> netsh-firewall.log
netsh firewall show currentprofile             >> netsh-firewall.log
netsh firewall show icmpsetting                >> netsh-firewall.log
netsh firewall show logging                    >> netsh-firewall.log
netsh firewall show multicastbroadcastresponse >> netsh-firewall.log
netsh firewall show notifications              >> netsh-firewall.log
netsh firewall show opmode                     >> netsh-firewall.log
netsh firewall show portopening                >> netsh-firewall.log
netsh firewall show service                    >> netsh-firewall.log
netsh firewall show state                      >> netsh-firewall.log

netsh interface ip show address   >  netsh-interface.log
netsh interface ip show config    >> netsh-interface.log
netsh interface ip show dns       >> netsh-interface.log
netsh interface ip show icmp      >> netsh-interface.log
netsh interface ip show interface >> netsh-interface.log
netsh interface ip show ipaddress >> netsh-interface.log
netsh interface ip show ipnet     >> netsh-interface.log
netsh interface ip show ipstats   >> netsh-interface.log
netsh interface ip show joins     >> netsh-interface.log
netsh interface ip show offload   >> netsh-interface.log
netsh interface ip show tcpconn   >> netsh-interface.log
netsh interface ip show tcpstats  >> netsh-interface.log
netsh interface ip show udpconn   >> netsh-interface.log
netsh interface ip show udpstats  >> netsh-interface.log
netsh interface ip show wins      >> netsh-interface.log

net use                           > net-use.log
net session                       > net-session.log
net share                         > net-share.log
net config workstation            > net-config-workstation.log
net config server                 > net-config-server.log

