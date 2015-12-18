#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>	/*��Linuxϵͳ������ʱ��ͷ�ļ�*/
#include<unistd.h>	/*����POSIX��׼�����unix��ϵͳ������ų�����ͷ�ļ������������UNIXϵͳ����ĺ���ԭ�ͣ�����read������write������getpid����*/
#include<string.h>
#include<sys/socket.h>	/*��������socket��������*/
#include<sys/types.h>
#include<netdb.h>	/*�������������йصĽṹ���������ͣ��꣬����������gethostbyname()��*/
#include<errno.h>	/*sys/types.h��������Ϊ����ϵͳ��������*/
#include<arpa/inet.h>	/*inet_ntoa()��inet_addr()������������������ arpa/inet.h*/
#include<signal.h>	/*���̶��źŽ��д���*/
#include<netinet/in.h>	/*��������ַ��*/

#include"ping.h"
#define IP_HSIZE sizeof(struct iphdr)   /*����IP_HSIZEΪipͷ������*/
#define IPVERSION  4   /*����IPVERSIONΪ4��ָ����ipv4*/
/*���õ�ʱ����һ���ṹ�壬����ʱ���ã��ظ���ʱ����ʱֵ��Ϊ1��*/
struct itimerval val_alarm={.it_interval.tv_sec = 1,	
                               .it_interval.tv_usec=0,
                               .it_value.tv_sec=0,
                               .it_value.tv_usec=1
                                };

int main(int argc,char **argv)	/*argc��ʾ���γ����������в�������Ŀ��argv��һ��ָ���ַ�������ָ�룬����ÿһ���ַ���Ӧһ������*/
{
    struct hostent *host; /*�ýṹ������include<netdb.h>*/ 
    int on =1;

    if(argc<2){		/*�ж��Ƿ������˵�ַ*/
    printf("Usage: %s hostname\n",argv[0]);
    exit(1);
    }

    if((host=gethostbyname(argv[1]))==NULL){	/*gethostbyname()���ض�Ӧ�ڸ����������İ����������ֺ͵�ַ��Ϣ�Ľṹָ��,*/
      perror("can not understand the host name"); 	/*��ⲻ������ĵ�ַ*/
      exit(1);
      }

      hostname=argv[1];/*ȡ����ַ��*/

      memset(&dest,0,sizeof dest);	/*��dest��ǰsizeof(dest)���ֽ��滻Ϊ0������s,�˴�Ϊ��ʼ��,������ڴ�����*/
      dest.sin_family=PF_INET;	/*PF_INETΪIPV4��internetЭ�飬��<netinet/in.h>�У���ַ��*/ 
      dest.sin_port=ntohs(0);	/*�˿ں�,ntohs()����һ���������ֽ�˳���������*/
      dest.sin_addr=*(struct in_addr *)host->h_addr_list[0];/*host->h_addr_list[0]�ǵ�ַ��ָ��.����IP��ַ����ʼ��*/

      if((sockfd = socket(PF_INET,SOCK_RAW,IPPROTO_ICMP))<0){ /*PF_INEI�׽���Э���壬SOCK_RAW�׽������ͣ�IPPROTO_ICMPʹ��Э�飬����socket����������һ���ܹ���������ͨ�ŵ��׽��֡������ж��Ƿ񴴽��ɹ�*/
          perror("raw socket created error");
          exit(1);
       }

       setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on));  /*���õ�ǰ�׽���ѡ���ض�����ֵ��sockfd�׽��֣�IPPROTO_IPЭ���ΪIP�㣬IP_HDRINCL�׽���ѡ����Ŀ���׽��ֽ��ջ�����ָ�룬sizeof(on)���������ȵĳ���*/

setuid(getuid());/*getuid()��������һ�����ó������ʵ�û�ID,setuid()������ͨ�û�������root�û��Ľ�ɫ����ֻ��root�ʺŲ������еĳ�������*/
pid=getpid(); /*getpid��������ȡ��Ŀǰ���̵Ľ���ʶ����*/

