


#include <stdio.h>

#include ".\Parser\AstPrint.h"
#include ".\Parser\Parser.h"
#include ".\CPrime\CodePrint.h"
#include ".\CPrime\TCLint2.h"
#include "..\CPrime\Base\Path.h"
#include "UnitTest.h"

void Compile(const char* configFileName,
    const char* inputFileName,
    Options* options)
{
    TProgram program = TPROGRAM_INIT;


    printf("Parsing...\n");
    if (GetAST(inputFileName, configFileName, &program))
    {


        //TProgram_Analize(&program);

        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        SplitPath(inputFileName, drive, dir, fname, ext); // C4996

        char outjs[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 1];
        MakePath(outjs, drive, dir, fname, ".json");

        if (inputFileName != NULL &&
            program.MySourceDir.size == 1 &&
            strcmp(program.MySourceDir.pItems[0], inputFileName) == 0)
        {
            //nao passa o nome eh considera o dir
            inputFileName = NULL;
        }

        if (program.MySourceDir.size != 0)
        {
            //nao passa o nome eh considera o dir
            inputFileName = NULL;
        }

        //TProgram_PrintAstToFile(&program, outjs, inputFileName);

        strcat(dir, "\\out\\");
        MakePath(outjs, drive, dir, fname, ext);

        printf("Generating code for %s...\n", inputFileName);
        TProgram_PrintCodeToFile(&program, options, outjs, inputFileName);

        printf("Done!\n");
    }
    TProgram_Destroy(&program);
}

void PrintHelp()
{
    printf("Syntax: cprime [options] [file ...]\n");
    printf("\n");
    printf("Examples: cprime hello.c\n");
    printf("          cprime -config config.h hello.c\n");
    printf("          cprime -config config.h -P hello.c\n");
    printf("          cprime -E hello.c\n");
    printf("\n");
    printf("Options:\n");
    printf("-config FILE                          Configuration file.\n");
    printf("-help                                 Print this message.\n");
    printf("-E                                    Preprocess to console.\n");
    printf("-P                                    Preprocess to file.\n");

}
int main(int argc, char* argv[])
{
    printf("\n");
    printf("Version " __DATE__ "\n");
    printf("https://github.com/thradams/CPrime\n\n");


    //AllTests();

    if (argc < 2)
    {
        PrintHelp();
        return 1;
    }

    const char* configFileName = NULL;
    

    Options options = OPTIONS_INIT;
    bool bPrintPreprocessedToFile = false;
    bool bPrintPreprocessedToConsole = false;

    for (int i = 1; i < argc; i++)
    {
        const char * option = argv[i];
        if (strcmp(option, "-P") == 0)
        {
            bPrintPreprocessedToFile = true;
        }
        else if (strcmp(option, "-E") == 0)
        {
            bPrintPreprocessedToConsole = true;
        }
        else if (strcmp(option, "-help") == 0)
        {
            PrintHelp();
        }
        else if (strcmp(option, "-config") == 0)
        {
            if (i + i < argc)
            {
                configFileName = argv[i + 1];
                i++;
            }
            else
            {
                printf("missing file\n");
            }
        }
        else
        {
            const char* inputFileName = option;
            String inputFullPath = NULL;
            GetFullPath(inputFileName, &inputFullPath);

            if (bPrintPreprocessedToFile)
            {
                PrintPreprocessedToFile(inputFullPath, configFileName);
            }
            else if (bPrintPreprocessedToConsole)
            {
                PrintPreprocessedToConsole(inputFullPath, configFileName);
            }
            else
            {
                Compile(configFileName, inputFullPath, &options);
            }
            String_Destroy(&inputFullPath);

        }
    }





    return 0;
}

