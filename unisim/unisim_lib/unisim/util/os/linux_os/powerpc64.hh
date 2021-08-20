/*
 *  Copyright (c) 2018 Commissariat a l'Energie Atomique (CEA) All rights
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
 */
 
#ifndef __UNISIM_UTIL_OS_LINUX_POWERPC64_HH__
#define __UNISIM_UTIL_OS_LINUX_POWERPC64_HH__

#include <unisim/util/os/linux_os/errno.hh>
#include <unisim/util/likely/likely.hh>

#include <stdexcept>
#include <cerrno>
#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <process.h>
#include <windows.h>
#else
#include <sys/times.h>
#include <sys/time.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

  template <class LINUX>
  struct PowerPC64System : public LINUX::TargetSystem
  {
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // PowerPC64 linux structs
    struct powerpc64_timespec {
      int64_t tv_sec;   /* Seconds.     */
      uint64_t tv_nsec; /* Nanoseconds. */
    };
    
    /* st_dev: 0, 64, unsigned, __dev_t, Device.
     * st_ino: 8, 64, unsigned, __ino64_t, File serial number.
     * st_nlink: 16, 64, unsigned, __nlink_t, Link count.
     * st_mode: 24, 32, unsigned, __mode_t, File mode.
     * st_uid: 28, 32, unsigned, __uid_t, User ID of the file's owner.
     * st_gid: 32, 32, unsigned, __gid_t, Group ID of the file's group.
     * __pad2: 36, 32, signed, int, Padding.
     * st_rdev: 40, 64, unsigned, __dev_t, Device number, if device.
     * st_size: 48, 64, signed, __off64_t, Size of file, in bytes.
     * st_blksize: 56, 64, signed, __blksize_t, Optimal block size for I/O.
     * st_blocks: 64, 64, signed, __blkcnt64_t, Number 512-byte blocks allocated.
     * st_atim: 72, 128, unsigned, struct timespec, Time of last access.
     * st_mtim: 88, 128, unsigned, struct timespec, Time of last modification.
     * st_ctim: 104, 128, unsigned, struct timespec, Time of last status change.
     */
    struct powerpc64_stat64
    {
      uint64_t st_dev;                    /* Device.                            0 (__dev_t) */
      uint64_t st_ino;                    /* File serial number.                8 (__ino64_t) */
      uint64_t st_nlink;                  /* Link count.                       16 (__nlink_t) */
      uint32_t st_mode;                   /* File mode.                        24 (__mode_t) */
      uint32_t st_uid;                    /* User ID of the file's owner.      28 (__uid_t) */
      uint32_t st_gid;                    /* Group ID of the file's group.     32 (__gid_t) */
      int32_t  __pad2;                    /*                                   36 (int) */
      uint64_t st_rdev;                   /* Device number, if device.         40 (__dev_t) */
      int64_t  st_size;                   /* Size of file, in bytes.           48 (__off64_t) */
      int64_t  st_blksize;                /* Optimal block size for I/O.       56 (__blksize_t) */
      int64_t  st_blocks;                 /* Number 512-byte blocks allocated. 64 (__blkcnt64_t) */
      struct powerpc64_timespec st_atim;  /* Time of last access.              72 (struct timespec) */
      struct powerpc64_timespec st_mtim;  /* Time of last modification.        88 (struct timespec) */
      struct powerpc64_timespec st_ctim;  /* Time of last status change.      104 (struct timespec) */
    };

    // struct powerpc64_stat
    // {
    //   int64_t st_dev;
    //   uint32_t __pad1;
    //   uint32_t st_ino;
    //   uint32_t st_mode;
    //   uint32_t st_nlink;
    //   uint32_t st_uid;
    //   uint32_t st_gid;
    //   int64_t st_rdev;
    //   uint32_t __pad2;
    //   int32_t st_size;
    //   int32_t st_blksize;
    //   int32_t st_blocks;
    //   struct powerpc64_timespec st_atim;
    //   struct powerpc64_timespec st_mtim;
    //   struct powerpc64_timespec st_ctim;
    //   uint32_t __unused4;
    //   uint32_t __unused5;
    // };

    struct powerpc64_tms {
      int32_t tms_utime;      /* User CPU time.                    (clock_t) */
      int32_t tms_stime;      /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;     /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;     /* System CPU time of dead children. (clock_t) */
    };

    struct powerpc64_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
      char domainname[65];
    };

    struct powerpc64_timeval
    {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct powerpc64_timezone
    {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int)*/
      int32_t tz_dsttime;     /* type of dst correction    (int)*/
    };

    PowerPC64System( LINUX& _lin ) : LINUX::TargetSystem( "powerpc64", _lin ) {}
        
    bool GetAT_HWCAP( address_type& hwcap ) const { return false; }
        
    bool SetupTarget() const
    {
      // Reset all target registers
      {
        char const* clear_registers[] = {
        "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
        "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",
        "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
        "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
        "cr"
        };
        for (int idx = sizeof(clear_registers)/sizeof(clear_registers[0]); --idx >= 0;)
          if (not lin.ClearTargetRegister(clear_registers[idx]))
            return false;
      }
      
      // Set CIA, TOC and ENVPTR to the program entry point descriptor
      address_type desc_addr = lin.GetEntryPoint();
      parameter_type instruction_address, toc, environment_pointer;
      if (not lin.ReadTargetMemory(desc_addr +  0, instruction_address) or
          not lin.SetTargetRegister("cia", instruction_address) or
          not lin.ReadTargetMemory(desc_addr +  8, toc) or
          not lin.SetTargetRegister("r2",  toc) or
          not lin.ReadTargetMemory(desc_addr + 16, environment_pointer) or
          not lin.SetTargetRegister("r11", environment_pointer)
          )
        return false;
      
      // Set SP to the base of the created stack
      unisim::util::blob::Section<address_type> const * sp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (sp_section == NULL)
        {
          lin.DebugErrorStream() << "Could not find the stack pointer section." << std::endl;
          return false;
        }

      // SP is register 1 in PowerPC64 ABI
      address_type stack_pointer = sp_section->GetAddr();
      if (not lin.SetTargetRegister("r1", stack_pointer))
        return false;
      parameter_type par1, par2;
      
      if (not lin.ReadTargetMemory(stack_pointer +  8, par1) or
          not lin.SetTargetRegister("r3", par1) or
          not lin.ReadTargetMemory(stack_pointer + 16, par2) or
          not lin.SetTargetRegister("r4", par2))
        return false;
      
      return true;
    }

    // static int Fstat(int fd, struct powerpc64_stat *target_stat, unisim::util::endian::endian_type endianness)