set_sighandler();/*���źŴ���*/
printf("Ping %s(%s): %d bytes data in ICMP packets.\n",
       argv[1],inet_ntoa(dest.sin_addr),datalen);

       if((setitimer(ITIMER_REAL,&val_alarm,NULL))==-1)	/*��ʱ����*/
          bail("setitimer fails.");


          recv_reply();/*����pingӦ��*/

          return 0;
}
/*����ping��Ϣ*/
void send_ping(void)
{
    struct iphdr *ip_hdr;	/*iphdrΪIPͷ���ṹ��*/
    struct icmphdr *icmp_hdr;	/*icmphdrΪICMPͷ���ṹ��*/
    int len;
    int len1;
		/*ipͷ���ṹ�������ʼ��*/
    ip_hdr=(struct iphdr *)sendbuf;	/*�ַ���ָ��*/	
    ip_hdr->hlen=sizeof(struct iphdr)>>2;	/*ͷ������*/
    ip_hdr->ver=IPVERSION;	/*�汾*/
    ip_hdr->tos=0;	/*��������*/
    ip_hdr->tot_len=IP_HSIZE+ICMP_HSIZE+datalen;	/*����ͷ�������ݵ��ܳ���*/
    ip_hdr->id=0;	/*��ʼ�����ı�ʶ*/
    ip_hdr->frag_off=0;	/*����flag���Ϊ0*/
    ip_hdr->protocol=IPPROTO_ICMP;/*���õ�Э��ΪICMPЭ��*/
    ip_hdr->ttl=255;	/*һ������������Ͽ��Դ���ʱ��*/
    ip_hdr->daddr=dest.sin_addr.s_addr;	/*Ŀ�ĵ�ַ*/
    len1=ip_hdr->hlen<<2;	/*ip���ݳ���*/
	/*ICMPͷ���ṹ�������ʼ��*/
    icmp_hdr=(struct icmphdr *)(sendbuf+len1);	/*�ַ���ָ��*/
    icmp_hdr->type=8;  	/*��ʼ��ICMP��Ϣ����type*/
    icmp_hdr->code=0;	/*��ʼ����Ϣ����code*/
    icmp_hdr->icmp_id=pid;	/*�ѽ��̱�ʶ���ʼ��icmp_id*/
    icmp_hdr->icmp_seq=nsent++;	/*���͵�ICMP��Ϣ��Ÿ�ֵ��icmp���*/    
    memset(icmp_hdr->data,0xff,datalen);	 /*��datalen��ǰdatalen���ֽ��滻Ϊ0xff������icmp_hdr-dat*/  

    gettimeofday((struct timeval *)icmp_hdr->data,NULL);	/* ��ȡ��ǰʱ��*/

    len=ip_hdr->tot_len;	/*�����ܳ��ȸ�ֵ��len����*/
    icmp_hdr->checksum=0;	/*��ʼ��*/
    icmp_hdr->checksum=checksum((u8 *)icmp_hdr,len);  /*����У���*/

    sendto(sockfd,sendbuf,len,0,(struct sockaddr *)&dest,sizeof (dest)); /*��socket��������*/
}
/*���ճ��򷢳���ping�����Ӧ��*/
void recv_reply()
{
    int n,len;
    int errno;

    n=nrecv=0;
    len=sizeof(from);	/*����pingӦ����Ϣ������IP*/

    while(nrecv<4){
      if((n=recvfrom(sockfd,recvbuf,sizeof recvbuf,0,(struct sockaddr *)&from,&len))<0){	/*��socket��������,�����ȷ���շ��ؽ��յ����ֽ�����ʧ�ܷ���0.*/
              if(errno==EINTR)	/*EINTR��ʾ�ź��ж�*/
              continue;
              bail("recvfrom error");
            }

            gettimeofday(&recvtime,NULL);	/*��¼�յ�Ӧ���ʱ��*/

            if(handle_pkt())	/*���յ������ICMPӦ����Ϣ*/
				
            continue;
            nrecv++;
    }

    get_statistics(nsent,nrecv);	 /*ͳ��ping����ļ����*/
}
 /*����У���*/
