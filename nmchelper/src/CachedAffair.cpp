#include "StdAfx.h"

#if (defined(WIN32) || defined(WIN64))
#else

#include <sys/time.h>

#endif

#include <string.h>
#include <time.h>
#include <algorithm>


#include <errno.h>

#include "COMM_HEADER.h"
#include "CachedAffair.h"
#include "jtprintf.h"

void CCachedAffairMapLock::LockMap()
{
#if (defined(WIN32) || defined(WIN64))
	EnterCriticalSection(&m_simpleCS);
#else
	pthread_mutex_lock(&m_simpleCS);
#endif	
}
void CCachedAffairMapLock::UnLockMap()
{
#if (defined(WIN32) || defined(WIN64))
	LeaveCriticalSection(&m_simpleCS);
#else
	pthread_mutex_unlock(&m_simpleCS);
#endif	
}
CCachedAffairMapLock::CCachedAffairMapLock()
{
#if (defined(WIN32) || defined(WIN64))
	InitializeCriticalSection(&m_simpleCS);
#else
	pthread_mutex_init(&m_simpleCS,NULL);
#endif
}

CCachedAffairMapLock::~CCachedAffairMapLock()
{
#if (defined(WIN32) || defined(WIN64))
	DeleteCriticalSection (&m_simpleCS);
#else
	pthread_mutex_destroy(&m_simpleCS);
#endif	
}


///int CCachedAffair::gCachedAffairId=0;

CCachedAffair::CCachedAffair(ST_AFFAIR_CALLBACK* pAffairCallBack)
:m_recvDataCanceled(0)//m_CachedAffairId(++gCachedAffairId),
,m_fpOnGotData(NULL),m_fpOnAffairOverTime(NULL),m_dwUserCookie(0),m_dwPushedTime(time(0))
,m_nOverTime(1000),m_nOutCmdIdx(0),m_nExptedRecvCmdIdx(0),m_nOutSeqIdx(0)//,m_pRecvDataBuf(NULL),m_recvDataSize(0)

{
	//m_ConditionMutex=;
	//m_handleOverTime=PTHREAD_COND_INITIALIZER;
    if (pAffairCallBack && pAffairCallBack->m_pOnGotData && pAffairCallBack->m_pOnAffairOverTime)
    {
		//jtprintf("[%s]new Id %d, Asyn\n", __FUNCTION__, m_CachedAffairId);
        m_fpOnGotData			= pAffairCallBack->m_pOnGotData;
        m_fpOnAffairOverTime	= pAffairCallBack->m_pOnAffairOverTime;
        m_dwUserCookie			= pAffairCallBack->m_dwUserCookie;
        m_nOverTime				= pAffairCallBack->m_nOverTime;
    }
    else
    {
    	//jtprintf("[%s]new Id %d, Syn\n", __FUNCTION__, m_CachedAffairId);

		m_dwUserCookie			= pAffairCallBack->m_dwUserCookie;
        m_nOverTime				= pAffairCallBack->m_nOverTime;
		
#if (defined(WIN32) || defined(WIN64))
		m_handleOverTime		= CreateEvent(NULL,TRUE,FALSE,NULL);
#else
		pthread_cond_init(&m_handleOverTime,NULL);
		pthread_mutex_init(&m_ConditionMutex,NULL);
#endif

    }
}

CCachedAffair::~CCachedAffair(void)
{
	//jtprintf("[%s]1\n", __FUNCTION__, m_CachedAffairId);

    //try
   	{
	    /*if (NULL != m_pRecvDataBuf)
	    {
	    	jtprintf("[%s] free(m_pRecvDataBuf)\n", __FUNCTION__);
	        free(m_pRecvDataBuf);
	        m_pRecvDataBuf	= NULL;
	    }*/

		if (m_fpOnGotData && m_fpOnAffairOverTime)
		{
			//jtprintf("[%s]delete Id %d, Asyn\n", __FUNCTION__, m_CachedAffairId);
		}
		else
		{
			//jtprintf("[%s]delete Id %d, Syn\n", __FUNCTION__, m_CachedAffairId);

			m_recvDataCanceled = true;

#if (defined(WIN32) || defined(WIN64))
			SetEvent(m_handleOverTime);

			//wait .....???
			if (m_handleOverTime) 
				CloseHandle(m_handleOverTime);
			m_handleOverTime = NULL;
#else
			pthread_cond_signal(&m_handleOverTime);

			//wait .....???

			pthread_mutex_destroy(&m_ConditionMutex);
			pthread_cond_destroy(&m_handleOverTime);
#endif
		}
    }
	//catch(...)
	{

	}
}

