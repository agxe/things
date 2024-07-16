/* 
// Config file sample content

#==============  Brightness    ==============#
userExposure		-600
userTempAdjust      -500
userColorTemp		2100
outputFilename      D:\output.txt

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PARSER_OK          0
#define FILE_PARSER_ERR         -1
#define FILE_PARSER_EOF         1

#define MAX_NUM_PARAM           (250)

#define PARAM_NAME_LENGTH       (64)
#define PARAM_VALUE_LENGTH      (256)

#define PARAM_TYPE_INT          (0)
#define PARAM_TYPE_FLOAT        (1)
#define PARAM_TYPE_DOUBLE       (2)
#define PARAM_TYPE_BOOL         (3)
#define PARAM_TYPE_CHAR         (4)
#define PARAM_TYPE_STRING       (5)

#if defined(_WIN32) || defined(_WIN64)
#define STRNCPY(DST,DSIZE,SRC,SSIZE)                  strncpy_s(DST,DSIZE,SRC,SSIZE)
#define SSCANF_PARSE(BUF,FMT,ARG1,SIZE1,ARG2,SIZE2)   sscanf_s(BUF,FMT,ARG1,SIZE1,ARG2,SIZE2)
#else
#define STRNCPY(DST,DSIZE,SRC,SSIZE)                  strncpy(DST,SRC,SSIZE)
#define SSCANF_PARSE(BUF,FMT,ARG1,SIZE1,ARG2,SIZE2)   sscanf(BUF,FMT,ARG1,ARG2)
#endif

#define STRNCMP(DST,SRC,SIZE)   strncmp(DST,SRC,SIZE)
#define STRCMP (DST,SRC)        strcmp(DST,SRC)
#define STRLEN (DST)            strlen(DST)


//user defined parameter list
typedef struct tagCONFIG_PARAM {
    int userExposure;
    int userTempAdjust;
    int userColorTemp;
    char outFilename[PARAM_VALUE_LENGTH];
} config_param_t;


//file parser definition
typedef struct tagPARAM_LIST {
    char    param_name[PARAM_NAME_LENGTH];
    int     param_type;
} PARAM_LIST;

class FileParser {
    typedef union tagPARAM_VALUE {
        char    svalue[PARAM_VALUE_LENGTH];
        int     ivalue;
        float   fvalue;
        double  dvalue;
        char    cvalue;
        int     bvalue;
    } PARAM_VALUE;

    PARAM_LIST *m_pParamList;
    PARAM_VALUE *m_ParamValue;
    bool m_ParamAvail[MAX_NUM_PARAM];

private:
    FILE *fp;

    int OpenFile(char *filename, const char *mode)
    {
        FILE *tfp = NULL;

        if (!(filename && mode)) {
            return FILE_PARSER_ERR;
        }

#if defined(_WIN32) || defined(_WIN64)
        int retval;
        if ((retval = fopen_s(&tfp, filename, mode)) != 0) {
            return FILE_PARSER_ERR;
        }
#else
        if ((tfp = fopen(filename, mode)) == NULL) {
            return FILE_PARSER_ERR;
        }
#endif

        fp = tfp;

        return FILE_PARSER_OK;
    }

    int ReadLine(char *linebuf, unsigned int bufsize)
    {
        char *retval = 0;

        if (!(linebuf && fp && bufsize > 0)) {
            return FILE_PARSER_ERR;
        }

        if (feof(fp)) {
            return FILE_PARSER_EOF;
        }

        *linebuf = '\0';
        if ((retval = fgets(linebuf, bufsize, fp)) == NULL) {
            return FILE_PARSER_EOF;
        }

        return FILE_PARSER_OK;
    }

    int CloseFile()
    {
        if (fp) {
            fclose(fp);
            fp = NULL;
        }

        return FILE_PARSER_OK;
    }

public:
    FileParser() {
        fp = 0;
        m_ParamValue = NULL;

        if (NULL == m_ParamValue) {
            m_ParamValue = new PARAM_VALUE[MAX_NUM_PARAM];
        }

        memset(m_ParamAvail, 0, (sizeof(bool) * MAX_NUM_PARAM));
        memset(m_ParamValue, 0, (sizeof(PARAM_VALUE) * MAX_NUM_PARAM));
    }

    ~FileParser() {
        CloseFile();

        if (m_ParamValue) {
            delete[] m_ParamValue;
            m_ParamValue = NULL;
        }
    }

    int Init(char *filename, PARAM_LIST *ParamList);
    void GetValue(const char *param_name, void *param_value);
};

int FileParser::Init(char *filename, PARAM_LIST *ParamList)
{
    const int bufsize = 1024;
    char param_name[PARAM_NAME_LENGTH] = { 0 };
    char param_value[PARAM_VALUE_LENGTH] = { 0 };
    char linebuf[bufsize] = { 0 };
    int retval = FILE_PARSER_OK;
    int n = 0;

    if (filename == NULL || ParamList == NULL) {
        return FILE_PARSER_ERR;
    }

    if (FILE_PARSER_OK != (retval = OpenFile(filename, "r"))) {
        return retval;
    }

    m_pParamList = ParamList;

    do {
        retval = ReadLine(linebuf, bufsize);
        if (FILE_PARSER_ERR == retval) {
            break;
        } else if (FILE_PARSER_EOF == retval) {
            retval = FILE_PARSER_OK;
            break;
        }

        SSCANF_PARSE(linebuf, "%s %s",
            param_name,  (unsigned)sizeof(param_name),
            param_value, (unsigned)sizeof(param_value));

        if (param_name[0] == '#' || param_name[0] == '\0' || param_name[0] == '\n') {
            continue;
        }

        while ((n < MAX_NUM_PARAM) && STRNCMP(m_pParamList[n].param_name, param_name, PARAM_NAME_LENGTH)) {
            n += 1;
        }

        if (n >= MAX_NUM_PARAM) {
            n = 0;
            continue;
        }

        memset(param_name, 0, PARAM_NAME_LENGTH);

        switch (m_pParamList[n].param_type) {
        case PARAM_TYPE_INT:
            m_ParamValue[n].ivalue = atoi(param_value);
            break;

        case PARAM_TYPE_FLOAT:
            m_ParamValue[n].fvalue = (float)atof(param_value);
            break;

        case PARAM_TYPE_DOUBLE:
            m_ParamValue[n].dvalue = atof(param_value);
            break;

        case PARAM_TYPE_BOOL:
            m_ParamValue[n].bvalue = atoi(param_value);
            break;

        case PARAM_TYPE_CHAR:
            STRNCPY(&m_ParamValue[n].cvalue, 1, param_value, 1);
            break;

        case PARAM_TYPE_STRING:
            STRNCPY(m_ParamValue[n].svalue, PARAM_VALUE_LENGTH, param_value, PARAM_VALUE_LENGTH);
            break;
        }

        m_ParamAvail[n] = true;

        n = 0;
    } while (1);

    CloseFile();

    return retval;
}

void FileParser::GetValue(const char *param_name, void *param_value)
{
    int n = 0;

    if (param_name == NULL || param_value == NULL) {
        return;
    }

    while (STRNCMP(m_pParamList[n].param_name, param_name, PARAM_NAME_LENGTH)) {
        n += 1;
    }

    if (true == m_ParamAvail[n]) {

        switch (m_pParamList[n].param_type) {
        case PARAM_TYPE_INT:
            *(int *)param_value = m_ParamValue[n].ivalue;
            break;

        case PARAM_TYPE_FLOAT:
            *(float *)param_value = m_ParamValue[n].fvalue;
            break;

        case PARAM_TYPE_DOUBLE:
            *(double *)param_value = m_ParamValue[n].fvalue;
            break;

        case PARAM_TYPE_BOOL:
            *(int *)param_value = m_ParamValue[n].ivalue;
            break;

        case PARAM_TYPE_CHAR:
            *(char *)param_value = m_ParamValue[n].ivalue;
            break;

        case PARAM_TYPE_STRING:
            STRNCPY((char*)param_value, PARAM_VALUE_LENGTH, m_ParamValue[n].svalue, PARAM_VALUE_LENGTH);
            break;
        }
    }
}

/*----------------------- Test code ------------------------*/

bool ParseFileParam(const char *cfgname, config_param_t *cfgprm)
{
	PARAM_LIST paramList[MAX_NUM_PARAM] = {
		//param name, param type
		{ "userExposure",         PARAM_TYPE_INT },
		{ "userTempAdjust",       PARAM_TYPE_INT },
		{ "userColorTemp",        PARAM_TYPE_INT },
        { "outputFilename",       PARAM_TYPE_STRING },
	};

	FileParser fileParser;
	if (FILE_PARSER_OK != fileParser.Init((char*)cfgname, paramList)) {
		printf("Config file is not available \n");
		return false;
	}

	fileParser.GetValue("userExposure", &cfgprm->userExposure);
	fileParser.GetValue("userTempAdjust", &cfgprm->userTempAdjust);
	fileParser.GetValue("userColorTemp", &cfgprm->userColorTemp);
    fileParser.GetValue("outputFilename", &cfgprm->outFilename);

	return true;
}

/*

//Sample code

int main(int argc, const char **argv)
{
	const char *cfgname = argv[1];

    config_param_t config_param = { 0 };
	if (true != ParseFileParam(cfgname, &config_param)) {
		return -1;
	}


	return 0;
}

*/