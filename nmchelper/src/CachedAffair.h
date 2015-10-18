#ifndef CACHEDAFFAIR_H
#define CACHEDAFFAIR_H


#if (defined(WIN32) || defined(WIN64))
#include <Windows.h>
#else

#include <pthread.h>
#include <tr1/memory>
#endif

#include <stdlib.h>

#include <string>
#include <map>
#include <list>


//#include "../jtprintf.h"
using namespace std;

#include "COMM_HEADER.h"

#if (defined(WIN32) || defined(WIN64))

#include <Windows.h>

#define JT_AMIOC_ADD(NUM,ONE) InterlockedExchangeAdd(&NUM,ONE);

#else

#define JT_AMIOC_ADD(NUM,ONE) __sync_add_and_fetch(&NUM,ONE);

#endif

struct ST_AFFAIR_CALLBACK
{
public:
    typedef int (*OnGotData)(long cookie, unsigned char* pData, int dataLen);
    typedef int (*OnAffairOverTime)(long cookie);

    ST_AFFAIR_CALLBACK (long cookie
                        , OnGotData pOnGotData = NULL//收到数据的回调函�?
                        , OnAffairOverTime pOnAffairOverTime = NULL//事务超时回调函数
                        , int overTime = 6000)//超时时间,毫秒为单�?
        :m_dwUserCookie(cookie),m_pOnGotData(pOnGotData),m_pOnAffairOverTime(pOnAffairOverTime)
        ,m_nOverTime(overTime)
    {};
	
    ~ST_AFFAIR_CALLBACK()
    {}

    long					m_dwUserCookie;
    OnGotData				m_pOnGotData;
    OnAffairOverTime		m_pOnAffairOverTime;
    int						m_nOverTime; //����

	string					m_pRecvData;
    //unsigned char*			m_pRecvData;
    //int						m_pRecvSize;
};


class CCachedAffairMapLock
{
#if (defined(WIN32) || defined(WIN64))
	CRITICAL_SECTION								m_simpleCS;
#else
	pthread_mutex_t                     			m_simpleCS;
#endif
public:
	CCachedAffairMapLock();
	~CCachedAffairMapLock();

	void LockMap();
	void UnLockMap();
};



class CCachedAffair
{
private:
	///////static int gCachedAffairId;
	///////int m_CachedAffairId;
public:

#if (defined(WIN32) || defined(WIN64))

	HANDLE										        m_handleOverTime;
	CRITICAL_SECTION									m_ConditionMutex;

#else

	pthread_cond_t										m_handleOverTime;
	pthread_mutex_t                             		m_ConditionMutex;

#endif

	string												m_pRecvDataBuf;
	unsigned int										m_recvDataCanceled;

    CCachedAffair(ST_AFFAIR_CALLBACK* pAffairCallBack);
    virtual ~CCachedAffair(void);
    void SetCommandIdxPair(int outCmdIdx, int exptedRecvCmdIdx);
    void SetSequencePair(long outSeqIdx);
    int DoOverTime();
    int DoOnGotData(unsigned char* pData, int dataLen);

    ST_AFFAIR_CALLBACK::OnGotData				m_fpOnGotData;
    ST_AFFAIR_CALLBACK::OnAffairOverTime		m_fpOnAffairOverTime;
    long         								m_dwUserCookie;
    unsigned int								m_dwPushedTime;
    int											m_nOverTime;
    int											m_nOutCmdIdx;
    int											m_nExptedRecvCmdIdx;
    long										m_nOutSeqIdx;
    unsigned int								m_dwDevId;
    bool										m_bOverTime;
};

class CCachedAffairMap
{
private:
    ///map<unsigned long long, tr1::shared_ptr<CCachedAffair> > m_cachedAffairMap;
    static unsigned long long								m_lastSeq;

    template <class K,class V>
    class CBeOverTime
    {
    public:
        CBeOverTime(unsigned int nowTime)
            :m_dwNowTime(nowTime){}
        bool operator() (const pair<unsigned long long, tr1::shared_ptr<CCachedAffair> > &rhs)
        {
            tr1::shared_ptr<CCachedAffair> pSelf = rhs.second ;
            if (!pSelf)
                return false;

            return rhs.second ->m_dwPushedTime + rhs.second ->m_nOverTime < m_dwNowTime ? true : false ;
        }
    private:
        unsigned int			m_dwNowTime;
    };

    template <class K,class V>
    class CBeExpectedDataComing
    {
    public:
        CBeExpectedDataComing(int recvCmdIdx,int recvSeqIdx)
            : m_nRecvCmdIdx(recvCmdIdx),m_nRecvSeqIdx(recvSeqIdx){}
        bool operator() (const pair<unsigned long long,tr1::shared_ptr<CCachedAffair> > &rhs)
        {
            tr1::shared_ptr<CCachedAffair> pAffairItem = rhs.second ;
            if (!pAffairItem)
                return false;

			//return (pAffairItem->m_nExptedRecvCmdIdx == m_nRecvCmdIdx ) ? true : false;
            return pAffairItem->m_nOutSeqIdx == m_nRecvSeqIdx;
        }
    private:
        int				m_nRecvCmdIdx;
        int				m_nRecvSeqIdx;
    };
protected:
    CCachedAffairMap();
    ~CCachedAffairMap();
public:

	//static int Static_PushNewAffair(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
	//	, ST_AFFAIR_CALLBACK* pAffairCallBack, long nOutSeqId, int nOutCmdId, int nExpetedCmdId);

	static unsigned long long Static_PushNewAffair_Pre(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		, tr1::shared_ptr<CCachedAffair> Affair, long nOutSeqId, int nOutCmdId, int nExpetedCmdId);

	static unsigned long long Static_CancelAffair_Pre(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		, unsigned long long AffairId);
	
	static int Static_PushNewAffair(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		, tr1::shared_ptr<CCachedAffair> Affair, unsigned long long AffairId);//, long nOutSeqId, int nOutCmdId, int nExpetedCmdId

	static int Static_CheckBeOverTime(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock, long nowTime);
	static int Static_CheckBeExptedData(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		, unsigned char* pData, long dataLen);
	static int Static_ClearCachedItem(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock);
};

#endif // CACHEDAFFAIR_H