int CCachedAffair::DoOverTime()
{
	//try
   	{
	    if (m_fpOnAffairOverTime)
	        m_fpOnAffairOverTime(m_dwUserCookie);

	    return 1;
	}
	//catch(...)
	{

	}
    return -1;
}

int CCachedAffair::DoOnGotData(unsigned char* pData, int dataLen)
{
	//try
   	{
	    if (m_fpOnGotData && m_fpOnAffairOverTime)//  m_fpOnGotData不为0，为异步
	    {
	    	//异步
	        m_fpOnGotData(m_dwUserCookie,pData,dataLen);
	        return 1;
	    }
	    else
	    {
//同步
#if (defined(WIN32) || defined(WIN64))

#else
			pthread_mutex_lock(&m_ConditionMutex);
#endif	
	        if(m_recvDataCanceled)
	        {
#if (defined(WIN32) || defined(WIN64))

#else
				pthread_mutex_unlock(&m_ConditionMutex);
#endif
	            return 3;
	        }

			if(dataLen)
				m_pRecvDataBuf.assign((char*)pData, dataLen);
	       	
			//得到数据，发送信号
#if (defined(WIN32) || defined(WIN64))
			SetEvent(m_handleOverTime);
#else
			pthread_mutex_unlock(&m_ConditionMutex);
			pthread_cond_signal(&m_handleOverTime);
#endif						
	        return 2;
	    }
	    return 1;
	}
	///catch(...)
	{

	}

    return -1;
}

void CCachedAffair::SetCommandIdxPair(int outCmdIdx,int exptedRecvCmdIdx)
{
    m_nOutCmdIdx			= outCmdIdx;
    m_nExptedRecvCmdIdx		= exptedRecvCmdIdx;
}

void CCachedAffair::SetSequencePair(long outSeqIdx)
{
    m_nOutSeqIdx			= outSeqIdx;
}

unsigned long long  CCachedAffairMap::m_lastSeq = 1;
CCachedAffairMap::CCachedAffairMap()
{

}

CCachedAffairMap::~CCachedAffairMap()
{

}


unsigned long long CCachedAffairMap::Static_PushNewAffair_Pre(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
	, tr1::shared_ptr<CCachedAffair> pCachedAffairItem, long nOutSeqId, int nOutCmdId, int nExpetedCmdId)//
{
	if(((!!(pCachedAffairItem->m_fpOnGotData)) ^ (!!(pCachedAffairItem->m_fpOnAffairOverTime))))
	{
		//同时有，同时没有才正确
		return -2;
	}

	pCachedAffairItem->SetSequencePair(nOutSeqId);
	pCachedAffairItem->SetCommandIdxPair(nOutCmdId,nExpetedCmdId);

	//unsigned long long AffairId=0;
	{
		//缩小加锁范围
		///AffairId = nOutSeqId;//JT_AMIOC_ADD(m_lastSeq,1);//使用外部的nOutSeqId，因为处理时可能涉及批量
		//if(AffairId==0)
		//	AffairId = JT_AMIOC_ADD(m_lastSeq,1);

		Lock.LockMap();
		cachedMap[nOutSeqId] = pCachedAffairItem;
		Lock.UnLockMap();
	}

	return nOutSeqId;
}
unsigned long long CCachedAffairMap::Static_CancelAffair_Pre(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		, unsigned long long AffairId)
{
	{
		//缩小加锁范围
		Lock.LockMap();
		cachedMap.erase(AffairId);
		Lock.UnLockMap();	
	}
	return AffairId;
}

