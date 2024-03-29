#include <stdio.h>
#include <assert.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

enum errors {
    SUCCESS = 0,
    WRONG_FILE = 1,
};

int getFileSize(const char *inPath);

int readFile(const char inPath[], char *text, size_t textSize);

void commandPrint(FILE *disasm, char *name, char **cur, int argType);

int main(const int argc, char * const argv[]) {
    char *binFilePath = argv[argc - 1];
    char *pathPoint = strrchr(binFilePath, '.');

    if (strcmp(pathPoint, ".bin") != 0) {
        printf("Wrong file type");
        return WRONG_FILE;
    }

    char disasmPath[FILENAME_MAX] = "";
    strncpy(disasmPath, binFilePath, pathPoint - binFilePath);
    strcpy(disasmPath + (pathPoint - binFilePath), "_dis.asm");

    FILE *disasm = fopen(disasmPath, "w");

    size_t binSize = getFileSize(binFilePath);
    char *buffer = (char *) calloc(binSize, sizeof(char) + 1);
    char *cur = buffer;

    if (readFile(binFilePath, buffer, binSize)) perror("Error:");

    while (cur < buffer + binSize) {
#define DEF_CMD(name, num, argType, code, asmCode) \
        case num: { \
            commandPrint(disasm, #name, &cur, argType); \
            break; \
        }

        switch (*cur) {

#include"../asmCompiler/commands.h"
            default: printf("Syntax error");
        }
    }
#undef DEF_CMD

    free(buffer);
    fclose(disasm);
    return 0;
}

int getFileSize(const char *inPath) {
    assert(inPath != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fseek(myFile, 0, SEEK_END);
    size_t textSize = ftell(myFile);
    fclose(myFile);
    return textSize;
}

int readFile(const char inPath[], char *text, size_t textSize) {
    assert(inPath != "");
    assert(text != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fread(text, 1, textSize, myFile);
    fclose(myFile);
    return 0;
}

//!
//! \param[out] disasm File buffer for out
//! \param[in] name command name
//! \param cur binary buffer
//! \param[in argType argument type
void commandPrint(FILE *disasm, char *name, char **cur, int argType) {
    fprintf(disasm, "%s", name);
            (*cur)++;
            if (argType == 1) {
                fprintf(disasm, " %d", *((int *) *cur));
                *cur += sizeof(int);
            }
            else if (argType == 2) {
                fprintf(disasm, " %cx", **cur);
                (*cur)++;
            }
            fprintf(disasm, "\n");
}