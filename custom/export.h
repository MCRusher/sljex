#ifndef CUSTOM_EXPORT_H

#ifdef EXPORT
	#ifdef MSVC
		#define EXPORTED __declspec(dllexport)
	#else
		#define EXPORTED __attribute__ ((dllexport))
	#endif
#else
	#define EXPORTED
#endif //EXPORT

#endif