//     {
//       int ret;
//       struct stat host_stat;
//       ret = fstat(fd, &host_stat);
//       if(ret < 0) return ret;

//       memset(target_stat, 0, sizeof(struct powerpc64_stat));
      
// #if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
//       // 64-bit host
//       target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
//       target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
//       target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
//       target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
//       target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
//       target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
//       target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
//       target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
// #if defined(WIN64) || defined(_WIN64) // Windows x64
//       target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atime);
//       target_stat->st_atim.tv_nsec = 0;
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtime);
//       target_stat->st_mtim.tv_nsec = 0;
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctime);
//       target_stat->st_ctim.tv_nsec = 0;
// #elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
//       target_stat->st_blksize = Host2Target(endianness, (int64_t) host_stat.st_blksize);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_sec);
//       target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_nsec);
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_sec);
//       target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_nsec);
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_sec);
//       target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_nsec);
// #elif defined(__APPLE_CC__) // darwin POWERPC6464/x86_64
//       target_stat->st_blksize = Host2Target(endianness, (int64_t) host_stat.st_blksize);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_sec);
//       target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_nsec);
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_sec);
//       target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_nsec);
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_sec);
//       target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_nsec);
// #endif

// #else
//       // 32-bit host
//       target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
//       target_stat->st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
//       target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
//       target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
//       target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
//       target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
//       target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
//       target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
// #if defined(WIN32) || defined(_WIN32) // Windows 32
//       target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
//       target_stat->st_atim.tv_nsec = 0;
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
//       target_stat->st_mtim.tv_nsec = 0;
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
//       target_stat->st_ctim.tv_nsec = 0;
// #elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
//       target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_sec);
//       target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_nsec);
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_sec);
//       target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_nsec);
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_sec);
//       target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_nsec);
// #elif defined(__APPLE_CC__) // Darwin POWERPC6432/x86
//       target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
//       target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
//       target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_sec);
//       target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_nsec);
//       target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_sec);
//       target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_nsec);
//       target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_sec);
//       target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_nsec);
// #endif

