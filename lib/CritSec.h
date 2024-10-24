#ifndef _CRITSEC_H
#define _CRITSEC_H

#include "Prefastdef.h"

// wrapper for whatever critical section we have
namespace WinCrashpad {
class CCritSec {
  // make copy constructor and assignment operator inaccessible

  CCritSec(const CCritSec& refCritSec);
  CCritSec& operator=(const CCritSec& refCritSec);

  CRITICAL_SECTION m_CritSec;

 public:
  CCritSec();

  ~CCritSec();

  void Lock();

  void Unlock();
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock {
  // make copy constructor and assignment operator inaccessible

  CAutoLock(const CAutoLock& refAutoLock);
  CAutoLock& operator=(const CAutoLock& refAutoLock);

 protected:
  CCritSec* m_pLock;

 public:
  CAutoLock(__in CCritSec* plock);

  ~CAutoLock();
};
}  // namespace WinCrashpad

#endif  //_CRITSEC_H