#if (defined(WIN32) || defined(WIN64))
int CCachedAffairMap::Static_PushNewAffair(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& m_cachedAffairMap, CCachedAffairMapLock &Lock, 
	tr1::shared_ptr<CCachedAffair> pCachedAffairItem, unsigned long long AffairId)
{
/*
	if(((!!(pCachedAffairItem->m_fpOnGotData)) ^ (!!(pCachedAffairItem->m_fpOnAffairOverTime))))
	{
		//同时有，同时没有才正确
		//printf("[%s]---sss-----------\n", __FUNCTION__);
		return -2;
	}
*/
	try
	{
		//默认事物超时时间为1秒
		//int nOverTimeSecond	= pAffairCallBack->m_nOverTime/1000;
		
		////tr1::shared_ptr<CCachedAffair> pCachedAffairItem(new CCachedAffair(pAffairCallBack));
		//if (!pCachedAffairItem)
		///	return -2;

		if(pCachedAffairItem->m_fpOnGotData)
		{
			//是异步的
			return 1;
		}
		else
		{
			//是同步的，等待事件返回

			//条件变量界限开始
			int nOverTimeMSecond = pCachedAffairItem->m_nOverTime;//%1000;

			DWORD res = 0;
			if(nOverTimeMSecond==-1)
				res = WaitForSingleObject(pCachedAffairItem->m_handleOverTime, INFINITE);
			else
				res = WaitForSingleObject(pCachedAffairItem->m_handleOverTime, nOverTimeMSecond);

			//jtprintf("[%s]pthread_cond_timedwait after, %d, %d\n", __FUNCTION__, res, errno);
			if(res==WAIT_OBJECT_0)
			{
				//同步接收到数据
				//jtprintf("[%s]pthread_cond_timedwait, %d, get data  %p, %d\n", __FUNCTION__, res, pCachedAffairItem->m_pRecvDataBuf, pCachedAffairItem->m_recvDataSize);
				if(!pCachedAffairItem->m_recvDataCanceled
					&& pCachedAffairItem->m_pRecvDataBuf.size())
				{
					//pAffairCallBack->m_pRecvData  = pCachedAffairItem->m_pRecvDataBuf;
				}	

				{
					Lock.LockMap();
					m_cachedAffairMap.erase(AffairId);
					Lock.UnLockMap();
				}

				return 0;
			}
			else if(res==WAIT_TIMEOUT)
			{
				//jtprintf("[%s]res==ETIMEDOUT, %d\n", __FUNCTION__, res);
				//等待超时，也要检查是否有数据了
				if(pCachedAffairItem->m_pRecvDataBuf.size())
				{
					//超时，但是有数据
					////pAffairCallBack ->m_pRecvData  = pCachedAffairItem->m_pRecvDataBuf;
					
					{
						Lock.LockMap();
						m_cachedAffairMap.erase(AffairId);
						Lock.UnLockMap();
					}

					return 0;
				}

				pCachedAffairItem->m_recvDataCanceled = true;
			}
			else
			{
				//jtprintf("[%s]pthread_cond_timedwait %s, %s\n", __FUNCTION__, res, strerror(errno));
				pCachedAffairItem->m_recvDataCanceled = true;
			}

			//系统错误
			Lock.LockMap();
			m_cachedAffairMap.erase(AffairId);
			Lock.UnLockMap();

			return -3;
		}

		return 0;
	}
	catch(...)
	{
		jtprintf("[%s]catch---------------------------------------------\n", __FUNCTION__);
	}

	return -1;
}

#else

