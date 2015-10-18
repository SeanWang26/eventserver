#ifndef JNTYPE_H
#define JNTYPE_H

struct st_jn_sub_equ
{
	int 	SubEquId;
	int 	EquId;
	int		CtlgId;
	char 	SubName[64];
	int 	SubSeq;
	int 	SubType;
	int 	SpecificType;
	int 	HasPTZ;
	int 	HasLamp;
	int 	HasDemist;
	int 	HasRainBrush;
	char	Extend[256];

	int nOpSeq;
	int nOpType;
	int nOpResult;
};

struct st_jn_equ
{
	int 	EquId;
	char 	FactoryName[64];
	char 	EquName[64];
	char 	IP[256];
	int 	Port;
	char 	UserName[64];
	char 	Password[64];
	int 	ChannelNum;
	int		InputNum;
	int		OutputNum;
	char	Extend[256];

	int		nOpSeq;
	int		nOpType;
	int		nOpResult;

	//when add main equ, the system add sub equ automatically
	int     nCtlgId;
	
	struct st_jn_sub_equ *stSubEqu;
	int                stSubEquCnt;
};

/*struct stOpCmd
{
	int		nOpSeq;
	int		nOpType;
	int		nOpResult;

	union
	{
		struct st_sub_equ;
		struct st_jn_equ;
	};
};*/







//释放内存
#define FREE_MEMORY_JN_ADD_EQU                   3001    //jn类型的内存释放
#define FREE_MEMORY_JN_GET_EQU_INFO              3002    //jn类型的内存释放
#define FREE_MEMORY_JN_GET_SUB_EQU_INFO          3003    //jn类型的内存释放
#define FREE_MEMORY_JN_GET_ENCODE_CONFIG         3004    //jn类型的内存释放











#endif
