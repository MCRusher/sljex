#ifndef CUSTOM_OS_H
#define CUSTOM_OS_H

//Operating System/os category tags are always in full uppercase
// OS/OS category traits that vary (like whether the os group is simulated) are in Pascal case
//EX: OS_UNIX_APPLE_IOS = OS, OS_UNIX_APPLE_Simulator = OS trait
#if defined(MSDOS) || defined(__MSDOS) || defined(_MSDOS) || defined(__MSDOS__)\
 || defined(DOS) || defined(__DOS) || defined(_DOS) || defined(__DOS__)
	#define OS_MSDOS 1
#elif defined(_WIN32)
	#define OS_WINDOWS 1
	#if defined(_WIN64)
		#define OS_WINDOWS_64 1
	#endif
#elif defined(unix) || defined(__unix) || defined(__unix__)
	#define OS_UNIX 1
	#if defined(linux) || defined(__linux) || defined(__linux__)
		#define OS_UNIX_LINUX 1
		#if defined(ANDROID) || defined(__ANDROID) || defined(__ANDROID__)
			#define OS_UNIX_LINUX_ANDROID 1
		#endif
	#elif defined(BSD)
		#define OS_UNIX_BSD 1
		#if defined(DragonFly) || defined(__DragonFly) || defined(__DragonFly__)
			#define OS_UNIX_BSD_DRAGONFLY 1
		#elif defined(FreeBSD) || defined(__FreeBSD) || defined(__FREEBSD__)
			#define OS_UNIX_BSD_FREEBSD 1
		#elif defined(NetBSD) || defined(__NetBSD) || defined(__NetBSD__)
			#define OS_UNIX_BSD_NETBSD 1
		#elif defined(OpenBSD) || defined(__OpenBSD) || defined(_OpenBSD__)
			#define OS_UNIX_BSD_OPENBSD 1
		#endif
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#define OS_UNIX 1
	#define OS_UNIX_APPLE 1
	#if defined(TARGET_OS_MAC)
		#if defined(TARGET_OS_NANO) && !defined(TARGET_OS_WATCH)
			#define OS_UNIX_APPLE_NANO 1
		#elif defined(TARGET_OS_OSX)
			#define OS_UNIX_APPLE_MAC 1
			#if defined(TARGET_OS_MACCATALYST) || defined(TARGET_OS_UIKITFORMAC)
				#define OS_UNIX_APPLE_MAC_CATALYST 1
			#endif
		#elif defined(TARGET_OS_IPHONE)
			#if defined(TARGET_OS_SIMULATOR) || defined(TARGET_IPHONE_SIMULATOR)
				#define OS_UNIX_APPLE_Simulator 1
			#endif
			#if defined(TARGET_OS_IOS)
				#define OS_UNIX_APPLE_IOS 1
			#elif defined(TARGET_OS_TV)
				#define OS_UNIX_APPLE_TV 1
			#elif defined(TARGET_OS_WATCH)
				#define OS_UNIX_APPLE_WATCH 1
			#elif defined(TARGET_OS_BRIDGE)
				#define OS_UNIX_APPLE_BRIDGE 1
			#else
				#define OS_UNIX_APPLE_UNKNOWN 1
			#endif
		#elif defined(TARGET_OS_WIN32)
			#define OS_UNIX_APPLE_WINDOWS 1
		#elif defined(TARGET_OS_UNIX)
			#define OS_UNIX_APPLE_UNIX 1
		#else
			#define OS_UNIX_APPLE_UNKNOWN 1
		#endif
	#else
		#define OS_UNIX_APPLE_UNKNOWN 1
	#endif
#else
	#define OS_UNKNOWN 1
#endif

#endif