int CCachedAffairMap::Static_PushNewAffair(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock, 
	tr1::shared_ptr<CCachedAffair> pCachedAffairItem, unsigned long long AffairId)
{
	//if (NULL == pAffairCallBack)
	//	return -1;
	/*
	if(((!!pAffairCallBack->m_pOnGotData) ^ (!!pAffairCallBack->m_pOnAffairOverTime)))
	{
		//同时有，同时没有才正确
		//printf("[%s]---sss-----------\n", __FUNCTION__);
		return -2;
	}
	*/
	
	//try
	{
		if(pCachedAffairItem->m_fpOnGotData)
		{
			//是异步的
			return 1;
		}
		else
		{
			//是同步的，等待事件返回
			jtprintf("[%s]nOverTimeSecond %d\n", __FUNCTION__, pCachedAffairItem->m_nOverTime);

			struct timespec outtime;
			if(pCachedAffairItem->m_nOverTime!=-1)
			{
				int nOverTimeSecond	= pCachedAffairItem->m_nOverTime/1000;
				int nOverTimeMSecond = pCachedAffairItem->m_nOverTime%1000;

				struct timeval now;
				
				gettimeofday(&now, NULL);
				clock_gettime(CLOCK_REALTIME, &outtime);
				outtime.tv_sec = now.tv_sec + nOverTimeSecond;
				outtime.tv_nsec = now.tv_usec + nOverTimeMSecond * 1000;
			}
			
			//条件变量界限开始
			pthread_mutex_lock(&pCachedAffairItem->m_ConditionMutex);

			//jtprintf("[%s]pthread_cond_timedwait before %d %d, %ld\n", __FUNCTION__, nOverTimeSecond, nOverTimeMSecond, outtime.tv_sec);
			if(pCachedAffairItem->m_pRecvDataBuf.size())
			{
				//还未等待就已经得到数据了
				pthread_mutex_unlock(&pCachedAffairItem->m_ConditionMutex);

				{
					Lock.LockMap();
					cachedMap.erase(AffairId);
					Lock.UnLockMap();
				}

				return 0;
			}
			
			int res = 0;
			if(pCachedAffairItem->m_nOverTime!=-1)
				res = pthread_cond_timedwait(&(pCachedAffairItem->m_handleOverTime), &(pCachedAffairItem->m_ConditionMutex), &outtime);
			else
				res = pthread_cond_wait(&(pCachedAffairItem->m_handleOverTime), &(pCachedAffairItem->m_ConditionMutex));
			
			//jtprintf("[%s]pthread_cond_timedwait after, %d, %d\n", __FUNCTION__, res, errno);
			if(res==0)
			{
				//同步接收到数据
				//jtprintf("[%s]pthread_cond_timedwait, %d, get data  %p, %d\n", __FUNCTION__, res, pCachedAffairItem->m_pRecvDataBuf, pCachedAffairItem->m_recvDataSize);
				if(//!pCachedAffairItem->m_recvDataCanceled&& 
					pCachedAffairItem->m_pRecvDataBuf.size())
				{
					///pAffairCallBack->m_pRecvData  = pCachedAffairItem->m_pRecvDataBuf;
				}

				pthread_mutex_unlock(&pCachedAffairItem->m_ConditionMutex);

				{
					Lock.LockMap();
					cachedMap.erase(AffairId);
					Lock.UnLockMap();
				}

				return 0;
			}
			else if(res==ETIMEDOUT)
			{
				//等待超时，也要检查是否有数据了
				jtprintf("[%s]res==ETIMEDOUT, size %d, timeout %d\n"
						, __FUNCTION__, pCachedAffairItem->m_pRecvDataBuf.size(), pCachedAffairItem->m_nOverTime);
				if(pCachedAffairItem->m_pRecvDataBuf.size())
				{
					//超时，但是有数据
					////pAffairCallBack ->m_pRecvData  = pCachedAffairItem->m_pRecvDataBuf;

					pthread_mutex_unlock(&pCachedAffairItem->m_ConditionMutex);
					{
						Lock.LockMap();
						cachedMap.erase(AffairId);
						Lock.UnLockMap();
					}

					return 0;
				}

				pthread_mutex_unlock(&pCachedAffairItem->m_ConditionMutex);

				pCachedAffairItem->m_recvDataCanceled = true;

				return -2;
			}
			else
			{
				//jtprintf("[%s]pthread_cond_timedwait %s, %s\n", __FUNCTION__, res, strerror(errno));
				pCachedAffairItem->m_recvDataCanceled = true;
			}

			pthread_mutex_unlock(&pCachedAffairItem->m_ConditionMutex);

			Lock.LockMap();
			cachedMap.erase(AffairId);
			Lock.UnLockMap();

			return -3;
		}

		return 0;
	}
	//catch(...)
	{
		jtprintf("[%s]catch\n", __FUNCTION__);
	}

	return -1;
}
#endif	
int CCachedAffairMap::Static_CheckBeExptedData(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& m_cachedAffairMap, CCachedAffairMapLock &Lock
	, unsigned char* pData, long dataLen)
{
	///try
	{

	    ST_ICMS_CMD_HEADER* pCmdHeader = (ST_ICMS_CMD_HEADER*)pData;
	    if(!pCmdHeader) //|| IsBadReadPtr(pCmdHeader,sizeof(ST_ICMS_CMD_HEADER)))
	        return -1;

	    tr1::shared_ptr<CCachedAffair> pAffairItem;
	    {
			Lock.LockMap();

			map<unsigned long long,tr1::shared_ptr<CCachedAffair> >::iterator iter = find_if (m_cachedAffairMap.begin (),m_cachedAffairMap.end ()
				, CBeExpectedDataComing<unsigned long long, tr1::shared_ptr<CCachedAffair> > (pCmdHeader->nCmdType, pCmdHeader->nCmdSeq));
			if (iter != m_cachedAffairMap.end ())
			{
				pAffairItem = iter->second;
				m_cachedAffairMap.erase (iter);
			}

			Lock.UnLockMap();
	    }

	    if(pAffairItem)
	    {
	    	pAffairItem->DoOnGotData(pData,dataLen);
	    }

	    return 1;
	}
	///catch(...)
	{

	}

    return -1;
}

