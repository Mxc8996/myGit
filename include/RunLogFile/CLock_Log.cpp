#include "CLock.h"

CMutex::CMutex() 
{
	pthread_mutex_init(&m_mutex, NULL);
}

CMutex::~CMutex() 
{
	pthread_mutex_destroy(&m_mutex);
}

void CMutex::Lock() const 
{
	pthread_mutex_lock(&m_mutex);
}

void CMutex::UnLock() const
{
	pthread_mutex_unlock(&m_mutex);
}

CMyLock::CMyLock(const ILock& m)
	: m_lock(m)
{
	m_lock.Lock();
}

CMyLock::~CMyLock() 
{
	m_lock.UnLock();
}
