#include "clienttoolfun.h"



 

bool Comm_CreateThread(_HANDLE &handle ,ptrFun funName,void *funParam)
{

	bool ret = true ;

	#if defined(Q_OS_UNIX) 

	    pthread_attr_t		threadAttrib;		//线程属性

		//初始化线程属性
	    pthread_attr_init( &threadAttrib);
	
	    //设置线程绑定属性( 绑定:PTHREAD_SCOPE_SYSTEM, 非绑定:PTHREAD_SCOPE_PROCESS)
	    pthread_attr_setscope( &threadAttrib, PTHREAD_SCOPE_SYSTEM);
	
	    //设置线程分离属性( 分离:PTHREAD_CREATE_DETACHED, 非分离:PTHREAD_CREATE_JOINABLE)
	    pthread_attr_setdetachstate( &threadAttrib, PTHREAD_CREATE_DETACHED);
	
		int value  = pthread_create(&handle, &threadAttrib, funName, (void*)funParam);
		if (value != 0)	
			ret = false;

	#elif defined(Q_OS_WIN)

		handle = (_HANDLE)_beginthread (funName, 0, funParam);
		if (handle == NULL)
			ret = false;
	#endif

	return ret;


}


bool Comm_exitThread(_HANDLE &handle)
{

#if defined(Q_OS_WIN) 

	//TerminateThread((HANDLE)handle, 1L);
	//CloseHandle((HANDLE)handle);
	//handle = NULL;
	_endthread();

 
#elif defined(Q_OS_UNIX)

	pthread_exit(NULL);

#endif

	 return true;

}




 void ToolFun_TraceInfo(t_char* model,t_char *str,t_uint8 debug )
{
	if ( (debug != _DEBUG_) && (debug != _DEBUG_STR))
		return ;


	QString strInfo = "";

	if (str == NULL)
		return ;

	if (model == NULL)
	{
		 strInfo = QString("%1").arg(str);
	}
	else
	{
		strInfo = QString("%1/%2").arg(model).arg(str);
	}
	
	
   printf("%s\r\n",strInfo.toAscii().data());
	
   
}



static const QString strData[256] =
	{
		"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
		"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
		"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
		"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
		"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
		"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
		"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
		"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
		"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
		"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
		"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
		"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
		"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
		"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
		"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
		"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"

	};

 void ToolFun_TraceBirary(t_uint8 *buf,t_uint32 len,bool bSend,t_uint32 port,t_uint8 debug)
 {

	if ( (debug != _DEBUG_) && (debug != _DEBUG_MSG))
		return ;



	 QString   str    = "";

	 QDateTime dD = QDateTime::currentDateTime();
	 QString st1r= dD.toString("yyyy/MM/dd HH:mm:ss.zzz");

	if (bSend)
	{
		str = QString("%1 len:%2 port:%3 SEND:  ").arg(st1r).arg(len).arg(port);
	}
	else
	{
		str = QString("%1 len:%2 port:%3 RECV:  ").arg(st1r).arg(len).arg(port);

	}

	bool      print  = false;

	len = len>10?16:10;


	for (int i = 0 ; i < len ; i++)
	{
		quint8 value = buf[i];
		str += strData[value] + " " ;
		print = true;
		if ((i != 0) && (i % 40  == 0))
		{
			printf("%s\n",str.toAscii().data());
			str.clear();
			print = false;
		}
	}

	if (print)
	  printf("%s\n",str.toAscii().data());

 }

 
ulong Comm_GetTickCount()
{
	#if defined(Q_OS_UNIX) 
		
		int				fd;
		unsigned long	sec,usec;
		char			buf[64];

		fd = open("/proc/uptime",O_RDONLY);

		if (fd == -1)
		{
			perror("open");
			return 0;
		}

		memset(buf,0,64);
		read(fd,buf,64);
		sscanf(buf,"%ld.%ld",&sec,&usec);

		close( fd);

		return (sec * 1000 + usec);


	#elif defined(Q_OS_WIN)
		
		return GetTickCount();
	
	#endif

}  


