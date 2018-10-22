#pragma once

#include <pthread.h>

class ILock 
{
public:
	virtual ~ILock() {};
	virtual void Lock() const = 0;
	virtual void UnLock() const = 0;
};

class CMutex : public ILock 
{
public:
	CMutex();
	~CMutex();

	virtual void Lock() const;
	virtual void UnLock() const;

private:
	mutable pthread_mutex_t m_mutex;
};

class CMyLock 
{
public:
	CMyLock(const ILock&);
	~CMyLock();

private:
	const ILock& m_lock;
};


