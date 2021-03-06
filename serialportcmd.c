# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <termios.h>
# include <errno.h>
# include <unistd.h>
# include <string.h>
# include <getopt.h>

void parse_opts(int argc,char *argv[]);
void readserialport(int fd);

char* device;
char* isread = "no";
char* at_cmd = "AT";

int main(int argc,char *argv[])
{

	parse_opts(argc,argv);

    int fd=open(device,O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd<0)
    {
        printf("open the %s failed\n",device);
        return -1;
    }

//get the serial port attr.
    struct termios old;
    int ret=tcgetattr(fd,&old);
    if(ret<0)
    {
        printf("get the serial attr fail");
        return -1;
    }

//make the old activity(send and receiver data),and set baudrate.
    old.c_cflag |=(CLOCAL|CREAD);
    cfsetispeed(&old,B115200);
    cfsetospeed(&old,B115200);

//set the data(8bit,1 byte==char)
    old.c_cflag &= ~CSIZE;
//    old.c_cflag |= CS8|CRTSCTS;
    old.c_cflag |= CS8;
//set the stop(1bit)
    old.c_cflag &= ~CSTOPB;
//set the check(no)
    old.c_cflag &= ~PARENB;

//set timeout
    old.c_cc[VTIME]=10;
    old.c_cc[VMIN]=0;

//make the stream control no effect.hardware
//    old.c_cflag &= ~CNEW_RTSCTS;
//    old.c_cflag |= CNEW_RTSCTS;

//make the stream control no effect.software
//    old.c_iflag &= ~(IXON|IXOFF|IXANY);
//    old.c_iflag |= (IXON|IXOFF|IXANY);//effect 

//set the attr to serialport.and make the setting effect now!
    ret=tcsetattr(fd,TCSANOW,&old);
    if(ret<0)
    {
        printf("set attr fail\n");
        return -1;
    }

//write data.
//   char data[]={0x7E,0x11,0x02,0x20,0x11,0x00,0x02,0x04,0x00,0x00,0x00,0x0D};
//   *(data+8)=1099&0XFF;
//   *(data+7)=(1099>>8)&0XFF;
//    ret=write(fd,data,12);
	size_t len = sizeof(at_cmd)/sizeof(at_cmd[0]);
	printf("the at_cmd length is %d\n",len);
	char cmd[7]={0};
	strcpy(cmd,at_cmd);
	cmd[len-1]='\r';
    ret=write(fd,cmd,len);
    if(ret<0)
    {
        printf("write fail\n");
        return -1;
    }

	if(!strcmp("read1",isread))
	{
		readserialport(fd);
	}
	else if(!strcmp("read10",isread))
	{
		for(int j = 0;j < 10;j++)
		{
			readserialport(fd);
		}
	}
	else if(!strcmp("read1+",isread))
	{
		while(1)	
			readserialport(fd);
	}
/**/

    ret=tcgetattr(fd,&old);
    if(ret<0)
    {
        printf("get the serial attr fail");
        return -1;
    }

//make the old activity(send and receiver data),and set baudrate.
    old.c_cflag |=(CLOCAL|CREAD);
    cfsetispeed(&old,B9600);
    cfsetospeed(&old,B9600);

//set the data(8bit,1 byte==char)
    old.c_cflag &= ~CSIZE;
//    old.c_cflag |= CS8|CRTSCTS;
    old.c_cflag |= CS8;
//set the stop(1bit)
    old.c_cflag &= ~CSTOPB;
//set the check(no)
    old.c_cflag &= ~PARENB;

//set timeout
    old.c_cc[VTIME]=10;
    old.c_cc[VMIN]=0;

//make the stream control no effect.hardware
//    old.c_cflag &= ~CNEW_RTSCTS;
//    old.c_cflag |= CNEW_RTSCTS;

//make the stream control no effect.software
//    old.c_iflag &= ~(IXON|IXOFF|IXANY);
//    old.c_iflag |= (IXON|IXOFF|IXANY);//effect 

//set the attr to serialport.and make the setting effect now!
    ret=tcsetattr(fd,TCSANOW,&old);
    if(ret<0)
    {
        printf("set attr fail\n");
        return -1;
    }

//write data.
//   char data[]={0x7E,0x11,0x02,0x20,0x11,0x00,0x02,0x04,0x00,0x00,0x00,0x0D};
//   *(data+8)=1099&0XFF;
//   *(data+7)=(1099>>8)&0XFF;
//    ret=write(fd,data,12);
    ret=write(fd,"AT\r",4);--------------------------------------
    if(ret<0)
    {
        printf("write fail\n");
        return -1;
    }
/**/
    close(fd);
}

void readserialport(int fd)
{
	int i=0;
    char recvbuf[128]={0};
	sleep(1);
    int ret=read(fd,recvbuf,128);
    if(ret>0)
    {
    	for(i=0;i<ret;i++)
       	{
        	 printf("%02x ",recvbuf[i]);
      	}
       	printf("\n");
    }	
}


void parse_opts(int argc,char *argv[])
{
	static const struct option opts[]=
	{
		{"devices",required_argument,NULL,'d'},
		{"read",required_argument,NULL,'r'},
		{"at",required_argument,NULL,'a'}
	};

	int c = -1;
	while((c = getopt_long(argc,argv,"d:r:",opts,NULL)) !=-1 )
	{
		if(c == -1)
			break;
		switch(c)
		{
			case 'd':
				device = optarg;
				break;
			case 'r':
				isread = optarg;	
				break;
			case 'a':
				at_cmd = optarg;	
				break;
			default:
				break;
		}
	}
}
