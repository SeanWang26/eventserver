#ifndef COMM_HEADER_H
#define COMM_HEADER_H

//�ⲿ����̶�ͷ���ṹ
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

	unsigned int				nSrcType;	//Դ������
	unsigned int				nCmdType;	//�����������
	unsigned int				nCmdSeq;	//�������
	unsigned int				datalen;	//��չ���ݴ�С
};

#endif