int CCachedAffairMap::Static_CheckBeOverTime(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock, long nowTime)
{
	//只处理异步的超时
	
	//try
	{
		list<tr1::shared_ptr<CCachedAffair> >				listNeedRemove ;
	    {
			//先将超时的条目移除
			Lock.LockMap();		
#if 0
	        if (cachedMap.size () < 1)
			{
				Lock.UnLockMap();
	            return 1;
			}

	        map<unsigned long long, tr1::shared_ptr<CCachedAffair> >::iterator iter 
				= find_if (cachedMap.begin(), cachedMap.end (), CCachedAffairMap::CBeOverTime<unsigned long long,CCachedAffair*> (nowTime));
	        while (iter != cachedMap.end ())
	        {
	            tr1::shared_ptr<CCachedAffair> pAffairItem = iter->second;
	            listNeedRemove.push_back (pAffairItem);
	            cachedMap.erase(iter);

	            ++iter;
	            if (iter == cachedMap.end ())
	                break;
				
	            iter = find_if (iter ,cachedMap.end (),CCachedAffairMap::CBeOverTime<unsigned long long,CCachedAffair*> (nowTime));
	        }
#else

#endif
			for(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >::iterator iter = cachedMap.begin();iter!=cachedMap.end();++iter)
			{
				if((iter->second ->m_dwPushedTime + iter->second ->m_nOverTime) < nowTime)
				{
					tr1::shared_ptr<CCachedAffair> pAffairItem = iter->second;
					listNeedRemove.push_back (pAffairItem);
					cachedMap.erase(iter);

					++iter;
					if (iter == cachedMap.end ())
						break;
				}
			}

			Lock.UnLockMap();
	    }

		//处理超时的条目
	    list<tr1::shared_ptr<CCachedAffair> >::iterator iterRemove = listNeedRemove.begin () ;
	    while (iterRemove != listNeedRemove.end ())
	    {
	    	(*iterRemove)->DoOverTime();
	    }

	    return 1;
	}
	//catch(...)
	{

	}

    return -1;
}
int CCachedAffairMap::Static_ClearCachedItem(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock)
{
    //try
	{
		Lock.LockMap();	
		cachedMap.clear();
		Lock.UnLockMap();
	    return 1;
	}
	//catch(...)
	{

	}

    return -1;
}