u16 checksum(u8 *buf,int len)
{
    u32 sum=0;
    u16 *cbuf;

    cbuf=(u16 *)buf;

    while(len>1){
    sum+=*cbuf++;
    len-=2;
    }

    if(len)
        sum+=*(u8 *)cbuf;

        sum=(sum>>16)+(sum & 0xffff);
        sum+=(sum>>16);

        return ~sum;
}
 /*ICMPӦ����Ϣ����*/
int handle_pkt()
{
    struct iphdr *ip;
    struct icmphdr *icmp;

    int ip_hlen;
    u16 ip_datalen;	/*ip���ݳ���*/
    double rtt;	/* ����ʱ��*/
    struct timeval *sendtime;

    ip=(struct iphdr *)recvbuf;

    ip_hlen=ip->hlen << 2;
    ip_datalen=ntohs(ip->tot_len)-ip_hlen;

    icmp=(struct icmphdr *)(recvbuf+ip_hlen);

    if(checksum((u8 *)icmp,ip_datalen))	/*����У���*/
       return -1;


       if(icmp->icmp_id!=pid)
        return -1;

        sendtime=(struct timeval *)icmp->data; /*����ʱ��*/
        rtt=((&recvtime)->tv_sec-sendtime->tv_sec)*1000+((&recvtime)->tv_usec-sendtime->tv_usec)/1000.0;/* ����ʱ��*/
		/*��ӡ���*/
        printf("%d bytes from %s:icmp_seq=%u ttl=%d rtt=%.3f ms\n",
               ip_datalen, /*IP���ݳ���*/
               inet_ntoa(from.sin_addr),	/*Ŀ��ip��ַ*/
               icmp->icmp_seq, /*icmp�������к�*/
               ip->ttl,	/*����ʱ��*/
               rtt);	/*����ʱ��*/

          return 0;
}
/*�����źŴ������*/
void set_sighandler()
{
    act_alarm.sa_handler=alarm_handler;
    if(sigaction(SIGALRM,&act_alarm,NULL)==-1)	/*sigaction()��������signumָ�����źű�������ø��źŵĴ�����������signumָ��Ҫ�����źŻ���Ե��źţ�&act���������õ��źŹ����壬NULL����֮ǰ���õ��źŴ���ṹ�塣�����ж϶��źŵĴ����Ƿ�ɹ���*/
       bail("SIGALRM handler setting fails.");

       act_int.sa_handler=int_handler;
       if(sigaction(SIGINT,&act_int,NULL)==-1)
         bail("SIGALRM handler setting fails.");
}
 /*ͳ��ping����ļ����*/
void get_statistics(int nsent,int nrecv)
{
    printf("--- %s ping statistics ---\n",inet_ntoa(dest.sin_addr)); /*�������ַת���ɡ�.��������ַ�����ʽ��*/
    printf("%d packets transmitted, %d received, %0.0f%% ""packet loss\n",
           nsent,nrecv,1.0*(nsent-nrecv)/nsent*100);
}
 /*���󱨸�*/
void bail(const char * on_what)
{
    fputs(strerror(errno),stderr);	/*:��ָ�����ļ�д��һ���ַ�������д���ַ���������Ƿ���\0�������ɹ�д��һ���ַ������ļ���λ��ָ����Զ����ƣ���������ֵΪ0�����򷵻�EOR(���ų�������ֵΪ-1)��*/
    fputs(":",stderr);
    fputs(on_what,stderr);
    fputc('\n',stderr);	/*��һ���ַ���һ������*/
    exit(1);
}

 /*SIGINT���ж��źţ��������*/
void int_handler(int sig)
{
    get_statistics(nsent,nrecv);	/*ͳ��ping����ļ����*/
    close(sockfd);	/*�ر������׽���*/
    exit(1);
}
 /*SIGALRM����ֹ���̣��������*/
void alarm_handler(int signo)
{
    send_ping();	/*����ping��Ϣ*/

}
