#ifndef FRAMEPKG_H
#define FRAMEPKG_H

#include <stdint.h>
#include <stdlib.h>


#include "devicetype.h"

//class OnGetFrameData;

#include "COMM_HEADER.h"
#include <string.h>

struct bufinf
{
	unsigned char* _buf;
	int            _buflen;
	int            _bufusedlen;
};

class OnGetFrameCallBack
{
public:
	virtual int OnGetFrame(unsigned char* Data, int len)=0;
	//virtual int OnMakeFramePkg(unsigned char* Data, int Len, int HeadLen)=0; //复制到一块buffer了，修改头部
};

template<class CHeadType>
class CFramePkg
{
private:
	struct bufinf m_framebuf;
	OnGetFrameCallBack *m_CallBack;
public:
	CFramePkg()
	{
		m_CallBack = NULL;
		m_framebuf._buflen = 1024*1024*2;
		m_framebuf._buf = (unsigned char*)malloc(m_framebuf._buflen);
		m_framebuf._bufusedlen = 0;
	}
	~CFramePkg()
	{
		free(m_framebuf._buf);
		m_framebuf._bufusedlen = 0;
		m_framebuf._buflen = 0;
	}

	int Init()
	{
		m_framebuf._bufusedlen = 0;
		return 0;
	}
	
	void SetCallBack(OnGetFrameCallBack *CallBack)
	{
		m_CallBack = CallBack;
	}
	int Receive(unsigned char* buf, int len)
	{
		//jtprintf("[%s]nmc len %d\n", __FUNCTION__, len);
		//还要判断一哈是否超出大小！！！！！
		if((m_framebuf._bufusedlen+len)>m_framebuf._buflen)
		{
			//jtprintf("[%s]1\n", __FUNCTION__);
			m_CallBack->OnGetFrame(NULL, 0);
			return -1;
		}

		memcpy(m_framebuf._buf+m_framebuf._bufusedlen, buf, len);
		m_framebuf._bufusedlen += len;

		while(m_framebuf._bufusedlen>0)
		{
			if(m_framebuf._bufusedlen < (int)sizeof(CHeadType))//
			{
				//小于包头
				//jtprintf("[%s]2\n", __FUNCTION__);
				return len;
			}

			CHeadType *stream_data = (CHeadType *)m_framebuf._buf;
			if(m_framebuf._bufusedlen < (int)(stream_data->datalen + sizeof(CHeadType)))
			{
				//使用量小于包头加数据部分
				//jtprintf("[%s]3\n", __FUNCTION__);
				return len;
			}

			//jtprintf("[%s]4\n", __FUNCTION__);
			//已大于包头加数据部分
			if(m_CallBack)
			{
				m_CallBack->OnGetFrame(m_framebuf._buf, stream_data->datalen+sizeof(CHeadType));
			}

			int leftlen = m_framebuf._bufusedlen - stream_data->datalen - sizeof(CHeadType);

			//移动到前面
			if(leftlen > 0)
			{
				memmove(m_framebuf._buf, m_framebuf._buf+stream_data->datalen+sizeof(CHeadType)
					, leftlen);
			}

			m_framebuf._bufusedlen = leftlen;
		}

		return 0;
	}

	CHeadType* MakeFramePkg(unsigned char* buf, int len)
	{
		CHeadType Head;
		memcpy(m_framebuf._buf + m_framebuf._bufusedlen, &Head, sizeof(CHeadType));
		m_framebuf._bufusedlen += sizeof(CHeadType);
		
		memcpy(m_framebuf._buf + m_framebuf._bufusedlen, buf, len);
		m_framebuf._bufusedlen += len;

		Head.datalen = len;

		//m_CallBack->OnMakeFramePkg(m_framebuf._buf, m_framebuf._bufusedlen, sizeof(CHeadType));

		return (CHeadType*)m_framebuf._buf;
	}

	uint8_t* GetData() { return m_framebuf._buf; }
	uint32_t GetDataLen() { return m_framebuf._bufusedlen; }
	
};

#endif
