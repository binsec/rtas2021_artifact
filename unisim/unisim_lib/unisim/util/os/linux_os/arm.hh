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

#ifndef __UNISIM_UTIL_OS_LINUX_ARM_HH__
#define __UNISIM_UTIL_OS_LINUX_ARM_HH__

#include <unisim/util/likely/likely.hh>
#include <unisim/util/os/linux_os/errno.hh>

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

  // Register names
  static char const* const kARM_r0  = "r0"; /* syscall arg#1 */
  static char const* const kARM_r1  = "r1"; /* syscall arg#2, argc */
  static char const* const kARM_r2  = "r2"; /* syscall arg#3, argv */
  static char const* const kARM_r3  = "r3"; /* syscall arg#4 */
  static char const* const kARM_r4  = "r4"; /* syscall arg#5 */
  static char const* const kARM_r5  = "r5"; /* syscall arg#6 */
  static char const* const kARM_r6  = "r6"; /* syscall arg#7 */
  static char const* const kARM_r7  = "r7"; /* syscall NR */
  static char const* const kARM_sp  = "sp"; /* stack pointer */
  static char const* const kARM_pc  = "pc"; /* programm counter */

  template <class LINUX>
  struct ARMTS : public LINUX::TargetSystem
  {
    // HWCAP flags
    static uint32_t const ARM_HWCAP_ARM_SWP       = 1 << 0;
    static uint32_t const ARM_HWCAP_ARM_HALF      = 1 << 1;
    static uint32_t const ARM_HWCAP_ARM_THUMB     = 1 << 2;
    static uint32_t const ARM_HWCAP_ARM_26BIT     = 1 << 3;
    static uint32_t const ARM_HWCAP_ARM_FAST_MULT = 1 << 4;
    static uint32_t const ARM_HWCAP_ARM_FPA       = 1 << 5;
    static uint32_t const ARM_HWCAP_ARM_VFP       = 1 << 6;
    static uint32_t const ARM_HWCAP_ARM_EDSP      = 1 << 7;
    static uint32_t const ARM_HWCAP_ARM_JAVA      = 1 << 8;
    static uint32_t const ARM_HWCAP_ARM_IWMMXT    = 1 << 9;
    static uint32_t const ARM_HWCAP_ARM_CRUNCH    = 1 << 10;
    static uint32_t const ARM_HWCAP_ARM_THUMBEE   = 1 << 11;
    static uint32_t const ARM_HWCAP_ARM_NEON      = 1 << 12;
    static uint32_t const ARM_HWCAP_ARM_VFPv3     = 1 << 13;
    static uint32_t const ARM_HWCAP_ARM_VFPv3D16  = 1 << 14;
    static uint32_t const ARM_HWCAP_ARM_TLS       = 1 << 15;
    static uint32_t const ARM_HWCAP_ARM_VFPv4     = 1 << 16;
    static uint32_t const ARM_HWCAP_ARM_IDIVA     = 1 << 17;
    static uint32_t const ARM_HWCAP_ARM_IDIVT     = 1 << 18;
    
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // ARM linux structs
    struct arm_timespec {
      uint32_t tv_sec;  /* Seconds.      (__time_t) */
      uint32_t tv_nsec; /* Nanoseconds.  (long int) */
    };

    struct arm_stat64
    {
      uint64_t st_dev;             /* Device.                           (__dev_t, @0) */
      uint32_t __pad1;
      uint32_t __st_ino;           /* 32bit file serial number.	    (__ino_t, @12) */
      uint32_t st_mode;            /* File mode.                        (__mode_t, @16) */
      uint32_t st_nlink;           /* Link count.                       (__nlink_t, @20) */
      uint32_t st_uid;             /* User ID of the file's owner.	    (__uid_t, @24) */
      uint32_t st_gid;             /* Group ID of the file's group.     (__gid_t, @28) */
      uint64_t st_rdev;            /* Device number, if device.         (__dev_t, @32) */
      uint64_t __pad2;
      int64_t st_size;             /* Size of file, in bytes.           (__off64_t, @48) */
      uint32_t st_blksize;         /* Optimal block size for I/O.       (__blksize_t, @56) */
      uint32_t __pad3;
      uint64_t st_blocks;          /* Number 512-byte blocks allocated. (__blkcnt64_t, @64) */
      struct arm_timespec st_atim; /* Time of last access.              (struct timespec, @72) */
      struct arm_timespec st_mtim; /* Time of last modification.        (struct timespec, @80) */
      struct arm_timespec st_ctim; /* Time of last status change.       (struct timespec, @88) */
      uint64_t st_ino;             /* File serial number.               (__ino64_t, @96) */
    };

    struct arm_tms
    {
      int32_t tms_utime;          /* User CPU time.                    (clock_t) */
      int32_t tms_stime;          /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;         /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;         /* System CPU time of dead children. (clock_t) */
    };

    struct arm_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
    };

    struct arm_timeval {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct arm_timezone {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int) */
      int32_t tz_dsttime;     /* type of dst correction    (int) */
    };
    
    ARMTS( std::string _name, LINUX& _lin ) : LINUX::TargetSystem( _name, _lin ) {}
    
    bool GetAT_HWCAP( address_type& hwcap ) const
    {
      uint32_t arm_hwcap = 0;
      std::string hwcap_token;
      std::stringstream sstr(this->GetHWCAP());
      while(sstr >> hwcap_token)
        {
          if(hwcap_token.compare("swp") == 0)             { arm_hwcap |= ARM_HWCAP_ARM_SWP; }
          else if(hwcap_token.compare("half") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_HALF; }
          else if(hwcap_token.compare("thumb") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_THUMB; }
          else if(hwcap_token.compare("26bit") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_26BIT; }
          else if(hwcap_token.compare("fastmult") == 0)   { arm_hwcap |= ARM_HWCAP_ARM_FAST_MULT; }
          else if(hwcap_token.compare("fpa") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_FPA; }
          else if(hwcap_token.compare("vfp") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_VFP; }
          else if(hwcap_token.compare("edsp") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_EDSP; }
          else if(hwcap_token.compare("edsp") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_JAVA; }
          else if(hwcap_token.compare("java") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_JAVA; }
          else if(hwcap_token.compare("iwmmxt") == 0)     { arm_hwcap |= ARM_HWCAP_ARM_IWMMXT; }
          else if(hwcap_token.compare("crunch") == 0)     { arm_hwcap |= ARM_HWCAP_ARM_CRUNCH; }
          else if(hwcap_token.compare("thumbee") == 0)    { arm_hwcap |= ARM_HWCAP_ARM_THUMBEE; }
          else if(hwcap_token.compare("neon") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_NEON; }
          else if(hwcap_token.compare("vfpv3") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_VFPv3; }
          else if(hwcap_token.compare("vfpv3d16") == 0)   { arm_hwcap |= ARM_HWCAP_ARM_VFPv3D16; }
          else if(hwcap_token.compare("tls") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_TLS; }
          else if(hwcap_token.compare("vfpv4") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_VFPv4; }
          else if(hwcap_token.compare("idiva") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_IDIVA; }
          else if(hwcap_token.compare("idivt") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_IDIVT; }
          else { lin.DebugWarningStream() << "unknown hardware capability \"" << hwcap_token << "\"" << std::endl; }
        }
      hwcap = arm_hwcap;
      return true;
    }
    bool SetupTarget() const
    {
      // Reset all target registers
      {
        char const* clear_registers[] = {
          "r0",  "r1", "r2", "r3", "r4", "r5", "r6", "r7",
          "r8",  "r9", "sl", "fp", "ip", "sp", "lr"
        };
        for (int idx = sizeof(clear_registers)/sizeof(clear_registers[0]); --idx >= 0;)
          if (not lin.ClearTargetRegister(clear_registers[idx]))
            return false;
      } 
      
      /*** Program Status Register (PSR) ***/
      // NZCVQ <- 0
      // J <- 0
      // ITState <- 0
      // GE <- 0
      // E <- 0
      // AIF <- 0
      // T <- 0 (will be overwritten as a side effect of PC assignment, below)
      // M <- 0b10000 /* USER_MODE */
      if (not lin.SetTargetRegister("cpsr", 0x00000010))
        return false;
      
      /* We need to set SCTLR and CPACR as a standard linux would have done. We
       * only affects flags that impact a Linux OS emulation (others
       * are unaffected).
       */
      {
        uint32_t sctlr;
        if (not lin.GetTargetRegister("sctlr", sctlr))
          return false;
        {
          uint32_t const I = 1<<12;
          uint32_t const C = 1<< 2;
          uint32_t const A = 1<< 1;
          
          sctlr |=  I; // Instruction Cache enable
          sctlr |=  C; // Cache enable
          sctlr &= ~A; // Alignment check disable
        }
        if (not lin.SetTargetRegister("sctlr", sctlr))
          return false;
        if (not lin.SetTargetRegister("cpacr", 0x00f00000))
          return false;
      }
      
      // Set PC to the program entry point
      if (not lin.SetTargetRegister(kARM_pc, lin.GetEntryPoint()))
        return false;
      // Set SP to the base of the created stack
      unisim::util::blob::Section<address_type> const * sp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (sp_section == NULL) {
        lin.DebugErrorStream() << "Could not find the stack pointer section." << std::endl;
        return false;
      }
      if (not lin.SetTargetRegister(kARM_sp, sp_section->GetAddr()))
        return false;
      address_type par1_addr = sp_section->GetAddr() + 4;
      address_type par2_addr = sp_section->GetAddr() + 8;
      parameter_type par1 = 0;
      parameter_type par2 = 0;
      if (not this->MemIF().ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)) or
          not this->MemIF().ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2)) or
          not lin.SetTargetRegister(kARM_r1, Target2Host(lin.GetEndianness(), par1)) or
          not lin.SetTargetRegister(kARM_r2, Target2Host(lin.GetEndianness(), par2)))
        return false;
          
      return true;
    }
    
    static void SetARMSystemCallStatus(LINUX& _lin, int ret, bool error) { _lin.SetTargetRegister(kARM_r0, (parameter_type) ret); }
    
    void SetSystemCallStatus(int64_t ret, bool error) const { SetARMSystemCallStatus( lin, ret, error ); }
    
    static parameter_type GetSystemCallParam( LINUX& _lin, int id )
    {
      parameter_type val = 0;
          
      switch (id) {
      case 0: _lin.GetTargetRegister(kARM_r0, val); break;
      case 1: _lin.GetTargetRegister(kARM_r1, val); break;
      case 2: _lin.GetTargetRegister(kARM_r2, val); break;
      case 3: _lin.GetTargetRegister(kARM_r3, val); break;
      case 4: _lin.GetTargetRegister(kARM_r4, val); break;
      case 5: _lin.GetTargetRegister(kARM_r5, val); break;
      case 6: _lin.GetTargetRegister(kARM_r6, val); break;
      default: throw std::logic_error("internal_error");
      }
          
      return val;
    }
    
    parameter_type GetSystemCallParam( int id ) const
    {
      try { return GetSystemCallParam( lin, id ); }
      
      catch (int x) {
        lin.DebugErrorStream() << "No syscall argument #" << id << " in " << this->name << " linux" << std::endl;
      }
      
      return 0;
    }

    static int GetTimeOfDay(struct arm_timeval *target_timeval, struct arm_timezone *target_timezone, unisim::util::endian::endian_type endianness)
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

    static int Fstat64(int fd, struct arm_stat64 *target_stat, unisim::util::endian::endian_type endianness)
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

      memset(target_stat, 0, sizeof(struct arm_stat64));
	
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
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
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }

    static int Stat64(const char *pathname, struct arm_stat64* target_stat, unisim::util::endian::endian_type endianness)
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

      memset(target_stat, 0, sizeof(struct arm_stat64));
	
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) || defined(_WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
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
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }


    static int Times(struct arm_tms* target_tms, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
      FILETIME ftCreationTime;
      FILETIME ftExitTime;
      FILETIME ftKernelTime;
      FILETIME ftUserTime;

      if(GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

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

    SysCall* GetSystemCall( int& id ) const
    {
      if (name.compare("arm-eabi") == 0)
        {
          // The arm eabi ignores the supplied id and uses register 7
          parameter_type id_from_reg;
          if (not lin.GetTargetRegister(kARM_r7, id_from_reg))
            return 0;
          id = int(id_from_reg);
        }
      else if (name.compare("arm") == 0)
        {
          id -= 0x0900000; // Offset translation
        }
          
      // see either arch/arm/include/asm/unistd.h or arch/arm/include/uapi/asm/unistd.h in Linux source
      switch (id) {
      case 0: return this->GetSysCall("restart_syscall");
      case 1: return this->GetSysCall("exit");
      case 2: return this->GetSysCall("fork");
      case 3: return this->GetSysCall("read");
      case 4: return this->GetSysCall("write");
      case 5: return this->GetSysCall("open");
      case 6: return this->GetSysCall("close");
        // 7 was "waitpid"
      case 8: return this->GetSysCall("creat");
      case 9: return this->GetSysCall("link");
      case 10: return this->GetSysCall("unlink");
      case 11: return this->GetSysCall("execve");
      case 12: return this->GetSysCall("chdir");
      case 13: return this->GetSysCall("time");
      case 14: return this->GetSysCall("mknod");
      case 15: return this->GetSysCall("chmod");
      case 16: return this->GetSysCall("lchown");
        // 17 was "break"
        // 18 was "stat"
      case 19: return this->GetSysCall("lseek");
      case 20: return this->GetSysCall("getpid");
      case 21: return this->GetSysCall("mount");
      case 22: return this->GetSysCall("umount");
      case 23: return this->GetSysCall("setuid");
      case 24: return this->GetSysCall("getuid");
      case 25: return this->GetSysCall("stime");
      case 26: return this->GetSysCall("ptrace");
      case 27: return this->GetSysCall("alarm");
        // 28 was "fstat"
      case 29: return this->GetSysCall("pause");
      case 30: return this->GetSysCall("utime");
        // 31 was "stty"
        // 32 was "gtty"
      case 33: return this->GetSysCall("access");
      case 34: return this->GetSysCall("nice");
        // 35 was "ftime"
      case 36: return this->GetSysCall("sync");
      case 37: return this->GetSysCall("kill");
      case 38: return this->GetSysCall("rename");
      case 39: return this->GetSysCall("mkdir");
      case 40: return this->GetSysCall("rmdir");
      case 41: return this->GetSysCall("dup");
      case 42: return this->GetSysCall("pipe");
        // 43: times (see arm specific)
        // 44 was "prof"
      case 45: return this->GetSysCall("brk");
      case 46: return this->GetSysCall("setgid");
      case 47: return this->GetSysCall("getgid");
        // 48 was "signal"
      case 49: return this->GetSysCall("geteuid");
      case 50: return this->GetSysCall("getegid");
      case 51: return this->GetSysCall("acct");
      case 52: return this->GetSysCall("umount2");
        // 53 was "lock"
      case 54: return this->GetSysCall("ioctl");
      case 55: return this->GetSysCall("fcntl");
        // 56 was "mpx"
      case 57: return this->GetSysCall("setpgid");
        // 58 was "ulimit"
        // 59 was "olduname"
      case 60: return this->GetSysCall("umask");
      case 61: return this->GetSysCall("chroot");
      case 62: return this->GetSysCall("ustat");
      case 63: return this->GetSysCall("dup2");
      case 64: return this->GetSysCall("getppid");
      case 65: return this->GetSysCall("getpgrp");
      case 66: return this->GetSysCall("setsid");
      case 67: return this->GetSysCall("sigaction");
        // 68 was "sgetmask"
        // 69 was "ssetmask"
      case 70: return this->GetSysCall("setreuid");
      case 71: return this->GetSysCall("setregid");
      case 72: return this->GetSysCall("sigsuspend");
      case 73: return this->GetSysCall("sigpending");
      case 74: return this->GetSysCall("sethostname");
      case 75: return this->GetSysCall("setrlimit");
      case 76: return this->GetSysCall("getrlimit");
      case 77: return this->GetSysCall("getrusage");
        // 78: gettimeofday (see arm specific)
      case 79: return this->GetSysCall("settimeofday");
      case 80: return this->GetSysCall("getgroups");
      case 81: return this->GetSysCall("setgroups");
      case 82: return this->GetSysCall("select");
      case 83: return this->GetSysCall("symlink");
        // 84 was "lstat"
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
        // 98 was "profile"
      case 99: return this->GetSysCall("statfs");
      case 100: return this->GetSysCall("fstatfs");
      case 102: return this->GetSysCall("socketcall");
      case 103: return this->GetSysCall("syslog");
      case 104: return this->GetSysCall("setitimer");
      case 105: return this->GetSysCall("getitimer");
      case 106: return this->GetSysCall("stat");
      case 107: return this->GetSysCall("lstat");
        // 108: fstat (see arm specific)
        // 109 was "uname"
        // 110 was "iopl"
      case 111: return this->GetSysCall("vhangup");
        // 112 was "idle"
      case 113: return this->GetSysCall("syscall");
      case 114: return this->GetSysCall("wait4");
      case 115: return this->GetSysCall("swapoff");
      case 116: return this->GetSysCall("sysinfo");
      case 117: return this->GetSysCall("ipc");
      case 118: return this->GetSysCall("fsync");
      case 119: return this->GetSysCall("sigreturn");
      case 120: return this->GetSysCall("clone");
      case 121: return this->GetSysCall("setdomainname");
        // 122: uname (see arm specific)
        // 123 was "modify_ldt"
      case 124: return this->GetSysCall("adjtimex");
      case 125: return this->GetSysCall("mprotect");
      case 126: return this->GetSysCall("sigprocmask");
        // 127 was "create_module"
      case 128: return this->GetSysCall("init_module");
      case 129: return this->GetSysCall("delete_module");
        // 130 was "get_kernel_syms"
      case 131: return this->GetSysCall("quotactl");
      case 132: return this->GetSysCall("getpgid");
      case 133: return this->GetSysCall("fchdir");
      case 134: return this->GetSysCall("bdflush");
      case 135: return this->GetSysCall("sysfs");
      case 136: return this->GetSysCall("personality");
        // 137 was "afs_syscall"
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
      case 156: return this->GetSysCall("sched_setscheduler");
      case 157: return this->GetSysCall("sched_getscheduler");
      case 158: return this->GetSysCall("sched_yield");
      case 159: return this->GetSysCall("sched_get_priority_max");
      case 160: return this->GetSysCall("sched_get_priority_min");
      case 161: return this->GetSysCall("sched_rr_get_interval");
      case 162: return this->GetSysCall("nanosleep");
      case 163: return this->GetSysCall("mremap");
      case 164: return this->GetSysCall("setresuid");
      case 165: return this->GetSysCall("getresuid");
        // 166 was "vm86"
        // 167 was "query_module"
      case 168: return this->GetSysCall("poll");
      case 169: return this->GetSysCall("nfsservctl");
      case 170: return this->GetSysCall("setresgid");
      case 171: return this->GetSysCall("getresgid");
      case 172: return this->GetSysCall("prctl");
      case 173: return this->GetSysCall("rt_sigreturn");
      case 174: return this->GetSysCall("rt_sigaction");
      case 175: return this->GetSysCall("rt_sigprocmask");
      case 176: return this->GetSysCall("rt_sigpending");
      case 177: return this->GetSysCall("rt_sigtimedwait");
      case 178: return this->GetSysCall("rt_sigqueueinfo");
      case 179: return this->GetSysCall("rt_sigsuspend");
      case 180: return this->GetSysCall("pread64");
      case 181: return this->GetSysCall("pwrite64");
      case 182: return this->GetSysCall("chown");
      case 183: return this->GetSysCall("getcwd");
      case 184: return this->GetSysCall("capget");
      case 185: return this->GetSysCall("capset");
      case 186: return this->GetSysCall("sigaltstack");
      case 187: return this->GetSysCall("sendfile");
        // 188 is reserved
        // 189 is reserved
      case 190: return this->GetSysCall("vfork");
      case 191: return this->GetSysCall("ugetrlimit");
      case 192: return this->GetSysCall("mmap2");
      case 193: return this->GetSysCall("truncate64");
      case 194: return this->GetSysCall("ftruncate64");
        // 195: stat64 (see arm specific)
      case 196: return this->GetSysCall("lstat64");
        // 197: fstat64 (see arm specific)  
      case 198: return this->GetSysCall("lchown32");
      case 199: return this->GetSysCall("getuid32");
      case 200: return this->GetSysCall("getgid32");
      case 201: return this->GetSysCall("geteuid32");
      case 202: return this->GetSysCall("getegid32");
      case 203: return this->GetSysCall("setreuid32");
      case 204: return this->GetSysCall("setregid32");
      case 205: return this->GetSysCall("getgroups32");
      case 206: return this->GetSysCall("setgroups32");
      case 207: return this->GetSysCall("fchown32");
      case 208: return this->GetSysCall("setresuid32");
      case 209: return this->GetSysCall("getresuid32");
      case 210: return this->GetSysCall("setresgid32");
      case 211: return this->GetSysCall("getresgid32");
      case 212: return this->GetSysCall("chown32");
      case 213: return this->GetSysCall("setuid32");
      case 214: return this->GetSysCall("setgid32");
      case 215: return this->GetSysCall("setfsuid32");
      case 216: return this->GetSysCall("setfsgid32");
      case 217: return this->GetSysCall("getdents64");
      case 218: return this->GetSysCall("pivot_root");
      case 219: return this->GetSysCall("mincore");
      case 220: return this->GetSysCall("madvise");
      case 221: return this->GetSysCall("fcntl64");
        // 222 is for tux
        // 223 is unused
      case 224: return this->GetSysCall("gettid");
      case 225: return this->GetSysCall("readahead");
      case 226: return this->GetSysCall("setxattr");
      case 227: return this->GetSysCall("lsetxattr");
      case 228: return this->GetSysCall("fsetxattr");
      case 229: return this->GetSysCall("getxattr");
      case 230: return this->GetSysCall("lgetxattr");
      case 231: return this->GetSysCall("fgetxattr");
      case 232: return this->GetSysCall("listxattr");
      case 233: return this->GetSysCall("llistxattr");
      case 234: return this->GetSysCall("flistxattr");
      case 235: return this->GetSysCall("removexattr");
      case 236: return this->GetSysCall("lremovexattr");
      case 237: return this->GetSysCall("fremovexattr");
      case 238: return this->GetSysCall("tkill");
      case 239: return this->GetSysCall("sendfile64");
      case 240: return this->GetSysCall("futex");
      case 241: return this->GetSysCall("sched_setaffinity");
      case 242: return this->GetSysCall("sched_getaffinity");
      case 243: return this->GetSysCall("io_setup");
      case 244: return this->GetSysCall("io_destroy");
      case 245: return this->GetSysCall("io_getevents");
      case 246: return this->GetSysCall("io_submit");
      case 247: return this->GetSysCall("io_cancel");
      case 248: return this->GetSysCall("exit_group");
      case 249: return this->GetSysCall("lookup_dcookie");
      case 250: return this->GetSysCall("epoll_create");
      case 251: return this->GetSysCall("epoll_ctl");
      case 252: return this->GetSysCall("epoll_wait");
      case 253: return this->GetSysCall("remap_file_pages");
        // 254 is for set_thread_area
        // 255 is for get_thread_area
      case 256: return this->GetSysCall("set_tid_address");
      case 257: return this->GetSysCall("timer_create");
      case 258: return this->GetSysCall("timer_settime");
      case 259: return this->GetSysCall("timer_gettime");
      case 260: return this->GetSysCall("timer_getoverrun");
      case 261: return this->GetSysCall("timer_delete");
      case 262: return this->GetSysCall("clock_settime");
      case 263: return this->GetSysCall("clock_gettime");
      case 264: return this->GetSysCall("clock_getres");
      case 265: return this->GetSysCall("clock_nanosleep");
      case 266: return this->GetSysCall("statfs64");
      case 267: return this->GetSysCall("fstatfs64");
      case 268: return this->GetSysCall("tgkill");
      case 269: return this->GetSysCall("utimes");
      case 270: return this->GetSysCall("arm_fadvise64_64");
      case 271: return this->GetSysCall("pciconfig_iobase");
      case 272: return this->GetSysCall("pciconfig_read");
      case 273: return this->GetSysCall("pciconfig_write");
      case 274: return this->GetSysCall("mq_open");
      case 275: return this->GetSysCall("mq_unlink");
      case 276: return this->GetSysCall("mq_timedsend");
      case 277: return this->GetSysCall("mq_timedreceive");
      case 278: return this->GetSysCall("mq_notify");
      case 279: return this->GetSysCall("mq_getsetattr");
      case 280: return this->GetSysCall("waitid");
      case 281: return this->GetSysCall("socket");
      case 282: return this->GetSysCall("bind");
      case 283: return this->GetSysCall("connect");
      case 284: return this->GetSysCall("listen");
      case 285: return this->GetSysCall("accept");
      case 286: return this->GetSysCall("getsockname");
      case 287: return this->GetSysCall("getpeername");
      case 288: return this->GetSysCall("socketpair");
      case 289: return this->GetSysCall("send");
      case 290: return this->GetSysCall("sendto");
      case 291: return this->GetSysCall("recv");
      case 292: return this->GetSysCall("recvfrom");
      case 293: return this->GetSysCall("shutdown");
      case 294: return this->GetSysCall("setsockopt");
      case 295: return this->GetSysCall("getsockopt");
      case 296: return this->GetSysCall("sendmsg");
      case 297: return this->GetSysCall("recvmsg");
      case 298: return this->GetSysCall("semop");
      case 299: return this->GetSysCall("semget");
      case 300: return this->GetSysCall("semctl");
      case 301: return this->GetSysCall("msgsnd");
      case 302: return this->GetSysCall("msgrcv");
      case 303: return this->GetSysCall("msgget");
      case 304: return this->GetSysCall("msgctl");
      case 305: return this->GetSysCall("shmat");
      case 306: return this->GetSysCall("shmdt");
      case 307: return this->GetSysCall("shmget");
      case 308: return this->GetSysCall("shmctl");
      case 309: return this->GetSysCall("add_key");
      case 310: return this->GetSysCall("request_key");
      case 311: return this->GetSysCall("keyctl");
      case 312: return this->GetSysCall("semtimedop");
      case 313: return this->GetSysCall("vserver");
      case 314: return this->GetSysCall("ioprio_set");
      case 315: return this->GetSysCall("ioprio_get");
      case 316: return this->GetSysCall("inotify_init");
      case 317: return this->GetSysCall("inotify_add_watch");
      case 318: return this->GetSysCall("inotify_rm_watch");
      case 319: return this->GetSysCall("mbind");
      case 320: return this->GetSysCall("get_mempolicy");
      case 321: return this->GetSysCall("set_mempolicy");
      case 322: return this->GetSysCall("openat");
      case 323: return this->GetSysCall("mkdirat");
      case 324: return this->GetSysCall("mknodat");
      case 325: return this->GetSysCall("fchownat");
      case 326: return this->GetSysCall("futimesat");
      case 327: return this->GetSysCall("fstatat64");
      case 328: return this->GetSysCall("unlinkat");
      case 329: return this->GetSysCall("renameat");
      case 330: return this->GetSysCall("linkat");
      case 331: return this->GetSysCall("symlinkat");
      case 332: return this->GetSysCall("readlinkat");
      case 333: return this->GetSysCall("fchmodat");
      case 334: return this->GetSysCall("faccessat");
      case 335: return this->GetSysCall("pselect6");
      case 336: return this->GetSysCall("ppoll");
      case 337: return this->GetSysCall("unshare");
      case 338: return this->GetSysCall("set_robust_list");
      case 339: return this->GetSysCall("get_robust_list");
      case 340: return this->GetSysCall("splice");
      case 341: return this->GetSysCall("sync_file_range2");
      case 342: return this->GetSysCall("tee");
      case 343: return this->GetSysCall("vmsplice");
      case 344: return this->GetSysCall("move_pages");
      case 345: return this->GetSysCall("getcpu");
      case 346: return this->GetSysCall("epoll_pwait");
      case 347: return this->GetSysCall("kexec_load");
      case 348: return this->GetSysCall("utimensat");
      case 349: return this->GetSysCall("signalfd");
      case 350: return this->GetSysCall("timerfd_create");
      case 351: return this->GetSysCall("eventfd");
      case 352: return this->GetSysCall("fallocate");
      case 353: return this->GetSysCall("timerfd_settime");
      case 354: return this->GetSysCall("timerfd_gettime");
      case 355: return this->GetSysCall("signalfd4");
      case 356: return this->GetSysCall("eventfd2");
      case 357: return this->GetSysCall("epoll_create1");
      case 358: return this->GetSysCall("dup3");
      case 359: return this->GetSysCall("pipe2");
      case 360: return this->GetSysCall("inotify_init1");
      case 361: return this->GetSysCall("preadv");
      case 362: return this->GetSysCall("pwritev");
      case 363: return this->GetSysCall("rt_tgsigqueueinfo");
      case 364: return this->GetSysCall("perf_event_open");
      case 365: return this->GetSysCall("recvmmsg");
      case 366: return this->GetSysCall("accept4");
      case 367: return this->GetSysCall("fanotify_init");
      case 368: return this->GetSysCall("fanotify_mark");
      case 369: return this->GetSysCall("prlimit64");
      case 370: return this->GetSysCall("name_to_handle_at");
      case 371: return this->GetSysCall("open_by_handle_at");
      case 372: return this->GetSysCall("clock_adjtime");
      case 373: return this->GetSysCall("syncfs");
      case 374: return this->GetSysCall("sendmmsg");
      case 375: return this->GetSysCall("setns");
      case 376: return this->GetSysCall("process_vm_readv");
      case 377: return this->GetSysCall("process_vm_writev");
        // 378 is for kcmp
	
        // the following are private to the arm
      case 43: /* ARM specific times syscall */
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
                  
              struct arm_tms target_tms;
              ret = Times(&target_tms, lin.GetEndianness());

              if(ret >= 0)
                {
                  lin.WriteMemory(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
                }
              else
                {
                  target_errno = SysCall::HostToLinuxErrno(errno);
                }
                  
              if(unlikely(lin.GetVerbose()))
                lin.DebugInfoStream() << "times(buf=0x" << std::hex << buf_addr << std::dec << ")" << std::endl;
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
            
      case 78: /* ARM specific gettimeofday syscall */
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

              struct arm_timeval target_tv;
              struct arm_timezone target_tz;

              ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0, lin.GetEndianness());
              if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);

              if(ret == 0)
                {
                  if(tv_addr)
                    {
                      lin.WriteMemory(tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
                    }
                  if(tz_addr)
                    {
                      lin.WriteMemory(tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
                    }
                }

              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                    << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
                    << ", tz = 0x" << std::hex << tz_addr << std::dec << ")"
                    << std::endl;
                }
	
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 108: /* ARM specific fstat syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "fstat"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              int32_t fd = GetSystemCallParam(lin, 0);
              address_type buf = GetSystemCallParam(lin, 1);
              sink << "(int fd=" << std::dec << fd << ", struct stat *buf=" << std::hex << buf << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_fd;
              int host_fd;
              address_type buf_address;
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
                  ret = -1;
                  target_errno = LINUX_ENOSYS;
                }

              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                    << "fstat(fd=" << target_fd
                    << ", buf_addr=0x" << std::hex << buf_address << std::dec
                    << ")" << std::endl;
                }
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 122: /* ARM specific uname syscall */
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
	
              struct arm_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              
              strncpy(value.sysname,  utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(value.nodename, utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(value.release,  utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(value.version,  utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(value.machine,  utsname.machine.c_str(), sizeof(value.machine) - 1);
              lin.WriteMemory(buf_addr, (uint8_t *)&value, sizeof(value));
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 195: /* ARM specific stat64 syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "stat64"; }
            void Describe( LINUX& lin, std::ostream& sink ) const
            {
              uint32_t path = GetSystemCallParam(lin, 0);
              address_type buf = GetSystemCallParam(lin, 1);
              sink << "(const char *pathname=" << std::hex << path
                   << ", struct stat *buf=" << std::hex << buf << ")";
            }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;

              address_type pathnameaddr = GetSystemCallParam(lin, 0);
              address_type buf_address = GetSystemCallParam(lin, 1);
              std::string pathname;
              if (lin.ReadString(pathnameaddr, pathname, true))
                {
                  struct arm_stat64 target_stat;
                  ret = Stat64(pathname.c_str(), &target_stat, lin.GetEndianness());
                  lin.WriteMemory(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                      
                  if(unlikely(lin.GetVerbose()))
                    {
                      lin.DebugInfoStream()
                        << "pathname = \"" << pathname << "\", buf_address = 0x" << std::hex << buf_address << std::dec
                        << std::endl;
                    }
                }
              else
                {
                  ret = -1;
                  target_errno = LINUX_ENOMEM;
                }
              
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 197: /* ARM specific fstat64 syscall */
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
                  struct arm_stat64 target_stat;
                  ret = Fstat64(host_fd, &target_stat, lin.GetEndianness());
                  if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
			
                  lin.WriteMemory(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                }
	
              if(unlikely(lin.GetVerbose()))
                {
                  lin.DebugInfoStream()
                    << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                    << std::endl;
                }
	
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;


      case 983041: {
        static struct : public SysCall {
          char const* GetName() const { return "breakpoint"; }
          void Describe( LINUX& lin, std::ostream& sink ) const { sink << "(...)"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983042: {
        static struct : public SysCall {
          char const* GetName() const { return "cacheflush"; }
          void Describe( LINUX& lin, std::ostream& sink ) const { sink << "(...)"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983043: {
        static struct : public SysCall {
          char const* GetName() const { return "usr26"; }
          void Describe( LINUX& lin, std::ostream& sink ) const { sink << "(...)"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983044: {
        static struct : public SysCall {
          char const* GetName() const { return "usr32"; }
          void Describe( LINUX& lin, std::ostream& sink ) const { sink << "(...)"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983045: {
        static struct : public SysCall {
          char const* GetName() const { return "set_tls"; }
          void Describe( LINUX& lin, std::ostream& sink ) const
          {
            uint32_t addr = GetSystemCallParam(lin, 0);
            //address_type buf = GetSystemCallParam(lin, 1);
            sink << "(void *tls_addr=" << std::hex << addr << std::dec << ")";
          }
          void Execute( LINUX& lin, int syscall_id ) const
          {
            uint32_t r0 = Host2Target(lin.GetEndianness(), GetSystemCallParam(lin, 0));
            lin.WriteMemory(0xffff0ff0UL, (uint8_t *)&(r0), sizeof(r0));
            SetARMSystemCallStatus(lin, (parameter_type)0, false);
          }
        } sc;
        return &sc;
      } break;
            
      }
          
      return 0;
    }
    
    bool SetSystemBlob( unisim::util::blob::Blob<address_type>* blob ) const
    {
      typedef unisim::util::blob::Section<address_type> Section;
      typedef unisim::util::blob::Segment<address_type> Segment;
      std::string mach = lin.GetUTSName().machine;
      if ((mach.compare("armv5") == 0) or (mach.compare("armv6") == 0) or (mach.compare("armv7") == 0))
        {
          if (blob == NULL) return false;
          // TODO: Check that the program/stack is not in conflict with the
          //   tls and cmpxchg interfaces
          // Set the tls interface, this requires a write into the memory
          //   system
          // The following instructions need to be added to memory:
          // 0xffff0fe0:  e59f0008   ldr r0, [pc, #(16 - 8)] @ TLS stored
          //                                                 @ at 0xffff0ff0
          // 0xffff0fe4:  e1a0f00e   mov pc, lr
          // 0xffff0fe8:  0
          // 0xffff0fec:  0
          // 0xffff0ff0:  0
          // 0xffff0ff4:  0
          // 0xffff0ff8:  0
          address_type tls_base_addr = 0xffff0fe0UL;
          static const uint32_t tls_buf_length = 7;
          static const uint32_t tls_buf[tls_buf_length] =
            {0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
          uint32_t *segment_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);
          uint32_t *section_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);

          for (unsigned int i = 0; i < tls_buf_length; ++i) {
            segment_tls_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), tls_buf[i]);
            section_tls_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), tls_buf[i]);
          }
          Segment *tls_if_segment =
            new Segment(Segment::TY_LOADABLE, Segment::SA_X, 4, tls_base_addr,
                        sizeof(tls_buf), sizeof(tls_buf), segment_tls_buf);
          Section *tls_if_section =
            new Section(Section::TY_UNKNOWN, Section::SA_A,
                        ".unisim.linux_os.interface.tls", 4, 0, tls_base_addr,
                        sizeof(tls_buf), section_tls_buf);

          // Set the cmpxchg (atomic compare and exchange) interface, the
          //   following instructions need to be added to memory:
          // 0xffff0fc0:  e5923000   ldr r3, [r2]
          // 0xffff0fc4:  e0533000   subs r3, r3, r0
          // 0xffff0fc8:  05821000   streq r1, [r2]
          // 0xffff0fcc:  e2730000   rsbs r0, r3, #0	; 0x0
          // 0xffff0fd0:  e1a0f00e   mov pc, lr
          address_type cmpxchg_base_addr = 0xffff0fc0UL;
          static const uint32_t cmpxchg_buf_length = 5;
          static const uint32_t cmpxchg_buf[cmpxchg_buf_length] = {
            0xe5923000UL,
            0xe0533000UL,
            0x05821000UL,
            0xe2730000UL,
            0xe1a0f00eUL
          };
          uint32_t *segment_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);
          uint32_t *section_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);

          for (unsigned int i = 0; i < cmpxchg_buf_length; ++i) {
            segment_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), cmpxchg_buf[i]);
            section_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), cmpxchg_buf[i]);
          }
          Section* cmpxchg_if_section =
            new Section(Section::TY_UNKNOWN, Section::SA_A,
                        ".unisim.linux_os.interface.cmpxchg",
                        4, 0, cmpxchg_base_addr,
                        sizeof(cmpxchg_buf), segment_cmpxchg_buf);
          Segment *cmpxchg_if_segment =
            new Segment(Segment::TY_LOADABLE, Segment::SA_X,
                        4, cmpxchg_base_addr,
                        sizeof(cmpxchg_buf), sizeof(cmpxchg_buf), section_cmpxchg_buf);

          blob->AddSegment(tls_if_segment);
          blob->AddSegment(cmpxchg_if_segment);
          blob->AddSection(tls_if_section);
          blob->AddSection(cmpxchg_if_section);
        }

      return true;
    }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_ARM_HH__
