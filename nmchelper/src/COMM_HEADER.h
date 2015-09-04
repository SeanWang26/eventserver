#ifndef COMM_HEADER_H
#define COMM_HEADER_H

//外部信令固定头部结构
/*#pragma pack(push,1)
typedef struct stExternalCommand
{
	stExternalCommand()
	{
		static unsigned int stExternalCommandSeq = 0;
		nCmdSeq = ++stExternalCommandSeq;
		datalen = 0;
	}

	unsigned int 	nSrcType;
	unsigned int 	nCmdType;
	unsigned int 	nCmdSeq;
	unsigned int 	datalen; //nContentSize;
}ExCommand;
#pragma pack(pop)*/

struct ST_ICMS_CMD_HEADER
{
	ST_ICMS_CMD_HEADER()
	{
		static unsigned int stExternalCommandSeq = 0;
		nCmdSeq = ++stExternalCommandSeq;
		datalen = 0;
	}

	unsigned int				nSrcType;	//源的类型
	unsigned int				nCmdType;	//信令的子类型
	unsigned int				nCmdSeq;	//信令序号
	unsigned int				datalen;	//扩展数据大小
};

#endif
