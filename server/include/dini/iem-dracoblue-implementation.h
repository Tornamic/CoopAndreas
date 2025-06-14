/*
 *            Dini 1.6
 *       (c) Copyright 2006-2008 by DracoBlue
 *
 * @author    : DracoBlue (http://dracoblue.com)
 * @date      : 13th May 2006
 * @update    : 16th Sep 2008
 *
 * This file is provided as is (no warranties).
 *
 * It's released under the terms of MIT.
 *
 * Feel free to use it, a little message in
 * about box is honouring thing, isn't it?
 *
 *
 ****************************************************
 *				DracoBlue Implementation
 *			https://github.com/XQALPHA/INIEngine
 *
 *	<Author> : LYZENCORE-9
 *	<Date>	 : 31 Decembre 2024
 *	<Update> : DD MM YY
 *
 *	Library is under MIT license like the original one from fantastic DracoBlue !
 *
 *	Feel free to ask me in discord (distroxyde/lyzencore9_official)
 *	or follow me in github (https://github.com/LYZENCORE-9)
 ****************************************************
 */
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#if !defined(_INIENGINE_DRACOBLUE_LIB_H_) || !defined(_INIENGINE_DINI_H_)
#define _INIENGINE_DRACOBLUE_LIB_H_
#define _INIENGINE_DINI_H_

#define DINI_MODULE_MAX_FILE_LINES (512)
#define DINI_MODULE_MAX_STRING_SIZE (256)
#define DINI_MODULE_MAX_FILENAME_SIZE (128)


namespace INIEngine::Module::Dini {

class dini_file {
public:
    dini_file() { }

    bool FileCreate(char* filename);
    bool FileExists(char* filename);
    bool FileDelete(char* filename);
    bool FileRemove(char* filename);

    ~dini_file() { }
};

class dini_write {
public:
    dini_write() {}

    void WriteString(char *filename, char *key, char *value);
    void WriteInt(char *filename, char *key, int *value);
    void WriteFloat(char *filename, char *key, float *value);
    void WriteBool(char *filename, char *key, bool *value);

    ~dini_write() {}
};

class dini_read {
public:
    dini_read() { }

    void ReadString(char *filename, char *key, char *value);
    void ReadInt(char *filename, char *key, int *value);
    void ReadFloat(char *filename, char *key, float *value);
    void ReadBool(char *filename, char *key, bool *value);

    ~dini_read() { }
};

class dini_content {
public:
    dini_content() { }

    void CreateKey(char *filename, char *key);
    void RemoveKey(char *filename, char *key);

    ~dini_content() { }
};

}

#endif
