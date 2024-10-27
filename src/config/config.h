#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "json.h"
#include "../common/utils.h"

void loadConfig(const char *path);
void freeConfig();

#define KeyChain (const char *[])

const jsonObj *getConfig();
int getConfigInt(const char *keychain[], int keycount);
const char *getConfigString(const char *keychain[], int keycount);
const ArrayInt getConfigArrayInt(const char *keychain[], int keycount);
const ArrayString getConfigArrayString(const char *keychain[], int keycount);


#endif // __CONFIG_H__
