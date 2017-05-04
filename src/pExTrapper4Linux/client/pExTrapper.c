/*
�u���[�h�L���X�g���M���

  IP�v���g�R���^�C�v�F UDP
  ���M�|�[�g�F 12083
  ���f�[�^�F 
GET CONSOLE COMMAND "2008/06/30 16:20:22" "192.168.1.24"

�R���\�[����M���

  IP�v���g�R���^�C�v�F TCP
  ���M�|�[�g�F 12084
  ���f�[�^�F 
TCP 01 DATA  macaddr "00:0C:29:1F:09:23" ipaddr "192.168.1.24" hostname "monster"

  IP�v���g�R���^�C�v�F TCP
  ���M�|�[�g�F 12084
  ���f�[�^�F 
TCP 02 DATA  macaddr "00:0C:29:1F:09:23" username "SYSTEM"boot_time "2008/06/30 16:11:14"time_span "00:18:47" cpu_usage "2" process "26" dwMemoryLoad "79" dwTotalPhys "104316928" dwAvailPhys "21385216" dwTotalVirtual "2147352576" dwAvailVirtual "2080309248" ulFreeBytesAvailable "7072067584" ulTotalNumberOfBytes "10725732352" ulTotalNumberOfFreeBytes "7072067584" in_packet "139" out_packet "104"


//shell�̃R�}���h�łƂ��Ă���� -> �V�X�e���R�[�����p�ӂ���Ă�͂�
TCP 02 DATA
macaddr "00:0C:29:1F:09:23"
username "SYSTEM"
boot_time "2008/06/30 16:11:14"
time_span "00:18:47" //on����Ă���
cpu_usage "2" //total
process "26" //�N�����Ă鐔 ps
dwMemoryLoad "79"
dwTotalPhys "104316928"
dwAvailPhys "21385216"
dwTotalVirtual "2147352576"
dwAvailVirtual "2080309248"
ulFreeBytesAvailable "7072067584"
ulTotalNumberOfBytes "10725732352"
ulTotalNumberOfFreeBytes "7072067584"
in_packet "139" //ifconfig
out_packet "104" //
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>

#define PORT_REQ 12083
#define PORT_ANS 12084

// �v�����b�Z�[�W���Ő擪����g�[�N��(")�����ꂾ���������IP�A�h���X�̈ʒu�ɂȂ�
#define TOKEN_COUNT 3

int makeMsg1(char *, int);
int makeMsg2(char *);
int sendMsg(char * ,char *, char *);

int main()
{
  int sock_req; //�T�[�o�[����̗v���p
  
  struct sockaddr_in addr;
  char recv_buf[1024];
  char msg1[1024], msg2[1024]; //�������b�Z�[�W
  const char command[32] = "GET CONSOLE COMMAND";

  //��M��
  sock_req = socket(AF_INET, SOCK_DGRAM, 0);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT_REQ);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(sock_req, (struct sockaddr *)&addr, sizeof(addr));


  while(1) {
    memset(recv_buf, 0, sizeof(recv_buf));
    recv(sock_req, recv_buf, sizeof(recv_buf), 0);
    printf("receive: %s\n", recv_buf);

    if (strncmp(recv_buf, command, strlen(command)) != 0) {
      printf("ignore\n\n");
      continue;
    }
    //���
    makeMsg1(msg1, sock_req);
    makeMsg2(msg2);
    printf("Msg2 %s\n\n", msg2);

    //�ȉ����M��
    sendMsg(recv_buf, msg1, msg2);
  }
  close(sock_req);

  return 0;
}
// �����p���b�Z�[�W�쐬
// TCP 01 DATA  macaddr "00:1B:77:CE:C4:19" ipaddr "192.168.1.24" hostname "monster"
int makeMsg1(char *msg, int sock)
{
  struct ifreq ifr;
  char str_mac[32];
  char str_ip[32];
  char str_host[30];
  int host_len=30;

  ifr.ifr_addr.sa_family = AF_INET; // for IPv4
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1); // for eth0
  ioctl(sock, SIOCGIFADDR, &ifr);

  // MAC�A�h���X
  sprintf(str_mac, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
          (unsigned char) ifr.ifr_hwaddr.sa_data[0],
          (unsigned char) ifr.ifr_hwaddr.sa_data[1],
          (unsigned char) ifr.ifr_hwaddr.sa_data[2],
          (unsigned char) ifr.ifr_hwaddr.sa_data[3],
          (unsigned char) ifr.ifr_hwaddr.sa_data[4],
          (unsigned char) ifr.ifr_hwaddr.sa_data[5]);

  // IP�A�h���X
  sprintf(str_ip, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

  // �z�X�g��
  if (gethostname(str_host, host_len) == -1) {
    perror("gethostname");
  }

//  sprintf(msg, "TCP 01 DATA macaddr \"%s\" ipaddr \"%s\" hostname \"%s\"", str_mac, str_ip, str_host);

  //�Ƃ肠�����Adummy
  sprintf(msg, "TCP 01 DATA  macaddr \"87:77:67:f7:67:a7\" ipaddr \"172.16.1.99\" hostname \"spark\"");

  printf("Msg1 %s\n", msg);
}

int makeMsg2(char *msg)
{
  //�Ƃ肠�����Adummy
  sprintf(msg, "TCP 02 DATA  macaddr \"87:77:67:f7:67:a7\" username \"SYSTEM\" boot_time \"2008/06/30 16:11:14\" time_span \"00:18:47\" cpu_usage \"2\" process \"26\" dwMemoryLoad \"79\" dwTotalPhys \"104316928\" dwAvailPhys \"21385216\" dwTotalVirtual \"2147352576\" dwAvailVirtual \"2080309248\" ulFreeBytesAvailable \"7072067584\" ulTotalNumberOfBytes \"10725732352\" ulTotalNumberOfFreeBytes \"7072067584\" in_packet \"139\" out_packet \"104\"");
}

int sendMsg(char *recv_buf, char *msg1, char *msg2)
{
  int sock_ans; //�T�[�o�[�ւ̉����p
  struct sockaddr_in addr;
  char *ipptr;
  int i;
  int errchk;

  bzero((char *)&addr, sizeof(addr)); //����́H
  sock_ans = socket(PF_INET, SOCK_STREAM, 0);
  if(sock_ans == -1)
  {
    printf("Error Creating Socket");
    return 0;
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(0x7F000001);
  addr.sin_port = htons(PORT_ANS);

//  printf("rcv %s\n",  recv_buf);
  ipptr = strtok(recv_buf, "\"");
  for (i = 1; i <= TOKEN_COUNT; i++) {
    ipptr = strtok(NULL, "\"");
  }
  printf("reply to %s\n",  ipptr);
  addr.sin_addr.s_addr = inet_addr(ipptr);
//  addr.sin_addr.s_addr = inet_addr("172.16.1.101"); //debug�p��


  if (connect(sock_ans, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
  }

	sendto(sock_ans, msg1, strlen(msg1), 0, (struct sockaddr *)&addr, sizeof(addr));

//  errchk = send(sock_ans, msg1, strlen(msg1), 0);
//  printf("client send msg1(%d)\n", errchk);
//  if (errchk < 0) {
    perror("send msg1");
//  }
  close(sock_ans);
  

  sock_ans = socket(PF_INET, SOCK_STREAM, 0);
  if(sock_ans == -1)
  {
    printf("Error Creating Socket");
    return 0;
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(0x7F000001);
  addr.sin_port = htons(PORT_ANS);

//  printf("rcv %s\n",  recv_buf);
  addr.sin_addr.s_addr = inet_addr(ipptr);

  if (connect(sock_ans, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
  }


	sendto(sock_ans, msg2, strlen(msg2), 0, (struct sockaddr *)&addr, sizeof(addr));
  
//  errchk = send(sock_ans, msg2, strlen(msg2), 0);
//  printf("client send msg2(%d)\n", errchk);
//  if (errchk < 0) {
    perror("send msg2");
//  }

  close(sock_ans);
}
