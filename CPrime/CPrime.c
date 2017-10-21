


#include <stdio.h>
#include <time.h>  
#include "AstPrint.h"
#include "Parser.h"
#include "CodePrint.h"
#include "Path.h"
#include "UnitTest.h"


void AstPlayground(TProgram* program);

void Compile(const char* configFileName,
    const char* inputFileName,
    const char* outputFileName,
    Options* options,
    bool bPrintASTFile)
{
    TProgram program;
    TProgram_Init(&program);

    clock_t tstart = clock();

    printf("Parsing...\n");
    if (GetAST(inputFileName, configFileName, &program))
    {
        
        AstPlayground(&program);

        char drive[CPRIME_MAX_DRIVE];
        char dir[CPRIME_MAX_DIR];
        char fname[CPRIME_MAX_FNAME];
        char ext[CPRIME_MAX_EXT];
        SplitPath(inputFileName, drive, dir, fname, ext); // C4996


        printf("Generating code for %s...\n", inputFileName);
        if (outputFileName)
        {
            if (bPrintASTFile)
            {
                TProgram_PrintAstToFile(&program, outputFileName, inputFileName);
            }
            else
            {
                TProgram_PrintCodeToFile(&program, options, outputFileName, inputFileName);
            }
        }
        else
        {
            char outc[CPRIME_MAX_DRIVE + CPRIME_MAX_DIR + CPRIME_MAX_FNAME +CPRIME_MAX_EXT + 1];

            if (bPrintASTFile)
            {
                //faz um  arquivo com extensao json
                MakePath(outc, drive, dir, fname, ".json");
                TProgram_PrintAstToFile(&program, outc, inputFileName);
            }
            else
            {
                //gera em cima do proprio arquivo
                MakePath(outc, drive, dir, fname, ext);
                TProgram_PrintCodeToFile(&program, options, outc, inputFileName);
            }
        }

        printf("Done!\n");
        clock_t tend = clock();
        printf("%d seconds", (int)((tend - tstart) / CLOCKS_PER_SEC));

    }
    TProgram_Destroy(&program);
}

void PrintHelp()
{
    printf("Syntax: cp [options] [file ...]\n");
    printf("\n");
    printf("Examples: cp hello.c\n");
    printf("          cp -config config.h hello.c\n");
    printf("          cp -config config.h hello.c -o hello.c\n");
    printf("          cp -config config.h -P hello.c\n");
    printf("          cp -E hello.c\n");
    printf("          cp -P hello.c\n");
    printf("          cp -A hello.c\n");
    printf("\n");
    printf("Options:\n");
    printf("-config FILE                          Configuration file.\n");
    printf("-help                                 Print this message.\n");
    printf("-o FILE                               Sets ouput file name.\n");
    printf("-E                                    Preprocess to console.\n");
    printf("-P                                    Preprocess to file.\n");
    printf("-A                                    Output AST to file.\n");
    printf("-r                                    Reverts generation.\n");

}
int main(int argc, char* argv[])
{
    printf("\n");
    printf("C' Version " __DATE__ "\n");
    printf("https://github.com/thradams/CPrime\n\n");

#ifdef _DEBUG
    AllTests();
#endif
    if (argc < 2)
    {
        PrintHelp();
        return 1;
    }

    const char* configFileName = NULL;

    String outputFullPath = NULL;
    String inputFullPath = NULL;


    Options options = OPTIONS_INIT;
    //options.bShrink = true;

    bool bPrintPreprocessedToFile = false;
    bool bPrintPreprocessedToConsole = false;
    bool bPrintASTFile = false;

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
        else if (strcmp(option, "-r") == 0)
        {
            bPrintPreprocessedToConsole = true;
        }
        else if (strcmp(option, "-A") == 0)
        {
            bPrintASTFile = true;
        }
        else if (strcmp(option, "-help") == 0)
        {
            PrintHelp();
        }
        else if (strcmp(option, "-config") == 0)
        {
            if (i + 1 < argc)
            {
                configFileName = argv[i + 1];
                i++;
            }
            else
            {
                printf("missing file\n");
            }
        }
        else if (strcmp(option, "-o") == 0)
        {
            if (i + 1 < argc)
            {
                GetFullPath(argv[i + 1], &outputFullPath);
                i++;
            }
            else
            {
                printf("missing file\n");
            }
        }
        else
        {
            //const char* inputFileName = option;
            //String inputFullPath = NULL;
            GetFullPath(option, &inputFullPath);


            //String_Destroy(&inputFullPath);

        }

    }

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
        Compile(configFileName, inputFullPath, outputFullPath, &options, bPrintASTFile);
    }

    String_Destroy(&outputFullPath);
    String_Destroy(&inputFullPath);

    return 0;
}



bool EachStructDeclarator(TStructUnionSpecifier* pStruct,
    int *i,
    TSpecifierQualifierList **ppSpecifierQualifierList,
    TStructDeclarator** ppStructDeclarator)
{
    /*
     Exemplo:
      TSpecifierQualifierList *pSpecifierQualifierList = NULL;
      TStructDeclarator* pStructDeclarator = NULL;
      int i = 0;
      while (EachStructDeclarator(pStruct, &i, &pSpecifierQualifierList, &pStructDeclarator))
      {
            printf("%s\n", TDeclarator_GetName(pStructDeclarator->pDeclarator));
      }
    */
    if (*ppStructDeclarator != NULL)
    {
        goto Continue;
    }

    for (; *i < pStruct->StructDeclarationList.size; (*i)++)
    {
        TAnyStructDeclaration* pAnyStructDeclaration =
            pStruct->StructDeclarationList.pItems[*i];

        TStructDeclaration* pStructDeclaration =
            TAnyStructDeclaration_As_TStructDeclaration(pAnyStructDeclaration);

        if (pStructDeclaration != NULL)
        {
            *ppSpecifierQualifierList =
                &pStructDeclaration->SpecifierQualifierList;
            *ppStructDeclarator =
                pStructDeclaration->DeclaratorList.pHead;

            while (*ppStructDeclarator != NULL)
            {
                return true;
            Continue:
                *ppStructDeclarator = (*ppStructDeclarator)->pNext;
            }
        }
    }
    return false;
}



void AstPlayground(TProgram* program)
{

    // SymbolMap_Print(&program->GlobalScope);

    // TStructUnionSpecifier* p =
    //     SymbolMap_FindStructUnion(&program->GlobalScope, "X");


    // TTypeSpecifier* p2 = 
     //    SymbolMap_FindTypedefSpecifierTarget(&program->GlobalScope, "Y");
     //find struct X
     //find X
}
