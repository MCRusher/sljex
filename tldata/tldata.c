#include "tldata.h"

#include "../custom/export.h"

EXPORTED bool tldata_init(tldata_t * tld);
EXPORTED bool tldata_deinit(tldata_t * tld);
EXPORTED void * tldata_get(tldata_t tld);
EXPORTED bool tldata_set(tldata_t tld, void * data);

#ifdef _WIN32
bool tldata_init(tldata_t * tld) {
	*tld = TlsAlloc();
	return *tld != TLS_OUT_OF_INDEXES;
}
bool tldata_deinit(tldata_t * tld) {
	return !!TlsFree(*tld);
}
void * tldata_get(tldata_t tld) {
	return TlsGetValue(tld);
}
bool tldata_set(tldata_t tld, void * data) {
	return !!TlsSetValue(tld, data);
}
#elif defined(unix) || defined(__unix) || defined(__unix__)
bool tldata_init(tldata_t * tld) {
	return !pthread_key_create(tld, NULL);
}
bool tldata_deinit(tldata_t * tld) {
	return !pthread_key_delete(*tld);
}
void * tldata_get(tldata_t tld) {
	return pthread_getspecific(tld);
}
bool tldata_set(tldata_t tld, void * data) {
	return !pthread_setspecific(tld, data);
}
#elif __STDC_VERSION__ >=  201112L && !defined( __STDC_NO_THREADS__)
bool tldata_init(tldata_t * tld) {
	return tss_create(tld, NULL) == thrd_success;
}
bool tldata_deinit(tldata_t * tld) {
	tss_delete(*tld);
	return true;
}
void * tldata_get(tldata_t tld) {
	return tss_get(tld);
}
bool tldata_set(tldata_t tld, void * data) {
	return tss_set(tld, data) == thrd_success;
}
#endif
