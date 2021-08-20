/*
 *  Copyright (c) 2011 Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_CALLS_TCC__
#define __UNISIM_UTIL_OS_LINUX_CALLS_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/files_flags.hh>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
#include <process.h>
#include <windows.h>
#else
#include <sys/times.h>
#include <sys/time.h>
#endif

#include <string.h>
#include <assert.h>
#include <fstream>
#include <stdexcept>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

using unisim::util::endian::Host2Target;
using unisim::util::endian::Target2Host;

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::HostToLinuxErrno(int host_errno)
{
  switch (host_errno)
    {
    default: return LINUX_EINVAL;
#ifdef EPERM
    case EPERM: return LINUX_EPERM;
#endif
#ifdef ENOENT
    case ENOENT: return LINUX_ENOENT;
#endif
#ifdef ESRCH
    case ESRCH: return LINUX_ESRCH;
#endif
#ifdef EINTR
    case EINTR: return LINUX_EINTR;
#endif
#ifdef EIO
    case EIO: return LINUX_EIO;
#endif
#ifdef ENXIO
    case ENXIO: return LINUX_ENXIO;
#endif
#ifdef E2BIG
    case E2BIG: return LINUX_E2BIG;
#endif
#ifdef ENOEXEC
    case ENOEXEC: return LINUX_ENOEXEC;
#endif
#ifdef EBADF
    case EBADF: return LINUX_EBADF;
#endif
#ifdef ECHILD
    case ECHILD: return LINUX_ECHILD;
#endif
#ifdef EAGAIN
    case EAGAIN: return LINUX_EAGAIN;
#endif
#ifdef ENOMEM
    case ENOMEM: return LINUX_ENOMEM;
#endif
#ifdef EACCES
    case EACCES: return LINUX_EACCES;
#endif
#ifdef EFAULT
    case EFAULT: return LINUX_EFAULT;
#endif
#ifdef ENOTBLK
    case ENOTBLK: return LINUX_ENOTBLK;
#endif
#ifdef EBUSY
    case EBUSY: return LINUX_EBUSY;
#endif
#ifdef EEXIST
    case EEXIST: return LINUX_EEXIST;
#endif
#ifdef EXDEV
    case EXDEV: return LINUX_EXDEV;
#endif
#ifdef ENODEV
    case ENODEV: return LINUX_ENODEV;
#endif
#ifdef ENOTDIR
    case ENOTDIR: return LINUX_ENOTDIR;
#endif
#ifdef EISDIR
    case EISDIR: return LINUX_EISDIR;
#endif
#ifdef EINVAL
    case EINVAL: return LINUX_EINVAL;
#endif
#ifdef ENFILE
    case ENFILE: return LINUX_ENFILE;
#endif
#ifdef EMFILE
    case EMFILE: return LINUX_EMFILE;
#endif
#ifdef ENOTTY
    case ENOTTY: return LINUX_ENOTTY;
#endif
#ifdef ETXTBSY
    case ETXTBSY: return LINUX_ETXTBSY;
#endif
#ifdef EFBIG
    case EFBIG: return LINUX_EFBIG;
#endif
#ifdef ENOSPC
    case ENOSPC: return LINUX_ENOSPC;
#endif
#ifdef ESPIPE
    case ESPIPE: return LINUX_ESPIPE;
#endif
#ifdef EROFS
    case EROFS: return LINUX_EROFS;
#endif
#ifdef EMLINK
    case EMLINK: return LINUX_EMLINK;
#endif
#ifdef EPIPE
    case EPIPE: return LINUX_EPIPE;
#endif
#ifdef EDOM
    case EDOM: return LINUX_EDOM;
#endif
#ifdef ERANGE
    case ERANGE: return LINUX_ERANGE;
#endif
#ifdef EDEADLK
    case EDEADLK: return LINUX_EDEADLK;
#endif
#ifdef ENAMETOOLONG
    case ENAMETOOLONG: return LINUX_ENAMETOOLONG;
#endif
#ifdef ENOLCK
    case ENOLCK: return LINUX_ENOLCK;
#endif
#ifdef ENOSYS
    case ENOSYS: return LINUX_ENOSYS;
#endif
#ifdef ENOTEMPTY
    case ENOTEMPTY: return LINUX_ENOTEMPTY;
#endif
#ifdef ELOOP
    case ELOOP: return LINUX_ELOOP;
#endif
#if defined(EWOULDBLOCK)
# if !defined(EAGAIN) || (EAGAIN != EWOULDBLOCK)
    case EWOULDBLOCK: return LINUX_EWOULDBLOCK;
# endif
#endif
#ifdef ENOMSG
    case ENOMSG: return LINUX_ENOMSG;
#endif
#ifdef EIDRM
    case EIDRM: return LINUX_EIDRM;
#endif
#ifdef ECHRNG
    case ECHRNG: return LINUX_ECHRNG;
#endif
#ifdef EL2NSYNC
    case EL2NSYNC: return LINUX_EL2NSYNC;
#endif
#ifdef EL3HLT
    case EL3HLT: return LINUX_EL3HLT;
#endif
#ifdef EL3RST
    case EL3RST: return LINUX_EL3RST;
#endif
#ifdef ELNRNG
    case ELNRNG: return LINUX_ELNRNG;
#endif
#ifdef EUNATCH
    case EUNATCH: return LINUX_EUNATCH;
#endif
#ifdef ENOCSI
    case ENOCSI: return LINUX_ENOCSI;
#endif
#ifdef EL2HLT
    case EL2HLT: return LINUX_EL2HLT;
#endif
#ifdef EBADE
    case EBADE: return LINUX_EBADE;
#endif
#ifdef EBADR
    case EBADR: return LINUX_EBADR;
#endif
#ifdef EXFULL
    case EXFULL: return LINUX_EXFULL;
#endif
#ifdef ENOANO
    case ENOANO: return LINUX_ENOANO;
#endif
#ifdef EBADRQC
    case EBADRQC: return LINUX_EBADRQC;
#endif
#ifdef EBADSLT
    case EBADSLT: return LINUX_EBADSLT;
#endif
#if defined(EDEADLOCK)
# if !defined(EDEADLK) || (EDEADLK != EDEADLOCK)
    case EDEADLOCK: return LINUX_EDEADLOCK;
# endif
#endif
#ifdef EBFONT
    case EBFONT: return LINUX_EBFONT;
#endif
#ifdef ENOSTR
    case ENOSTR: return LINUX_ENOSTR;
#endif
#ifdef ENODATA
    case ENODATA: return LINUX_ENODATA;
#endif
#ifdef ETIME
    case ETIME: return LINUX_ETIME;
#endif
#ifdef ENOSR
    case ENOSR: return LINUX_ENOSR;
#endif
#ifdef ENONET
    case ENONET: return LINUX_ENONET;
#endif
#ifdef ENOPKG
    case ENOPKG: return LINUX_ENOPKG;
#endif
#ifdef EREMOTE
    case EREMOTE: return LINUX_EREMOTE;
#endif
#ifdef ENOLINK
    case ENOLINK: return LINUX_ENOLINK;
#endif
#ifdef EADV
    case EADV: return LINUX_EADV;
#endif
#ifdef ESRMNT
    case ESRMNT: return LINUX_ESRMNT;
#endif
#ifdef ECOMM
    case ECOMM: return LINUX_ECOMM;
#endif
#ifdef EPROTO
    case EPROTO: return LINUX_EPROTO;
#endif
#ifdef EMULTIHOP
    case EMULTIHOP: return LINUX_EMULTIHOP;
#endif
#ifdef EDOTDOT
    case EDOTDOT: return LINUX_EDOTDOT;
#endif
#ifdef EBADMSG
    case EBADMSG: return LINUX_EBADMSG;
#endif
#ifdef EOVERFLOW
    case EOVERFLOW: return LINUX_EOVERFLOW;
#endif
#ifdef ENOTUNIQ
    case ENOTUNIQ: return LINUX_ENOTUNIQ;
#endif
#ifdef EBADFD
    case EBADFD: return LINUX_EBADFD;
#endif
#ifdef EREMCHG
    case EREMCHG: return LINUX_EREMCHG;
#endif
#ifdef ELIBACC
    case ELIBACC: return LINUX_ELIBACC;
#endif
#ifdef ELIBBAD
    case ELIBBAD: return LINUX_ELIBBAD;
#endif
#ifdef ELIBSCN
    case ELIBSCN: return LINUX_ELIBSCN;
#endif
#ifdef ELIBMAX
    case ELIBMAX: return LINUX_ELIBMAX;
#endif
#ifdef ELIBEXEC
    case ELIBEXEC: return LINUX_ELIBEXEC;
#endif
#ifdef EILSEQ
    case EILSEQ: return LINUX_EILSEQ;
#endif
#ifdef ERESTART
    case ERESTART: return LINUX_ERESTART;
#endif
#ifdef ESTRPIPE
    case ESTRPIPE: return LINUX_ESTRPIPE;
#endif
#ifdef EUSERS
    case EUSERS: return LINUX_EUSERS;
#endif
#ifdef ENOTSOCK
    case ENOTSOCK: return LINUX_ENOTSOCK;
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ: return LINUX_EDESTADDRREQ;
#endif
#ifdef EMSGSIZE
    case EMSGSIZE: return LINUX_EMSGSIZE;
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE: return LINUX_EPROTOTYPE;
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT: return LINUX_ENOPROTOOPT;
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT: return LINUX_EPROTONOSUPPORT;
#endif
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT: return LINUX_ESOCKTNOSUPPORT;
#endif
#ifdef EOPNOTSUPP
    case EOPNOTSUPP: return LINUX_EOPNOTSUPP;
#endif
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT: return LINUX_EPFNOSUPPORT;
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT: return LINUX_EAFNOSUPPORT;
#endif
#ifdef EADDRINUSE
    case EADDRINUSE: return LINUX_EADDRINUSE;
#endif
#ifdef EADDRNOTAVAIL
    case EADDRNOTAVAIL: return LINUX_EADDRNOTAVAIL;
#endif
#ifdef ENETDOWN
    case ENETDOWN: return LINUX_ENETDOWN;
#endif
#ifdef ENETUNREACH
    case ENETUNREACH: return LINUX_ENETUNREACH;
#endif
#ifdef ENETRESET
    case ENETRESET: return LINUX_ENETRESET;
#endif
#ifdef ECONNABORTED
    case ECONNABORTED: return LINUX_ECONNABORTED;
#endif
#ifdef ECONNRESET
    case ECONNRESET: return LINUX_ECONNRESET;
#endif
#ifdef ENOBUFS
    case ENOBUFS: return LINUX_ENOBUFS;
#endif
#ifdef EISCONN
    case EISCONN: return LINUX_EISCONN;
#endif
#ifdef ENOTCONN
    case ENOTCONN: return LINUX_ENOTCONN;
#endif
#ifdef ESHUTDOWN
    case ESHUTDOWN: return LINUX_ESHUTDOWN;
#endif
#ifdef ETOOMANYREFS
    case ETOOMANYREFS: return LINUX_ETOOMANYREFS;
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT: return LINUX_ETIMEDOUT;
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED: return LINUX_ECONNREFUSED;
#endif
#ifdef EHOSTDOWN
    case EHOSTDOWN: return LINUX_EHOSTDOWN;
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH: return LINUX_EHOSTUNREACH;
#endif
#ifdef EALREADY
    case EALREADY: return LINUX_EALREADY;
#endif
#ifdef EINPROGRESS
    case EINPROGRESS: return LINUX_EINPROGRESS;
#endif
#ifdef ESTALE
    case ESTALE: return LINUX_ESTALE;
#endif
#ifdef EUCLEAN
    case EUCLEAN: return LINUX_EUCLEAN;
#endif
#ifdef ENOTNAM
    case ENOTNAM: return LINUX_ENOTNAM;
#endif
#ifdef ENAVAIL
    case ENAVAIL: return LINUX_ENAVAIL;
#endif
#ifdef EISNAM
    case EISNAM: return LINUX_EISNAM;
#endif
#ifdef EREMOTEIO
    case EREMOTEIO: return LINUX_EREMOTEIO;
#endif
#ifdef EDQUOT
    case EDQUOT: return LINUX_EDQUOT;
#endif
#ifdef ENOMEDIUM
    case ENOMEDIUM: return LINUX_ENOMEDIUM;
#endif
#ifdef EMEDIUMTYPE
    case EMEDIUMTYPE: return LINUX_EMEDIUMTYPE;
#endif
#ifdef ECANCELED
    case ECANCELED: return LINUX_ECANCELED;
#endif
#ifdef ENOKEY
    case ENOKEY: return LINUX_ENOKEY;
#endif
#ifdef EKEYEXPIRED
    case EKEYEXPIRED: return LINUX_EKEYEXPIRED;
#endif
#ifdef EKEYREVOKED
    case EKEYREVOKED: return LINUX_EKEYREVOKED;
#endif
#ifdef EKEYREJECTED
    case EKEYREJECTED: return LINUX_EKEYREJECTED;
#endif
#ifdef EOWNERDEAD
    case EOWNERDEAD: return LINUX_EOWNERDEAD;
#endif
#ifdef ENOTRECOVERABLE
    case ENOTRECOVERABLE: return LINUX_ENOTRECOVERABLE;
#endif
#ifdef ERFKILL
    case ERFKILL: return LINUX_ERFKILL;
#endif
#ifdef EHWPOISON
    case EHWPOISON: return LINUX_EHWPOISON;
#endif
    }
    return LINUX_EINVAL;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::Target2HostFileDescriptor(Linux& lin, int32_t fd)
{
  // Return an error if file descriptor does not exist
  typename std::map<int32_t, int>::iterator iter = lin.target_to_host_fildes.find(fd);
  if(iter == lin.target_to_host_fildes.end()) return -1;

  // Translate the target file descriptor to an host file descriptor
  return (*iter).second;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::GetParam(Linux& lin, int id)
{
  return lin.target_system->GetSystemCallParam(id);
}

namespace {
  template <uintptr_t N>
  bool
  SubPathOf( std::string str, char const (&ref)[N] )
  {
    for (uintptr_t idx = 0; idx < (N-1); ++idx)
      if (ref[idx] != str[idx]) return false;
    return (str[N-1] == '\0') or (str[N-1] == '/');
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::OpenAt( int dfd, std::string const& filename, int flags, unsigned short mode )
{
  if (SubPathOf(filename, "/dev") or SubPathOf(filename, "/proc") or
      SubPathOf(filename, "/sys") or SubPathOf(filename, "/var"))
    {
      // deny access to /dev, /proc, /sys, /var
      debug_warning_stream << "No guest access to host " << filename << std::endl;
      SetSystemCallStatus(-LINUX_EACCES,true);
      return -1;
    }
  
  if ((filename[0] != '/') and (dfd != LINUX_AT_FDCWD))
    {
      debug_warning_stream << "Directory-descriptor-relative openat not (yet) supported." << std::endl;
      SetSystemCallStatus(-LINUX_EACCES,true);
      return -1;
    }
    
  int host_flags = 0, host_mode = 0;
        
#if defined(linux) || defined(__linux) || defined(__linux__)
  host_flags = flags;
  host_mode = mode;
#else
  // non-Linux open flags encoding may differ from a true Linux guest
  host_flags = 0;
  if((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
  if((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
  if((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
  if(flags & LINUX_O_CREAT) host_flags |= O_CREAT;
  if(flags & LINUX_O_EXCL) host_flags |= O_EXCL;
  if(flags & LINUX_O_TRUNC) host_flags |= O_TRUNC;
  if(flags & LINUX_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
  host_flags |= O_BINARY; // Linux opens file as binary files
  host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
  host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
#endif
  
  int host_fd = open(filename.c_str(), host_flags, host_mode);
	
  if (host_fd == -1)
    {
      SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
      return -1;
    }
    
  int target_fd = AllocateFileDescriptor();
  // keep relation between the target file descriptor and the host file descriptor
  MapTargetToHostFileDescriptor(target_fd, host_fd);
    
  SetSystemCallStatus(target_fd, false);
  return target_fd;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
typename Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSysCall( std::string _name )
{
  {
    static struct : public SysCall {
      char const* GetName() const { return "exit"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "(int status=" << std::dec << int(SysCall::GetParam(lin, 0)) << ")"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int status = SysCall::GetParam(lin, 0);
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        throw LSCExit( status );
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "read"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", void* buf=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", size_t count=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ")" << std::dec;
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd = SysCall::GetParam(lin, 0);
        parameter_type buf_addr = SysCall::GetParam(lin, 1);
        size_t count = (size_t) SysCall::GetParam(lin, 2);

        int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF,true);
            return;
          }
        
        void* buf = malloc(count);

        if (not buf)
          {
            lin.debug_warning_stream << "Out of memory" << std::endl;
            lin.SetSystemCallStatus(-LINUX_ENOMEM,true);
            return;
          }
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        ssize_t ret = read(host_fd, buf, count);
        
        if (ret == -1) {
          lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
          return;
        }
        if (ret > 0)
          lin.WriteMemory(buf_addr, (uint8_t *)buf, ret);
        
        free(buf);

        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "write"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", const void *buf=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", size_t count=" << std::dec << unsigned(SysCall::GetParam(lin, 2))
             << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int target_fd = SysCall::GetParam(lin, 0);
        parameter_type buf_addr = SysCall::GetParam(lin, 1);
        parameter_type count = SysCall::GetParam(lin, 2);
        
        int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF,true);
            return;
          }

        void* buf = malloc(count);

        if (not buf)
          {
            lin.debug_warning_stream << "Out of memory" << std::endl;
            lin.SetSystemCallStatus(-LINUX_ENOMEM,true);
            return;
          }
            
        lin.ReadMemory(buf_addr, (uint8_t *)buf, count, true);
                
        if (unlikely(lin.verbose_))
          {
            lin.debug_info_stream << this->TraceCall(lin) << std::endl << "*buffer =";
            for (size_t i = 0; i < count; i++)
              {
                lin.debug_info_stream << " 0x" << std::hex << (unsigned int)((uint8_t *)buf)[i] << std::dec;
              }
            lin.debug_info_stream << std::endl;
          }
                
        ssize_t ret = write(host_fd, buf, count);
                
        if (ret == -1) {
          lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
        }
            
        free(buf);
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "openat"; }
      // long sys_openat(int dfd, const char __user *filename, int flags, umode_t mode);
      struct Args
      {
        Args(Linux& lin)
          : dfd(SysCall::GetParam(lin, 0)), filename(SysCall::GetParam(lin, 1)), flags(SysCall::GetParam(lin, 2)), mode(SysCall::GetParam(lin, 3))
          , filename_string(), filename_valid(lin.ReadString(filename, filename_string, false))
        {};
        int dfd; parameter_type filename; int flags; unsigned short mode; std::string filename_string; bool filename_valid;
        void Describe( std::ostream& sink ) const
        {
          sink << "( int dfd=" << dfd
               << ", const char *filename=0x" << std::hex << filename << " \"" << filename_string << "\""
               << ", int flags=0x" << std::hex << unsigned(flags)
               << ", umode_t mode=0x" << std::hex << unsigned(mode)
               << " )" << std::dec;
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin).Describe(sink); }
      void Execute( Linux& lin, int syscall_id ) const
      {
        Args args(lin);
        
        if (not args.filename_valid)
          {
            lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
            return;
          }

        int fd = lin.OpenAt( args.dfd, args.filename_string, args.flags, args.mode );
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << GetName() << SysCall::argsPrint(args) << " => " << fd;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall
    {
      char const* GetName() const { return "open"; }
      // long sys_open(const char __user *filename, int flags, umode_t mode);
      struct Args
      {
        Args(Linux& lin, bool inject)
          : filename(SysCall::GetParam(lin, 0)), flags(SysCall::GetParam(lin, 1)), mode(SysCall::GetParam(lin, 2))
          , filename_string(), filename_valid(lin.ReadString(filename, filename_string, inject))
        {};
        parameter_type filename; int flags; unsigned short mode; std::string filename_string; bool filename_valid;
        void Describe(std::ostream& sink ) const
        {
          sink << "( const char *filename=0x" << std::hex << filename << " \"" << filename_string << "\""
               << ", int flags=0x" << std::hex << unsigned(flags)
               << ", umode_t mode=0x" << std::hex << unsigned(mode)
               << " )" << std::dec;
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
      void Execute( Linux& lin, int syscall_id ) const
      {
        Args args(lin, true);
        
        if (not args.filename_valid)
          {
            lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
            return;
          }

        int fd = lin.OpenAt( LINUX_AT_FDCWD, args.filename_string, args.flags, args.mode );
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << GetName() << SysCall::argsPrint(args) << " => " << fd;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "close"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ")" << std::dec;
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int target_fd = SysCall::GetParam(lin, 0);

        int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF,true);
            return;
          }
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        if (unsigned(host_fd) < 3)
          {
            lin.UnmapTargetToHostFileDescriptor(target_fd);
            lin.FreeFileDescriptor(target_fd);
            lin.SetSystemCallStatus(0, false);
            return;
          }
        
        int ret = close(host_fd);
        
        if (ret == -1)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
            return;
          }
        
        lin.UnmapTargetToHostFileDescriptor(target_fd);
        lin.FreeFileDescriptor(target_fd);
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "lseek"; }
      struct Args {
        Args(Linux& lin) : fd(SysCall::GetParam(lin, 0)), offset(SysCall::GetParam(lin, 1)), whence(SysCall::GetParam(lin, 2)) {};
        unsigned fd; sparameter_type offset; unsigned whence;
      };
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        Args sc(lin);
        sink << "(unsigned fd=" << std::dec << sc.fd << ", off_t offset=" << std::dec << sc.offset << ", unsigned whence=" << std::dec << sc.whence << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;

        Args sc(lin);
        int32_t host_fd = SysCall::Target2HostFileDescriptor(lin, sc.fd);
        
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF,false);
            return;
          }
        
        off_t ret = lseek(host_fd, sc.offset, sc.whence);
        
        if (ret == -1) {
          lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
          return;
        }
  
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getpid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;

        pid_t ret = (pid_t) getpid();
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getpgrp"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "gettid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // Note: in a single threaded environment, the thread ID is
        // equal to the process ID (PID, as returned by getpid)
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        pid_t ret = getpid();
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "getuid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        uid_t ret = getuid();
#endif
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "access"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        // int access(const char *pathname, int mode);
        parameter_type pathname = SysCall::GetParam(lin, 0);
        int mode = SysCall::GetParam(lin, 1);
        sink << "(const char *pathname=0x" << std::hex << pathname
             << ", int mode=0x" << std::hex << mode << ")" << std::dec;
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        parameter_type addr = SysCall::GetParam(lin, 0);
        mode_t mode = SysCall::GetParam(lin, 1);
        
        std::string pathname;
        if (not lin.ReadString(addr, pathname, true))
          {
            lin.debug_warning_stream << "Out of memory" << std::endl;
            lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
            return;
          }
        
        if (SubPathOf(pathname, "/dev") or SubPathOf(pathname, "/proc") or
            SubPathOf(pathname, "/sys") or SubPathOf(pathname, "/var"))
          {
            // deny access to /dev, /proc, /sys, /var
            lin.debug_warning_stream << "No guest access to host " << pathname << std::endl;
            lin.SetSystemCallStatus(-LINUX_EACCES,true);
            return;
          }

        if (unlikely(lin.verbose_))
          {
            lin.debug_info_stream << this->TraceCall(lin) << std::endl
                                  << "pathname=\"" << pathname << "\"" << std::endl;
          }
            
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        int win_mode = 0;
        win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
        int ret = access(pathname.c_str(), win_mode);
#else
        int ret = access(pathname.c_str(), mode);
#endif
        if (ret == -1)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno),true);
            return;
          }
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "brk"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        parameter_type addr = SysCall::GetParam(lin, 0);
        sink << "(void *addr=0x" << std::hex << addr << std::dec << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        parameter_type new_brk_point = SysCall::GetParam(lin, 0);

        if (new_brk_point > lin.brk_point_)
          {
            lin.brk_point_ = new_brk_point;
          }

        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
  
        lin.SetSystemCallStatus(lin.brk_point_, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getgid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        gid_t ret = getgid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "geteuid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        uid_t ret = geteuid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getegid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        gid_t ret = getegid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "munmap"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(void *addr=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", size_t length=" << std::dec << int(SysCall::GetParam(lin, 1))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "stat"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(const char *pathname=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", struct stat *buf=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "_llseek"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(unsigned int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", unsigned long offset_high=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", unsigned long offset_low=0x" << std::hex << (SysCall::GetParam(lin, 2))
             << ", loff_t *result=0x" << std::hex << (SysCall::GetParam(lin, 3))
             << ", unsigned int whence=" << std::dec << int(SysCall::GetParam(lin, 4))
             << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd = SysCall::GetParam(lin, 0);
        uint32_t offset_high = SysCall::GetParam(lin, 1);
        uint32_t offset_low = SysCall::GetParam(lin, 2);
        off64_t offset = (int64_t(offset_high) << 32) | offset_low;
        parameter_type result_addr = SysCall::GetParam(lin, 3);
        int whence = SysCall::GetParam(lin, 4);
        

        int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);

        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF,true);
            return;
          }
        
        off64_t ret = lseek64(host_fd, offset, whence);
      
        if (ret == -1)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
            return;
          }
        
        uint64_t lseek_result64 = unisim::util::endian::Host2Target(lin.endianness_, (uint64_t) ret);
        lin.WriteMemory(result_addr, (uint8_t *) &lseek_result64, sizeof(lseek_result64));
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
  
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "writev"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", const struct iovec *iov=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", int iovcnt=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // unsigned long fd, const struct iovec __user *vec, unsigned long vlen
        parameter_type target_fd = SysCall::GetParam(lin, 0);
        parameter_type iovecaddr = SysCall::GetParam(lin, 1);
        parameter_type count = SysCall::GetParam(lin, 2);
        unsigned const parameter_size = sizeof (parameter_type);
        int64_t sum = 0;
        
    
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        for (int step = count; (--step) >= 0; iovecaddr += 2*parameter_size)
          {
            parameter_type iov_base, iov_len;
            lin.ReadMemory(iovecaddr + 0*parameter_size, iov_base, true );
            lin.ReadMemory(iovecaddr + 1*parameter_size, iov_len, true );
//             iov_base = unisim::util::endian::Target2Host( lin.endianness_, iov_base );
//             iov_len  = unisim::util::endian::Target2Host( lin.endianness_, iov_len );
            assert( iov_len < 0x100000 );
            uint8_t buffer[iov_len];
            lin.ReadMemory(iov_base, &buffer[0], iov_len, true );
          
            int ret = ::write( target_fd, &buffer[0], iov_len );
          
            if (ret < 0)
              {
                lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno( errno ), true );
                return;
              }
          
            sum += ret;
          }
        
        lin.SetSystemCallStatus(sum, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "mmap"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(void *addr=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", size_t length=" << std::dec << int(SysCall::GetParam(lin, 1))
             << ", int prot=0x" << std::hex << (SysCall::GetParam(lin, 2))
             << ", int flags=0x" << std::hex << (SysCall::GetParam(lin, 3))
             << ", int fd=" << std::dec << int(SysCall::GetParam(lin, 4))
             << ", off_t offset=0x" << std::hex << (SysCall::GetParam(lin, 5))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "reboot"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int magic=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", int magic2=" << std::dec << int(SysCall::GetParam(lin, 1))
             << ", int cmd=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ", void *arg=0x" << std::hex << int(SysCall::GetParam(lin, 3))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;

  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "mprotect"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(void *addr=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", size_t len=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", int prot=0x" << std::hex << (SysCall::GetParam(lin, 2))
             << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "mmap2"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(void *addr=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", size_t length=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", int prot=0x" << std::hex << (SysCall::GetParam(lin, 2))
             << ", int flags=0x" << std::hex << (SysCall::GetParam(lin, 3))
             << ", int fd=" << std::dec << int(SysCall::GetParam(lin, 4))
             << ", off_t pgoffset=0x" << std::hex << (SysCall::GetParam(lin, 5))
             << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getuid32"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        uid_t ret = getuid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getgid32"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        gid_t ret = getgid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "geteuid32"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        uid_t ret = geteuid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getegid32"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        unsigned ret = 0;
#else
        gid_t ret = getegid();
#endif
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << " => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "flistxattr"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", char *list=0x" << std::hex << (SysCall::GetParam(lin, 1)) 
             << ", size_t size=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "exit_group"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int status=" << std::dec << int(SysCall::GetParam(lin, 0)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int status = SysCall::GetParam(lin, 0);
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        throw LSCExit( status );
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    struct FCntlSysCall : public SysCall
    {
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int fd=" << std::dec << unsigned(SysCall::GetParam(lin, 0))
             << ", unsigned cmd=" << std::dec << unsigned(SysCall::GetParam(lin, 1))
             << ", unsigned long arg=" << std::dec << SysCall::GetParam(lin, 2)
             << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        lin.SetSystemCallStatus(-LINUX_ENOSYS, true);
        return;
#else
        // long sys_fcntl64(unsigned int fd, unsigned int cmd, unsigned long arg);
        int32_t target_fd = SysCall::GetParam(lin, 0);
        int32_t cmd = SysCall::GetParam(lin, 1);
        parameter_type arg = SysCall::GetParam(lin, 2);
        
        int host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF, true);
            return;
          }
        
        switch(cmd)
          {
            // Safe fcntl commands
          case F_DUPFD:
            //    case F_DUPFD_CLOEXEC:
          case F_GETFD:
          case F_SETFD:
          case F_GETFL:
          case F_SETFL: {
            int64_t ret = fcntl(host_fd, cmd, arg);
        
            if (ret == -1)
              lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
            else
              lin.SetSystemCallStatus(ret, true);
            return;
          
          } break;
            
          case F_GETLK:
          case F_SETLK:
          case F_SETLKW:
          default:
            break;
            return;
          }
        lin.SetSystemCallStatus(-LINUX_EINVAL, true);
#endif
      }
    };
  
    static struct : public FCntlSysCall { char const* GetName() const { return "fcntl"; } } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
    static struct : public FCntlSysCall { char const* GetName() const { return "fcntl64"; } } sc64;
    if (_name.compare( sc64.GetName() ) == 0) return &sc64;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "dup"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(unsigned oldfd=" << std::dec << unsigned(SysCall::GetParam(lin, 0)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // long sys_dup(unsigned int fildes);
        int32_t target_oldfd = SysCall::GetParam(lin, 0);
        int host_oldfd =       SysCall::Target2HostFileDescriptor(lin, target_oldfd);
  
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        if (host_oldfd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF, true);
            return;
          }
        
        int64_t ret = dup(host_oldfd);
        
        if (ret == -1)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
            return;
          }
        
        int host_newfd = ret;
        int32_t target_newfd = lin.AllocateFileDescriptor();
        // keep relation between the target file descriptor and the host file descriptor
        lin.MapTargetToHostFileDescriptor(target_newfd, host_newfd);
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << GetName() << " => newfd=" << target_newfd << std::endl;
  
        lin.SetSystemCallStatus(target_newfd, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ioctl"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int fd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", unsigned long request=" << std::dec << unsigned(SysCall::GetParam(lin, 1))
             << ", ...)";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "chdir"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(const char *path=0x" << std::hex << (SysCall::GetParam(lin, 0)) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "getcwd"; }
      /* asmlinkage long sys_getcwd(char __user *buf, unsigned long size); */
      struct Args { Args(Linux& lin) : buf(SysCall::GetParam(lin, 0)), size(SysCall::GetParam(lin, 1)) {}; parameter_type buf, size; };
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        Args sc(lin);
        sink << "(char __user *buf=0x" << std::hex << sc.buf << ", unsigned long size=0x" << std::hex << sc.size << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
        lin.SetSystemCallStatus(-LINUX_ENOSYS, true);
        return;