// #endif
//       return ret;
//     }

    static int Fstat64(int fd, struct powerpc64_stat64 *target_stat, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
      struct _stati64 host_stat;
      ret = _fstati64(fd, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
      struct stat64 host_stat;
      ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
      struct stat host_stat;
      ret = fstat(fd, &host_stat);
#endif

      if(ret < 0) return ret;

      memset(target_stat, 0, sizeof(struct powerpc64_stat64));
      
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) || defined(_WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin POWERPC6464/x86_64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN32) || defined(_WIN32) // Windows 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin POWERPC6432/x86
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }

    static int Stat64(const char *pathname, struct powerpc64_stat64 *target_stat, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
      struct _stati64 host_stat;
      ret = _stati64(pathname, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
      struct stat64 host_stat;
      ret = stat64(pathname, &host_stat);
#elif defined(__APPLE_CC__)
      struct stat host_stat;
      ret = stat(pathname, &host_stat);
#endif

      if(ret < 0) return ret;

      memset(target_stat, 0, sizeof(struct powerpc64_stat64));
      
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) || defined(_WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin POWERPC6464/x86_64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (int64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN32) || defined(_WIN32) // Windows 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin POWERPC6432/x86
      target_stat->st_blksize = Host2Target(endianness, (int32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }

    static int Times(struct powerpc64_tms* target_tms, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
      FILETIME ftCreationTime;
      FILETIME ftExitTime;
      FILETIME ftKernelTime;
      FILETIME ftUserTime;

      if(GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime,
                         &ftKernelTime, &ftUserTime)) return -1;

      target_tms->tms_utime = Host2Target(endianness, (uint32_t) ftUserTime.dwLowDateTime);
      target_tms->tms_stime = Host2Target(endianness, (uint32_t) ftKernelTime.dwLowDateTime);
      target_tms->tms_cutime = 0;   // User CPU time of dead children
      target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
      struct tms host_tms;

      ret = (int) times(&host_tms);
      target_tms->tms_utime = Host2Target(endianness, (int32_t) host_tms.tms_utime);
      target_tms->tms_stime = Host2Target(endianness, (int32_t) host_tms.tms_stime);
      target_tms->tms_cutime = Host2Target(endianness, (int32_t) host_tms.tms_cutime);
      target_tms->tms_cstime = Host2Target(endianness, (int32_t) host_tms.tms_cstime);
#endif
      return ret;
    }

    static int GetTimeOfDay(struct powerpc64_timeval *target_timeval, struct powerpc64_timezone *target_timezone, unisim::util::endian::endian_type endianness)
    {
      int ret;
      struct timeval host_tv;
      struct timezone host_tz;

      ret = gettimeofday(&host_tv, &host_tz);

      if(ret == 0)
        {
          if(target_timeval)
            {
              target_timeval->tv_sec = Host2Target(endianness, (int32_t) host_tv.tv_sec);
              target_timeval->tv_usec = Host2Target(endianness, (int32_t) host_tv.tv_usec);
            }
          if(target_timezone)
            {
              target_timezone->tz_minuteswest = Host2Target(endianness, (int32_t) host_tz.tz_minuteswest);
              target_timezone->tz_dsttime = Host2Target(endianness, (int32_t) host_tz.tz_dsttime);
            }
        }
      return ret;
    }

    static void SetPOWERPC64SystemCallStatus(LINUX& _lin, int ret, bool error)
    {
      parameter_type val;

      char const* const cr = "cr";
  
      if(error)
        {
          if (not _lin.GetTargetRegister(cr, val)) return;
    
          val |= (1 << 28); // CR0[SO] <- 1
    
          if (not _lin.SetTargetRegister(cr, val)) return;
        }
      else
        {
          if (not _lin.GetTargetRegister(cr, val)) return;
    
          val &= ~(1 << 28); // CR0[SO] <- 0
    
          if (not _lin.SetTargetRegister(cr, val)) return;
        }
  
      val = (parameter_type)ret;
  
      if (not _lin.SetTargetRegister("r3", val)) return;
    }
    
    void SetSystemCallStatus(int64_t ret, bool error) const { SetPOWERPC64SystemCallStatus(lin, ret, error); }

    static parameter_type GetSystemCallParam(LINUX& lin, int id)
    {
      parameter_type val = 0;
          
      switch (id) {
      case 0: lin.GetTargetRegister("r3", val); break;
      case 1: lin.GetTargetRegister("r4", val); break;
      case 2: lin.GetTargetRegister("r5", val); break;
      case 3: lin.GetTargetRegister("r6", val); break;
      case 4: lin.GetTargetRegister("r7", val); break;
      default: throw std::logic_error("internal error");
      }
          
      return val;
    }
    
    parameter_type GetSystemCallParam(int id) const
    {
      try { return GetSystemCallParam( lin, id ); }
      
      catch (int x) {
        lin.DebugErrorStream() << "No syscall argument #" << id << " in " << this->name << " linux" << std::endl;
      }
      
      return 0;
    }

    SysCall* GetSystemCall( int& id ) const
    {
      // see either arch/powerpc64/include/asm/unistd.h or arch/powerpc64/include/uapi/asm/unistd.h in Linux source
      switch (id) {
      case 0: return this->GetSysCall("restart_syscall");
      case 1: return this->GetSysCall("exit");
      case 2: return this->GetSysCall("fork");
      case 3: return this->GetSysCall("read");
      case 4: return this->GetSysCall("write");
      case 5: return this->GetSysCall("open");
      case 6: return this->GetSysCall("close");
      case 7: return this->GetSysCall("waitpid");
      case 8: return this->GetSysCall("creat");
      case 9: return this->GetSysCall("link");
      case 10: return this->GetSysCall("unlink");
      case 11: return this->GetSysCall("execve");
      case 12: return this->GetSysCall("chdir");
      case 13: return this->GetSysCall("time");
      case 14: return this->GetSysCall("mknod");
      case 15: return this->GetSysCall("chmod");
      case 16: return this->GetSysCall("lchown");
      case 17: return this->GetSysCall("break");
      case 18: return this->GetSysCall("oldstat");
      case 19: return this->GetSysCall("lseek");
      case 20: return this->GetSysCall("getpid");
      case 21: return this->GetSysCall("mount");
      case 22: return this->GetSysCall("umount");
      case 23: return this->GetSysCall("setuid");
      case 24: return this->GetSysCall("getuid");
      case 25: return this->GetSysCall("stime");
      case 26: return this->GetSysCall("ptrace");
      case 27: return this->GetSysCall("alarm");
      case 28: return this->GetSysCall("oldfstat");
      case 29: return this->GetSysCall("pause");
      case 30: return this->GetSysCall("utime");
      case 31: return this->GetSysCall("stty");
      case 32: return this->GetSysCall("gtty");
      case 33: return this->GetSysCall("access");
      case 34: return this->GetSysCall("nice");
      case 35: return this->GetSysCall("ftime");
      case 36: return this->GetSysCall("sync");
      case 37: return this->GetSysCall("kill");
      case 38: return this->GetSysCall("rename");
      case 39: return this->GetSysCall("mkdir");
      case 40: return this->GetSysCall("rmdir");
      case 41: return this->GetSysCall("dup");
      case 42: return this->GetSysCall("pipe");
        // 43: times (see powerpc64 specific)
      case 44: return this->GetSysCall("prof");
      case 45: return this->GetSysCall("brk");
      case 46: return this->GetSysCall("setgid");
      case 47: return this->GetSysCall("getgid");
      case 48: return this->GetSysCall("signal");
      case 49: return this->GetSysCall("geteuid");
      case 50: return this->GetSysCall("getegid");
      case 51: return this->GetSysCall("acct");
      case 52: return this->GetSysCall("umount2");
      case 53: return this->GetSysCall("lock");
      case 54: return this->GetSysCall("ioctl");
      case 55: return this->GetSysCall("fcntl");
      case 56: return this->GetSysCall("mpx");
      case 57: return this->GetSysCall("setpgid");
      case 58: return this->GetSysCall("ulimit");
      case 59: return this->GetSysCall("oldolduname");
      case 60: return this->GetSysCall("umask");
      case 61: return this->GetSysCall("chroot");
      case 62: return this->GetSysCall("ustat");
      case 63: return this->GetSysCall("dup2");
      case 64: return this->GetSysCall("getppid");
      case 65: return this->GetSysCall("getpgrp");
      case 66: return this->GetSysCall("setsid");
      case 67: return this->GetSysCall("sigaction");
      case 68: return this->GetSysCall("sgetmask");
      case 69: return this->GetSysCall("ssetmask");
      case 70: return this->GetSysCall("setreuid");
      case 71: return this->GetSysCall("setregid");
      case 72: return this->GetSysCall("sigsuspend");
      case 73: return this->GetSysCall("sigpending");
      case 74: return this->GetSysCall("sethostname");
      case 75: return this->GetSysCall("setrlimit");
      case 76: return this->GetSysCall("getrlimit");
      case 77: return this->GetSysCall("getrusage");
        // 78: gettimeofday (see powerpc64 specific)
      case 79: return this->GetSysCall("settimeofday");
      case 80: return this->GetSysCall("getgroups");
      case 81: return this->GetSysCall("setgroups");
      case 82: return this->GetSysCall("select");
      case 83: return this->GetSysCall("symlink");
      case 84: return this->GetSysCall("oldlstat");
      case 85: return this->GetSysCall("readlink");
      case 86: return this->GetSysCall("uselib");
      case 87: return this->GetSysCall("swapon");
      case 88: return this->GetSysCall("reboot");
      case 89: return this->GetSysCall("readdir");
      case 90: return this->GetSysCall("mmap");
      case 91: return this->GetSysCall("munmap");
      case 92: return this->GetSysCall("truncate");
      case 93: return this->GetSysCall("ftruncate");
      case 94: return this->GetSysCall("fchmod");
      case 95: return this->GetSysCall("fchown");
      case 96: return this->GetSysCall("getpriority");
      case 97: return this->GetSysCall("setpriority");
      case 98: return this->GetSysCall("profil");
      case 99: return this->GetSysCall("statfs");
      case 100: return this->GetSysCall("fstatfs");
      case 101: return this->GetSysCall("ioperm");
      case 102: return this->GetSysCall("socketcall");
      case 103: return this->GetSysCall("syslog");
      case 104: return this->GetSysCall("setitimer");
      case 105: return this->GetSysCall("getitimer");
        // case stat (see powerpc64 specific)
      case 107: return this->GetSysCall("lstat");
        // 108: fstat (see powerpc64 specific)
      case 109: return this->GetSysCall("olduname");
      case 110: return this->GetSysCall("iopl");
      case 111: return this->GetSysCall("vhangup");
      case 112: return this->GetSysCall("idle");
      case 113: return this->GetSysCall("vm86");
      case 114: return this->GetSysCall("wait4");
      case 115: return this->GetSysCall("swapoff");
      case 116: return this->GetSysCall("sysinfo");
      case 117: return this->GetSysCall("ipc");
      case 118: return this->GetSysCall("fsync");
      case 119: return this->GetSysCall("sigreturn");
      case 120: return this->GetSysCall("clone");
      case 121: return this->GetSysCall("setdomainname");
        // 122: uname (see powerpc64 specific)
      case 123: return this->GetSysCall("modify_ldt");
      case 124: return this->GetSysCall("adjtimex");
      case 125: return this->GetSysCall("mprotect");
      case 126: return this->GetSysCall("sigprocmask");
      case 127: return this->GetSysCall("create_module");
      case 128: return this->GetSysCall("init_module");
      case 129: return this->GetSysCall("delete_module");
      case 130: return this->GetSysCall("get_kernel_syms");
      case 131: return this->GetSysCall("quotactl");
      case 132: return this->GetSysCall("getpgid");
      case 133: return this->GetSysCall("fchdir");
      case 134: return this->GetSysCall("bdflush");
      case 135: return this->GetSysCall("sysfs");
      case 136: return this->GetSysCall("personality");
      case 137: return this->GetSysCall("afs_syscall");
      case 138: return this->GetSysCall("setfsuid");
      case 139: return this->GetSysCall("setfsgid");
      case 140: return this->GetSysCall("_llseek");
      case 141: return this->GetSysCall("getdents");
      case 142: return this->GetSysCall("_newselect");
      case 143: return this->GetSysCall("flock");
      case 144: return this->GetSysCall("msync");
      case 145: return this->GetSysCall("readv");
      case 146: return this->GetSysCall("writev");
      case 147: return this->GetSysCall("getsid");
      case 148: return this->GetSysCall("fdatasync");
      case 149: return this->GetSysCall("_sysctl");
      case 150: return this->GetSysCall("mlock");
      case 151: return this->GetSysCall("munlock");
      case 152: return this->GetSysCall("mlockall");
      case 153: return this->GetSysCall("munlockall");
      case 154: return this->GetSysCall("sched_setparam");
      case 155: return this->GetSysCall("sched_getparam");
      case 159: return this->GetSysCall("sched_get_priority_max");
      case 160: return this->GetSysCall("sched_set_priority_max");
      case 161: return this->GetSysCall("sched_rr_get_interval");
      case 162: return this->GetSysCall("nanosleep");
      case 163: return this->GetSysCall("mremap");
      case 164: return this->GetSysCall("setresuid");
      case 165: return this->GetSysCall("getresuid");
      case 166: return this->GetSysCall("query_module");
      case 167: return this->GetSysCall("poll");
      case 168: return this->GetSysCall("nfsservctl");
      case 169: return this->GetSysCall("setresgid");
      case 170: return this->GetSysCall("getresgid");
      case 171: return this->GetSysCall("prctl");
      case 172: return this->GetSysCall("rt_sigreturn");
      case 173: return this->GetSysCall("rt_sigaction");
      case 174: return this->GetSysCall("rt_sigprocmask");
      case 175: return this->GetSysCall("rt_sigpending");
      case 176: return this->GetSysCall("rt_sigtimedwait");
      case 177: return this->GetSysCall("rt_sigqueueinfo");
      case 178: return this->GetSysCall("ry_sigsuspend");
      case 179: return this->GetSysCall("pread64");
      case 180: return this->GetSysCall("pwrite64");
      case 181: return this->GetSysCall("chown");
      case 182: return this->GetSysCall("getcwd");
      case 183: return this->GetSysCall("capget");
      case 184: return this->GetSysCall("capset");
      case 185: return this->GetSysCall("sigaltstack");
      case 186: return this->GetSysCall("sendfile");
      case 187: return this->GetSysCall("getpmsg");
      case 188: return this->GetSysCall("putpmsg");
      case 189: return this->GetSysCall("vfork");
      case 190: return this->GetSysCall("ugetrlimit");
      case 191: return this->GetSysCall("readahead");
      case 192: return this->GetSysCall("mmap2");
      case 193: return this->GetSysCall("truncate64");
      case 194: return this->GetSysCall("ftruncate64");
        // 195: stat64 (see powerpc64 specific)
      case 196: return this->GetSysCall("lstat64");
        // 197: fstat64 (see powerpc64 specific)
      case 198: return this->GetSysCall("pciconfig_read");
      case 199: return this->GetSysCall("pciconfig_write");
      case 200: return this->GetSysCall("pciconfig_iobase");
      case 201: return this->GetSysCall("multiplexer");
      case 202: return this->GetSysCall("getdents64");
      case 203: return this->GetSysCall("pivot_root");
      case 204: return this->GetSysCall("fcntl64");
      case 205: return this->GetSysCall("madvise");
      case 206: return this->GetSysCall("mincore");
      case 207: return this->GetSysCall("getpid"); // in reality gettid: assuming that in a mono-thread environment pid=tid
      case 208: return this->GetSysCall("tkill");
      case 209: return this->GetSysCall("setxattr");
      case 210: return this->GetSysCall("lsetxattr");
      case 211: return this->GetSysCall("fsetxattr");
      case 212: return this->GetSysCall("getxattr");
      case 213: return this->GetSysCall("lgetxattr");
      case 214: return this->GetSysCall("fgetxattr");
      case 215: return this->GetSysCall("listxattr");
      case 216: return this->GetSysCall("llistxattr");
      case 217: return this->GetSysCall("flistxattr");
      case 218: return this->GetSysCall("removexattr");
      case 219: return this->GetSysCall("lremovexattr");
      case 220: return this->GetSysCall("fremovexattr");
      case 221: return this->GetSysCall("futex");
      case 222: return this->GetSysCall("sched_setaffinity");
      case 223: return this->GetSysCall("sched_getaffinity");
        // 224 currently unused
      case 225: return this->GetSysCall("tuxcall");
      case 226: return this->GetSysCall("sendfile64");
      case 227: return this->GetSysCall("io_setup");
      case 228: return this->GetSysCall("io_destroy");
      case 229: return this->GetSysCall("io_getevents");
      case 230: return this->GetSysCall("io_submit");
      case 231: return this->GetSysCall("io_cancel");
      case 232: return this->GetSysCall("set_tid_address");
      case 233: return this->GetSysCall("fadvise64");
      case 234: return this->GetSysCall("exit_group");
      case 235: return this->GetSysCall("lookup_dcookie");
      case 236: return this->GetSysCall("epoll_create");
      case 237: return this->GetSysCall("epoll_ctl");
      case 238: return this->GetSysCall("epoll_wait");
      case 239: return this->GetSysCall("remap_file_pages");
      case 240: return this->GetSysCall("timer_create");
      case 241: return this->GetSysCall("timer_settime");
      case 242: return this->GetSysCall("timer_gettime");
      case 243: return this->GetSysCall("timer_getoverrun");
      case 244: return this->GetSysCall("timer_delete");
      case 245: return this->GetSysCall("clock_settime");
      case 246: return this->GetSysCall("clock_gettime");
      case 247: return this->GetSysCall("clock_getres");
      case 248: return this->GetSysCall("clock_nanosleep");
      case 249: return this->GetSysCall("swapcontext");
      case 250: return this->GetSysCall("tgkill");
      case 251: return this->GetSysCall("utimes");
      case 252: return this->GetSysCall("statfs64");
      case 253: return this->GetSysCall("fstatfs64");
      case 254: return this->GetSysCall("fadvise64_64");
      case 255: return this->GetSysCall("rtas");
      case 256: return this->GetSysCall("sys_debug_setcontext");
        // 257 is reserved for vserver
      case 258: return this->GetSysCall("migrate_pages");
      case 259: return this->GetSysCall("mbind");
      case 260: return this->GetSysCall("get_mempolicy");
      case 261: return this->GetSysCall("set_mempolicy");
      case 262: return this->GetSysCall("mq_open");
      case 263: return this->GetSysCall("mq_unlink");
      case 264: return this->GetSysCall("mq_timedsend");
      case 265: return this->GetSysCall("mq_timedreceive");
      case 266: return this->GetSysCall("mq_notify");
      case 267: return this->GetSysCall("mq_getsetattr");
      case 268: return this->GetSysCall("kexec_load");
      case 269: return this->GetSysCall("add_key");
      case 270: return this->GetSysCall("request_key");
      case 271: return this->GetSysCall("keyctl");
      case 272: return this->GetSysCall("waitid");
      case 273: return this->GetSysCall("ioprio_set");
      case 274: return this->GetSysCall("ioprio_get");
      case 275: return this->GetSysCall("inotify_init");
      case 276: return this->GetSysCall("inotify_add_watch");
      case 277: return this->GetSysCall("inotify_rm_watch");
      case 278: return this->GetSysCall("spu_run");
      case 279: return this->GetSysCall("spu_create");
      case 280: return this->GetSysCall("pselect6");
      case 281: return this->GetSysCall("ppoll");
      case 282: return this->GetSysCall("unshare");
      case 283: return this->GetSysCall("splice");
      case 284: return this->GetSysCall("tee");
      case 285: return this->GetSysCall("vmsplice");
      case 286: return this->GetSysCall("openat");
      case 287: return this->GetSysCall("mkdirat");
      case 288: return this->GetSysCall("mknodat");
      case 289: return this->GetSysCall("fchownat");
      case 290: return this->GetSysCall("futimesat");
      case 291: return this->GetSysCall("fstatat64");
      case 292: return this->GetSysCall("unlinkat");
      case 293: return this->GetSysCall("renameat");
      case 294: return this->GetSysCall("linkat");
      case 295: return this->GetSysCall("symlinkat");
      case 296: return this->GetSysCall("readlinkat");
      case 297: return this->GetSysCall("fchmodat");
      case 298: return this->GetSysCall("faccessat");
      case 299: return this->GetSysCall("get_robust_list");
      case 300: return this->GetSysCall("set_robust_list");
      case 301: return this->GetSysCall("move_pages");
      case 302: return this->GetSysCall("getcpu");
      case 303: return this->GetSysCall("epoll_wait");
      case 304: return this->GetSysCall("utimensat");
      case 305: return this->GetSysCall("signalfd");
      case 306: return this->GetSysCall("timerfd_create");
      case 307: return this->GetSysCall("eventfd");
      case 308: return this->GetSysCall("sync_file_range2");
      case 309: return this->GetSysCall("fallocate");
      case 310: return this->GetSysCall("subpage_prot");
      case 311: return this->GetSysCall("timerfd_settime");
      case 312: return this->GetSysCall("timerfd_gettime");
      case 313: return this->GetSysCall("signalfd4");
      case 314: return this->GetSysCall("eventfd2");
      case 315: return this->GetSysCall("epoll_create1");
      case 316: return this->GetSysCall("dup3");
      case 317: return this->GetSysCall("pipe2");
      case 318: return this->GetSysCall("inotify_init1");
      case 319: return this->GetSysCall("perf_event_open");
      case 320: return this->GetSysCall("preadv");
      case 321: return this->GetSysCall("pwritev");
      case 322: return this->GetSysCall("rt_tsigqueueinfo");
      case 323: return this->GetSysCall("fanotify_init");
      case 324: return this->GetSysCall("fanotify_mark");
      case 325: return this->GetSysCall("prlimit64");
      case 326: return this->GetSysCall("socket");
      case 327: return this->GetSysCall("bind");
      case 328: return this->GetSysCall("connect");
      case 329: return this->GetSysCall("listen");
      case 330: return this->GetSysCall("accept");
      case 331: return this->GetSysCall("getsockname");
      case 332: return this->GetSysCall("getpeername");
      case 333: return this->GetSysCall("socketpair");
      case 334: return this->GetSysCall("send");
      case 335: return this->GetSysCall("sendto");
      case 336: return this->GetSysCall("recv");
      case 337: return this->GetSysCall("recvfrom");
      case 338: return this->GetSysCall("shutdown");
      case 339: return this->GetSysCall("setsockopt");
      case 340: return this->GetSysCall("getsockopt");
      case 341: return this->GetSysCall("sendmsg");
      case 342: return this->GetSysCall("recvmsg");
      case 343: return this->GetSysCall("recvmmsg");
      case 344: return this->GetSysCall("accept4");
      case 345: return this->GetSysCall("name_to_handle_at");
      case 346: return this->GetSysCall("open_by_handle_at");
      case 347: return this->GetSysCall("clock_adjtime");
      case 348: return this->GetSysCall("syncfs");
      case 349: return this->GetSysCall("sendmmsg");
      case 350: return this->GetSysCall("setns");
      case 351: return this->GetSysCall("process_vm_readv");
      case 352: return this->GetSysCall("process_vm_writev");
            
        // Following is specific to PowerPC64
      case 43: /* PowerPC64 specific times syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "times"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              address_type buf_addr = GetSystemCallParam(lin, 0);
              sink << "(struct tms *buf=" << std::hex << buf_addr << std::dec << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              address_type buf_addr;
              int32_t target_errno = 0;
  
              buf_addr = GetSystemCallParam(lin, 0);

              struct powerpc64_tms target_tms;
              ret = Times(&target_tms, lin.GetEndianness());

              if (ret >= 0)
                lin.WriteMemory(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
              else
                target_errno = SysCall::HostToLinuxErrno(errno);

              if (unlikely(lin.GetVerbose()))
                lin.DebugInfoStream() << "times(buf=0x" << std::hex << buf_addr << std::dec << ")" << std::endl;
  
              SetPOWERPC64SystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        }

      case 78: /* PowerPC64 specific gettimeofday syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "gettimeofday"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              address_type tv = GetSystemCallParam(lin, 0);
              address_type tz = GetSystemCallParam(lin, 1);
              sink << "(struct timeval *tv=" << std::hex << tv << std::dec
                   << ", struct timezone *tz=" << std::hex << tz << std::dec << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret = -1;
              address_type tv_addr;
              address_type tz_addr;
              int32_t target_errno = 0;
              tv_addr = GetSystemCallParam(lin, 0);
              tz_addr = GetSystemCallParam(lin, 1);

              struct powerpc64_timeval target_tv;
              struct powerpc64_timezone target_tz;

              ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0, lin.GetEndianness());
              if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);

              if(ret == 0)
                {
                  if(tv_addr)
                    {
                      lin.WriteMemory( tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
                    }
                  if(tz_addr)
                    {
                      lin.WriteMemory( tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
                    }
                }

              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                              << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
                              << ", tz = 0x" << std::hex << tz_addr << std::dec << ")"
                              << std::endl;
                }
  
              SetPOWERPC64SystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 106: /* PowerPC64 specific stat syscall */
        {
          static struct : public SysCall
          {
            char const* GetName() const { return "stat"; }
            // long sys_stat64(const char *filename, struct stat *statbuf);
            struct Args
            {
              Args(LINUX& lin, bool inject)
                : filename(SysCall::GetParam(lin, 0)), statbuf(SysCall::GetParam(lin, 1))
                , filename_string(), filename_valid(lin.ReadString(filename, filename_string, inject))
              {}
              address_type filename; address_type statbuf; std::string filename_string; bool filename_valid;
              void Describe( std::ostream& sink ) const
              {
                sink << "( const char *filename=0x" << std::hex << filename << " \"" << filename_string << "\""
                     << ", struct stat *statbuf=0x" << std::hex << statbuf
                     << " )" << std::dec;
              }
            };
            void Describe( LINUX& lin, std::ostream& sink ) const { Args(lin, false).Describe(sink); }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              Args args(lin, true);
        
              if (not args.filename_valid)
                {
                  lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
                  return;
                }

              struct powerpc64_stat64 target_stat;
              long status = Stat64(args.filename_string.c_str(), &target_stat, lin.GetEndianness());
              
              if (status != 0)
                {
                  lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
                  return;
                }
              
              if (not lin.WriteMemory( args.statbuf, (uint8_t *)&target_stat, sizeof(target_stat)))
                {
                  lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
                  return;
                }

              if (unlikely(lin.GetVerbose()))
                lin.DebugInfoStream() << GetName() << SysCall::argsPrint(args) << std::endl;

              lin.SetSystemCallStatus(0, false);
            }
          } sc;
          return &sc;
        } break;

      case 108: /* PowerPC64 specific fstat syscall */
        {
          static struct : public SysCall
          {
            char const* GetName() const { return "fstat"; }
            //long sys_fstat64(unsigned long fd, struct stat64 __user *statbuf);
            struct Args
            {
              Args(LINUX& lin) : fd(SysCall::GetParam(lin, 0)), statbuf(SysCall::GetParam(lin, 1)) {};
              parameter_type fd; address_type statbuf;
              void Describe( std::ostream& sink ) const
              {
                sink << "( unsigned long fd=" << std::dec << fd
                     << ", struct stat *statbuf=0x" << std::hex << statbuf
                     << " )" << std::dec;
              }
            };
            void Describe( LINUX& lin, std::ostream& sink ) const { Args(lin).Describe(sink); }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              Args args(lin);
        
              int host_fd = SysCall::Target2HostFileDescriptor(lin, args.fd);

              if (host_fd == -1)
                {
                  lin.SetSystemCallStatus(-LINUX_EBADF, true);
                  return;
                }
              
              struct powerpc64_stat64 target_stat;
              long status = Fstat64(host_fd, &target_stat, lin.GetEndianness());
              
              if (status != 0)
                {
                  lin.SetSystemCallStatus(-SysCall::HostToLinuxErrno(errno), true);
                  return;
                }
              
              if (not lin.WriteMemory( args.statbuf, (uint8_t *)&target_stat, sizeof(target_stat)))
                {
                  lin.SetSystemCallStatus(-LINUX_ENOMEM, true);
                  return;
                }

              if (unlikely(lin.GetVerbose()))
                lin.DebugInfoStream() << GetName() << SysCall::argsPrint(args) << std::endl;

              lin.SetSystemCallStatus(0, false);
            }
          } sc;
          return &sc;
        } break;

      case 122: /* PowerPC64 specific uname syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "uname"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              address_type buf_addr = GetSystemCallParam(lin, 0);
              sink << "(struct utsname *buf=" << std::hex << buf_addr << std::dec << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;
              address_type buf_addr = GetSystemCallParam(lin, 0);
              ret = 0;
                  
              struct powerpc64_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              strncpy(value.sysname,     utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(value.nodename,    utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(value.release,     utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(value.version,     utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(value.machine,     utsname.machine.c_str(), sizeof(value.machine) - 1);
              strncpy(value.domainname,  utsname.domainname.c_str(), sizeof(value.domainname) - 1);
              lin.WriteMemory( buf_addr, (uint8_t *)&value, sizeof(value));
  
              SetPOWERPC64SystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
        
        // case 195: /* PowerPC64 specific stat64 syscall */
        // {
        //   static struct : public SysCall {
        //     char const* GetName() const { return "stat64"; }
        //     void Describe( LINUX& lin, std::ostream& sink ) const
        //     {
        //       uint32_t path = GetSystemCallParam(lin, 0);
        //       address_type buf = GetSystemCallParam(lin, 1);
        //       sink << "(const char *pathname=" << std::hex << path
        //            << ", struct stat *buf=" << std::hex << buf << ")";
        //     }
        //     void Execute( LINUX& lin, int syscall_id ) const
        //     {
        //       int ret;
        //       address_type pathnameaddr = GetSystemCallParam(lin, 0);
        //       address_type buf_address = GetSystemCallParam(lin, 1);
        //       int32_t target_errno = 0;

        //       std::string pathname;
        //       if (this->ReadMemString(lin, pathnameaddr, pathname))
        //         {
        //           struct powerpc64_stat64 target_stat;
        //           ret = Stat64(pathname.c_str(), &target_stat, lin.GetEndianness());
        //           lin.WriteMemory( buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
    
        //           if(unlikely(lin.GetVerbose()))
        //             {
        //               lin.DebugInfoStream()
        //                           << "pathname = \"" << pathname << "\", buf_address = 0x" << std::hex << buf_address << std::dec
        //                           << std::endl;
        //             }
        //         }
        //       else
        //         {
        //           ret = -1;
        //           target_errno = LINUX_ENOMEM;
        //         }
  
        //       SetPOWERPC64SystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
        //     }
        //   } sc;
        //   return &sc;
        // }
        break;

      case 197: /* PowerPC64 specific fstat64 syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "fstat64"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              int32_t fd = GetSystemCallParam(lin, 0);
              address_type buf = GetSystemCallParam(lin, 1);
              sink << "(int fd=" << std::dec << fd << ", struct stat *buf=" << std::hex << buf << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              address_type buf_address;
              int32_t target_fd;
              int host_fd;
              int32_t target_errno = 0;

              target_fd = GetSystemCallParam(lin, 0);
              buf_address = GetSystemCallParam(lin, 1);
  
              host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
              if(host_fd == -1)
                {
                  target_errno = LINUX_EBADF;
                  ret = -1;
                }
              else
                {
                  struct powerpc64_stat64 target_stat;
                  ret = Fstat64(host_fd, &target_stat, lin.GetEndianness());
                  if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
      
                  lin.WriteMemory( buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                }
  
              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                              << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                              << std::endl;
                }
  
              SetPOWERPC64SystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
      }
          
      return 0;
    }
    
    bool SetSystemBlob( unisim::util::blob::Blob<address_type>* blob ) const { return true; }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_POWERPC64_HH__