#endif
        Args sc(lin);

        assert( sc.size < 0x10000 );
        char buffer[sc.size];

        char* ret = ::getcwd( &buffer[0], sc.size );

        if (not ret)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
            return;
          }

        uint32_t len = strlen( ret ) + 1;
        lin.WriteMemory(sc.buf, (uint8_t *)&buffer[0], len);
        
        lin.SetSystemCallStatus(len, false);
      }
      
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ugetrlimit"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(unsigned resource=" << std::dec << unsigned(SysCall::GetParam(lin, 0))
             << ", struct rlimit *rlim=0x" << std::hex << SysCall::GetParam(lin, 1) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getrlimit"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(unsigned resource=" << std::dec << unsigned(SysCall::GetParam(lin, 0))
             << ", struct rlimit *rlim=0x" << std::hex << SysCall::GetParam(lin, 1) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "setpgid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "(pid_t pid=" << std::dec << +SysCall::GetParam(lin, 0) << ", pid_t pgid=" << std::dec << +SysCall::GetParam(lin, 1) << ")"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "setrlimit"; }
      void Describe( Linux& lin, std::ostream& sink ) const {
        sink << "(unsigned resource=" << std::dec << unsigned(SysCall::GetParam(lin, 0))
             << ", const struct rlimit *rlim=0x" << std::hex << SysCall::GetParam(lin, 1) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rt_sigaction"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int signum=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", const struct sigaction *act=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", struct sigaction *oldact=0x" << std::hex << (SysCall::GetParam(lin, 2)) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getrusage"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int who=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", struct rusage *usage=0x" << std::hex << (SysCall::GetParam(lin, 1)) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "unlink"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(const char *pathname=0x" << std::hex << (SysCall::GetParam(lin, 0)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        parameter_type pathnameaddr = SysCall::GetParam(lin, 0);
        
        std::string pathname;
        if (not lin.ReadString(pathnameaddr, pathname, true))
          {
            lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
            return;
          }
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << GetName() << ".pathname=\"" << pathname << "\"" << std::endl;
        
        int ret = unlink(pathname.c_str());
        
        if (ret == -1) {
          lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
          return;
        }
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rename"; }
      struct Args { Args(Linux& lin) : oldpath(SysCall::GetParam(lin, 0)), newpath(SysCall::GetParam(lin, 1)) {}; parameter_type oldpath, newpath; };
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        Args sc(lin);
        sink << "(const char *oldpath=0x" << std::hex << sc.oldpath << ", const char *newpath=0x" << std::hex << sc.newpath << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        Args sc(lin);
        
        std::string oldpath, newpath;
        if (not lin.ReadString(sc.oldpath, oldpath, true) or not lin.ReadString(sc.newpath, newpath, true))
          {
            lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
            return;
          }
        
        int ret = rename(oldpath.c_str(), newpath.c_str());
        
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << GetName() << "(oldpath=\"" << oldpath << "\", newpath=\"" << newpath << "\") => " << ret << std::endl;
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "time"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(time_t *t=0x" << std::hex << (SysCall::GetParam(lin, 0)) << ")";
      }
      /* WAS EINVAL */
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "socketcall"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int call=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", unsigned long *args=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ")";
      }
      /* WAS EINVAL */
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "socket"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int domain=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", int type=" << std::dec << int(SysCall::GetParam(lin, 1))
             << ", int protocol=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "connect"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int sockfd=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", const struct sockaddr *addr=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", socklen_t addrlen=" << std::dec << int(SysCall::GetParam(lin, 2))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rt_sigprocmask"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int how=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", const sigset_t *set=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ", sigset_t *oldset=0x" << std::hex << (SysCall::GetParam(lin, 2))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "kill"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int pid=" << std::dec << int(SysCall::GetParam(lin, 0)) << ", int sig=" << std::dec << int(SysCall::GetParam(lin, 1)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        lin.SetSystemCallStatus(0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "tkill"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int tid=" << std::dec << int(SysCall::GetParam(lin, 0)) << ", int sig=" << std::dec << int(SysCall::GetParam(lin, 1)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        lin.SetSystemCallStatus(0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "tgkill"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int tgid=" << std::dec << int(SysCall::GetParam(lin, 0))
             << ", int tid=" << std::dec << int(SysCall::GetParam(lin, 1))
             << ", int sig=" << std::dec << int(SysCall::GetParam(lin, 2)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        lin.SetSystemCallStatus(0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ftruncate"; }
      struct Args { Args(Linux& lin) : fd(SysCall::GetParam(lin, 0)), length(SysCall::GetParam(lin, 1)) {}; unsigned fd; parameter_type length; };
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        Args sc(lin);
        sink << "(unsigned fd=" << std::dec << sc.fd << ", unsigned long length=" << std::dec << sc.length << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;
        
        Args sc(lin);
        int32_t host_fd = SysCall::Target2HostFileDescriptor(lin, sc.fd);
  
        if (host_fd == -1)
          {
            lin.SetSystemCallStatus(-LINUX_EBADF, true);
            return;
          }
        
        // TODO: check that the guest-long length fits ftruncate's parameter
        int ret = ftruncate(host_fd, sc.length);
        
        if (ret == -1)
          {
            lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
            return;
          }
  
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "umask"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(mode_t mask=0" << std::oct << uint32_t(SysCall::GetParam(lin, 0)) << ")";
      }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if (unlikely(lin.verbose_))
          lin.debug_info_stream << this->TraceCall(lin) << std::endl;

        mode_t mask = SysCall::GetParam(lin, 0);
        
        int ret = umask(mask);
        
        lin.SetSystemCallStatus(ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "statfs"; }
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(const char *path=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", struct statfs *buf=0x" << std::hex << (SysCall::GetParam(lin, 1))
             << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "vfork"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "execve"; }
      struct Args {
        Args(Linux& lin, bool inject)
          : filename(SysCall::GetParam(lin, 0)), argv(SysCall::GetParam(lin, 1)), envp(SysCall::GetParam(lin, 2))
          , filename_string(), filename_valid(lin.ReadString(filename, filename_string, inject))
        {}
        parameter_type filename; parameter_type argv; parameter_type envp;
        std::string filename_string; bool filename_valid;
        void Describe( std::ostream& sink ) const
        {
          sink << "(const char *filename=0x" << std::hex << filename << " \"" << filename_string << "\""
              << ", char *const argv[]=0x" << std::hex << argv
              << ", char *const envp[]=0x" << std::hex << envp
              << ")";
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "mount"; }
      // int mount(const char *source, const char *target,
      //           const char *filesystemtype, unsigned long mountflags,
      //           const void *data);
      struct Args
      {
        Args(Linux& lin, bool inject)
          : source(SysCall::GetParam(lin, 0)), target(SysCall::GetParam(lin, 1)), filesystemtype(SysCall::GetParam(lin, 2)), mountflags(SysCall::GetParam(lin, 3)), data(SysCall::GetParam(lin, 4))
          , source_string(), source_valid(lin.ReadString(source, source_string, inject))
          , target_string(), target_valid(lin.ReadString(target, target_string, inject))
          , filesystemtype_string(), filesystemtype_valid(lin.ReadString(filesystemtype, filesystemtype_string, inject))
          , data_string(), data_valid(lin.ReadString(data, data_string, inject))
        {}
        parameter_type source; parameter_type target; parameter_type filesystemtype; parameter_type mountflags; parameter_type data;
        std::string source_string; bool source_valid;
        std::string target_string; bool target_valid;
        std::string filesystemtype_string; bool filesystemtype_valid;
        std::string data_string; bool data_valid;
        
        void Describe(std::ostream& sink ) const
        {
          sink << "( const char *source=0x" << std::hex << source << " \"" << source_string << "\""
               << ", const char *target=0x" << std::hex << target << " \"" << target_string << "\""
               << ", const char *filesystemtype=0x" << std::hex << filesystemtype << " \"" << filesystemtype_string << "\""
               << ", unsigned long mountflags=0x" << std::hex << mountflags
               << ", const void *data=0x" << std::hex << data << " \"" << data_string << "\""
               << " )" << std::dec;
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "sync"; }
      // void sync(void);
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "pipe"; }
      // int pipe(int pipefd[2]);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int *pipefd=" << std::hex << unsigned(SysCall::GetParam(lin, 0)) << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "dup2"; }
      // int dup2(int oldfd, int newfd);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(int oldfd=" << std::dec << +SysCall::GetParam(lin, 0)
             << "(int newfd=" << std::dec << +SysCall::GetParam(lin, 1)
            << ")";
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getppid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "setsid"; }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "()"; }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "readlink"; }
      // ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
      struct Args
      {
        Args(Linux& lin, bool inject)
          : pathname(SysCall::GetParam(lin, 0)), buf(SysCall::GetParam(lin, 1)), bufsize(SysCall::GetParam(lin, 2))
          , pathname_string(), pathname_valid(lin.ReadString(pathname, pathname_string, inject))
        {}
        parameter_type pathname; parameter_type buf; unsigned long bufsize; std::string pathname_string; bool pathname_valid;
        void Describe(std::ostream& sink ) const
        {
          sink << "( const char *pathname=0x" << std::hex << pathname << " \"" << pathname_string << "\""
               << ", char *buf=0x" << std::hex << buf
               << ", size_t bufsize=0x" << std::hex << bufsize
               << " )" << std::dec;
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "wait4"; }
      // pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(pid_t pid=0x" << std::hex << (SysCall::GetParam(lin, 0))
              << ", int *wstatus=0x" << std::hex << (SysCall::GetParam(lin, 1))
              << ", int options=0x" << std::hex << (SysCall::GetParam(lin, 2))
              << ", struct rusage *rusage=0x" << std::hex << (SysCall::GetParam(lin, 3))
              << " )" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "sysinfo"; }
      // int sysinfo(struct sysinfo *info);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "(struct sysinfo *info=0x" << std::hex << (SysCall::GetParam(lin, 0)) << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "clone"; }
      // FIXME: The order of the arguments differs in the raw system call, and there are variations in the arguments across architectures.
      // x86-32, score, ARM, ARM 64, PA-RISC, arc, Power PC, xtensa, and MIPS
      // long clone(unsigned long flags, void *child_stack,
      //            int *ptid, unsigned long newtls,
      //            int *ctid);
      struct Args
      {
        Args(Linux& lin, bool inject)
          : flags(SysCall::GetParam(lin, 0))
          , child_stack(SysCall::GetParam(lin, 1))
          , ptid(SysCall::GetParam(lin, 2))
          , newtls(SysCall::GetParam(lin, 3))
          , ctid(SysCall::GetParam(lin, 4))
        {}
        parameter_type flags;
        parameter_type child_stack;
        parameter_type ptid;
        parameter_type newtls;
        parameter_type ctid;
        void Describe(std::ostream& sink ) const
        {
          sink << "( unsigned long flags=0x" << std::hex << flags
               << ", void *child_stack=0x" << std::hex << child_stack
               << ", int *ptid=0x" << std::hex << ptid
               << ", unsigned long newtls=0x" << std::hex << newtls
               << ", int *ctid=0x" << std::hex << ctid
               << ")" << std::dec;
        }
      };
      void Describe( Linux& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "waitpid"; }
      // pid_t waitpid(pid_t pid, int *wstatus, int options);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "( pid_t pid=0x" << std::hex << (SysCall::GetParam(lin, 0))
              << ", int *wstatus=0x" << std::hex << (SysCall::GetParam(lin, 1))
              << ", int options=0x" << std::hex << (SysCall::GetParam(lin, 2))
              << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "nanosleep"; }
      // int nanosleep(const struct timespec *req, struct timespec *rem);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "( const struct timespec *req=0x" << std::hex << (SysCall::GetParam(lin, 0))
              << ", struct timespec *rem=0x" << std::hex << (SysCall::GetParam(lin, 1))
              << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall
    {
      char const* GetName() const { return "getdents64"; }
      // int getdents64(unsigned int fd, struct linux_dirent64 *dirp,
      //                unsigned int count);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "( unsigned int fd=0x" << std::hex << (SysCall::GetParam(lin, 0))
              << ", struct linux_dirent64 *dirp=0x" << std::hex << (SysCall::GetParam(lin, 1))
              << ", unsigned int count=0x" << std::hex << (SysCall::GetParam(lin, 2))
              << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall
    {
      char const* GetName() const { return "poll"; }
      // long poll(struct pollfd __user *ufds, unsigned int nfds, long timeout);
      void Describe( Linux& lin, std::ostream& sink ) const
      {
        sink << "( struct pollfd __user *ufds=0x" << std::hex << (SysCall::GetParam(lin, 0))
             << ", unsigned int nfds=" << std::dec << (SysCall::GetParam(lin, 1))
             << ", long timeout=" << std::dec << (SysCall::GetParam(lin, 2))
             << ")" << std::dec;
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    struct UnimplementedSC : public SysCall
    {
      UnimplementedSC( std::string _name ) : name( _name ) {} std::string name;
      char const* GetName() const { return name.c_str(); }
      void Describe( Linux& lin, std::ostream& sink ) const { sink << "( ? )"; }
      void Release() { delete this; }
    };
  
    return new UnimplementedSC( _name );
  }
}

} // end of namespace unisim
} // end of namespace util
} // end of namespace os
} // end of namespace linux_os

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
