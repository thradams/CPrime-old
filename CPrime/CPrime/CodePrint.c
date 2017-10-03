
#include "CodePrint.h"
#include "..\Base\Array.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\Base\Path.h"



#include "Templates\AllTemplates.h"


bool bExpandMacros;

void Options_Destroy(Options* options) _default
{
}

static bool TInitializerListItem_CodePrint2(TProgram* program,
    Options * options,
    TSpecifierQualifierList* pSpecifierQualifierList,
    bool bIsPointer,
    TInitializerListItem* p,
    bool b,
    StrBuilder* fp);



bool TSpecifierQualifierList_CodePrint(TProgram* program, Options * options, TSpecifierQualifierList* pDeclarationSpecifiers, bool b, StrBuilder* fp);

bool TTypeName_CodePrint(TProgram* program, Options * options, TTypeName* p, bool b, StrBuilder* fp);

static bool TInitializer_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializer* pTInitializer,
    bool b,
    StrBuilder* fp);

static bool TInitializerList_CodePrint(TProgram* program,
    Options * options,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializerList*p,
    bool b,
    StrBuilder* fp);


static bool TInitializerListItem_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializerListItem* p,
    bool b,
    StrBuilder* fp);


static bool TTypeQualifierList_CodePrint(TProgram* program, Options * options, TTypeQualifierList* p, bool b, StrBuilder* fp);
//static bool TInitializerList_CodePrint(TProgram* program, Options * options, TTypeSpecifier* pTypeSpecifier, bool bIsPointer, TInitializerList*p, bool b, StrBuilder* fp);
//static bool TInitializerListType_CodePrint(TTypeSpecifier* pTypeSpecifier, bool b, StrBuilder* fp);
static bool TDeclarator_CodePrint(TProgram* program, Options * options, TDeclarator* p, bool b, bool bPrintName, StrBuilder* fp);
static bool TAnyDeclaration_CodePrint(TProgram* program, Options * options, TAnyDeclaration *pDeclaration, bool b, StrBuilder* fp);
static bool TTypeSpecifier_CodePrint(TProgram* program, Options * options, TTypeSpecifier* p, bool b, StrBuilder* fp);
static bool TAnyStructDeclaration_CodePrint(TProgram* program, Options * options, TAnyStructDeclaration* p, bool b, StrBuilder* fp);
static bool TTypeQualifier_CodePrint(TProgram* program, Options * options, TTypeQualifier* p, bool b, StrBuilder* fp);
static bool TDeclaration_CodePrint(TProgram* program, Options * options, TDeclaration* p, bool b, StrBuilder* fp);
static bool TExpression_CodePrint(TProgram* program, Options * options, TExpression * p, const char* name, bool b, StrBuilder* fp);
static bool TStatement_CodePrint(TProgram* program, Options * options, TStatement * p, bool b, StrBuilder* fp);
static bool TBlockItem_CodePrint(TProgram* program, Options * options, TBlockItem * p, bool b, StrBuilder* fp);

static bool TPointer_CodePrint(TProgram* program, Options * options, TPointer* pPointer, bool b, StrBuilder* fp);
static bool TParameter_CodePrint(TProgram* program, Options * options, TParameter* p, bool b, StrBuilder* fp);
//static bool TInitializerListItem_CodePrint(TProgram* program, Options * options, TTypeSpecifier* pTypeSpecifier, bool bIsPointer, TInitializerListItem* p, bool b, StrBuilder* fp);

static bool bInclude = true;
static int IncludeLevel = 0;

void Output_Append(StrBuilder* p,
    const char* source)
{
    if (bInclude  && IncludeLevel == 0)
        StrBuilder_Append(p, source);
}

static void TNodeClueList_CodePrint(Options* options, TScannerItemList* list,
    StrBuilder* fp)
{

    if (options->bDontPrintClueList)
    {
        if (list->pHead != NULL)
        {
            //Output_Append(fp, " ");
        }
        return;
    }

    ForEachListItem(ScannerItem, pNodeClue, list)
    {
        switch (pNodeClue->token)
        {
        case TK_PRE_INCLUDE:

            Output_Append(fp, pNodeClue->lexeme.c_str);
            Output_Append(fp, "\n");
            //if (pNodeClue->bActive)
            
            IncludeLevel++;
            //}
            break;

        case TK_FILE_EOF:
            IncludeLevel--;
            //ASSERT(IncludeLevel > 0);
            //bInclude = true;
            break;
        case TK_PRE_DEFINE:
        case TK_PRE_UNDEF:
        case TK_PRE_PRAGMA:
        case TK_PRE_IF:
        case TK_PRE_ENDIF:
        case TK_PRE_ELSE:
        case TK_PRE_IFDEF:
        case TK_PRE_IFNDEF:
        case TK_PRE_ELIF:
            Output_Append(fp, pNodeClue->lexeme.c_str);
            Output_Append(fp, "\n");
            break;

        case TK_COMMENT:
            if (options->bIncludeComments)
            {
                Output_Append(fp, pNodeClue->lexeme.c_str);
            }
            else
            {
                Output_Append(fp, " ");
            }

            break;

        case TK_LINE_COMMENT:
            if (options->bIncludeComments)
            {
                Output_Append(fp, pNodeClue->lexeme.c_str);
            }
            else
            {
                Output_Append(fp, "\n");
            }
            break;

        case TK_BREAKLINE:
            Output_Append(fp, "\n");

            break;

        case TK_MACRO_CALL:
            if (options->bExpandMacros)
            {

            }
            else
            {
                Output_Append(fp, pNodeClue->lexeme.c_str);
                bInclude = false;
            }
            break;


        case TK_MACRO_EOF:
            if (options->bExpandMacros)
            {

            }
            else
            {
                bInclude = true;
            }


            break;

        case TK_SPACES:
            Output_Append(fp, pNodeClue->lexeme.c_str);
            break;

            //case NodeClueTypeNone:      
        default:
            Output_Append(fp, pNodeClue->lexeme.c_str);
            break;
        }

    }
}



void BuildEnumSpecifierInitialization(TProgram* program, Options * options,
    TEnumSpecifier* pTEnumSpecifier,
    StrBuilder* strBuilder)
{
    Output_Append(strBuilder, "0");
}





static bool TCompoundStatement_CodePrint(TProgram* program,
    Options * options,
    TCompoundStatement * p,
    bool b,
    StrBuilder* fp)
{



    //
    b = true;

    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "{");

    for (size_t j = 0; j < p->BlockItemList.size; j++)
    {
        TBlockItem *pBlockItem = p->BlockItemList.pItems[j];
        TBlockItem_CodePrint(program, options, pBlockItem, j > 0, fp);
    }

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "}");
    return b;
}


static bool TLabeledStatement_CodePrint(TProgram* program, Options * options, TLabeledStatement * p, bool b, StrBuilder* fp)
{
    b = true;

    if (p->token == TK_CASE)
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "case");
        if (p->pStatementOpt)
        {
            b = TExpression_CodePrint(program, options, p->pExpression, "", false, fp);
        }
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, ":");
        b = TStatement_CodePrint(program, options, p->pStatementOpt, false, fp);
    }
    else if (p->token == TK_DEFAULT)
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "default");
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, ":");
        b = TStatement_CodePrint(program, options, p->pStatementOpt, false, fp);
    }
    else if (p->token == TK_IDENTIFIER)
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, p->Identifier);

        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, ":");

        TStatement_CodePrint(program, options, p->pStatementOpt, false, fp);
    }


    return b;
}

static bool TForStatement_CodePrint(TProgram* program, Options * options, TForStatement * p, bool b, StrBuilder* fp)
{
    b = true;
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "for");
    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "(");

    if (p->pInitDeclarationOpt)
    {
        TAnyDeclaration_CodePrint(program, options, p->pInitDeclarationOpt, b, fp);
        if (p->pExpression2)
        {
            b = TExpression_CodePrint(program, options, p->pExpression2, "expr2", true, fp);
        }
        TNodeClueList_CodePrint(options, &p->ClueList2, fp);
        Output_Append(fp, ";");
        b = TExpression_CodePrint(program, options, p->pExpression3, "expr3", b, fp);
    }
    else
    {
        b = TExpression_CodePrint(program, options, p->pExpression1, "expr1", true, fp);
        TNodeClueList_CodePrint(options, &p->ClueList2, fp);
        Output_Append(fp, ";");
        b = TExpression_CodePrint(program, options, p->pExpression2, "expr2", b, fp);
        TNodeClueList_CodePrint(options, &p->ClueList3, fp);
        Output_Append(fp, ";");
        b = TExpression_CodePrint(program, options, p->pExpression3, "expr3", b, fp);
    }

    TNodeClueList_CodePrint(options, &p->ClueList4, fp);
    Output_Append(fp, ")");

    b = TStatement_CodePrint(program, options, p->pStatement, false, fp);

    return b;
}


static bool TWhileStatement_CodePrint(TProgram* program, Options * options, TWhileStatement * p, bool b, StrBuilder* fp)
{
    b = true;
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "while");
    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "(");
    b = TExpression_CodePrint(program, options, p->pExpression, "expr", false, fp);
    TNodeClueList_CodePrint(options, &p->ClueList2, fp);
    Output_Append(fp, ")");
    b = TStatement_CodePrint(program, options, p->pStatement, false, fp);
    return b;
}



static bool TDoStatement_CodePrint(TProgram* program, Options * options, TDoStatement * p, bool b, StrBuilder* fp)
{
    b = true;
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "do");

    b = TStatement_CodePrint(program, options, p->pStatement, false, fp);

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "while");
    TNodeClueList_CodePrint(options, &p->ClueList2, fp);
    Output_Append(fp, "(");
    b = TExpression_CodePrint(program, options, p->pExpression, "expr", false, fp);
    TNodeClueList_CodePrint(options, &p->ClueList3, fp);
    Output_Append(fp, ")");
    TNodeClueList_CodePrint(options, &p->ClueList4, fp);
    Output_Append(fp, ";");

    return b;
}


static bool TExpressionStatement_CodePrint(TProgram* program, Options * options, TExpressionStatement * p, bool b, StrBuilder* fp)
{
    TExpression_CodePrint(program, options, p->pExpression, "", b, fp);

    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, ";");

    return true;
}


static bool TJumpStatement_CodePrint(TProgram* program, Options * options, TJumpStatement * p, bool b, StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    switch (p->token)
    {
    case TK_GOTO:
        Output_Append(fp, "goto");
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, p->Identifier);
        break;
    case  TK_CONTINUE:
        Output_Append(fp, "continue");
        break;
    case TK_BREAK:
        Output_Append(fp, "break");
        break;
    case TK_RETURN:
        Output_Append(fp, "return");
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        TExpression_CodePrint(program, options, p->pExpression, "", false, fp);
        break;

    default:
        ASSERT(false);
    }


    TNodeClueList_CodePrint(options, &p->ClueList2, fp);
    Output_Append(fp, ";");

    return true;
}

static bool TAsmStatement_CodePrint(TProgram* program, Options * options, TAsmStatement * p, bool b, StrBuilder* fp)
{
    Output_Append(fp, "\"type\":\"asm-statement\"");
    return true;
}

static bool TSwitchStatement_CodePrint(TProgram* program, Options * options, TSwitchStatement * p, bool b, StrBuilder* fp)
{
    b = true;
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "switch");

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "(");

    b = TExpression_CodePrint(program, options, p->pConditionExpression, "expr", false, fp);

    TNodeClueList_CodePrint(options, &p->ClueList2, fp);
    Output_Append(fp, ")");

    b = TStatement_CodePrint(program, options, p->pExpression, false, fp);
    return b;
}


static bool TIfStatement_CodePrint(TProgram* program, Options * options, TIfStatement * p, bool b, StrBuilder* fp)
{
    b = true;
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "if");

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "(");


    b = TExpression_CodePrint(program, options, p->pConditionExpression, "expr", false, fp);

    TNodeClueList_CodePrint(options, &p->ClueList2, fp);
    Output_Append(fp, ")");

    if (p->pStatement->Type != TCompoundStatement_ID)
        Output_Append(fp, "");

    if (p->pStatement)
    {
        b = TStatement_CodePrint(program, options, p->pStatement, false, fp);
    }

    if (p->pElseStatement)
    {
        TNodeClueList_CodePrint(options, &p->ClueList3, fp);
        Output_Append(fp, "else");
        b = TStatement_CodePrint(program, options, p->pElseStatement, false, fp);
    }

    return b;
}

static bool TStatement_CodePrint(TProgram* program, Options * options, TStatement *  p, bool b, StrBuilder* fp)
{
    if (p == NULL)
    {
        return false;
    }

    switch (p->Type)
    {
    case TExpressionStatement_ID:
        b = TExpressionStatement_CodePrint(program, options, (TExpressionStatement*)p, b, fp);
        break;

    case TSwitchStatement_ID:
        b = TSwitchStatement_CodePrint(program, options, (TSwitchStatement*)p, b, fp);
        break;

    case TLabeledStatement_ID:
        b = TLabeledStatement_CodePrint(program, options, (TLabeledStatement*)p, b, fp);
        break;

    case TForStatement_ID:
        b = TForStatement_CodePrint(program, options, (TForStatement*)p, b, fp);
        break;

    case TJumpStatement_ID:
        b = TJumpStatement_CodePrint(program, options, (TJumpStatement*)p, b, fp);
        break;

    case TAsmStatement_ID:
        b = TAsmStatement_CodePrint(program, options, (TAsmStatement*)p, b, fp);
        break;

    case TCompoundStatement_ID:
        b = TCompoundStatement_CodePrint(program, options, (TCompoundStatement*)p, b, fp);
        break;

    case TIfStatement_ID:
        b = TIfStatement_CodePrint(program, options, (TIfStatement*)p, b, fp);
        break;

    case TDoStatement_ID:
        TDoStatement_CodePrint(program, options, (TDoStatement*)p, b, fp);
        break;

    default:
        ASSERT(false);
        break;
    }

    return b;
}

static bool TBlockItem_CodePrint(TProgram* program, Options * options, TBlockItem *  p, bool b, StrBuilder* fp)
{
    if (p == NULL)
    {
        ASSERT(false);
        return false;
    }


    switch (p->Type)
    {
    case TEofDeclaration_ID:
        break;

    case TStaticAssertDeclaration_ID:
        break;

    case TSwitchStatement_ID:

        b = TSwitchStatement_CodePrint(program, options, (TSwitchStatement*)p, false, fp);

        break;

    case TJumpStatement_ID:

        b = TJumpStatement_CodePrint(program, options, (TJumpStatement*)p, false, fp);

        break;

    case TForStatement_ID:

        b = TForStatement_CodePrint(program, options, (TForStatement*)p, false, fp);

        break;

    case TIfStatement_ID:

        b = TIfStatement_CodePrint(program, options, (TIfStatement*)p, false, fp);

        break;

    case TWhileStatement_ID:

        b = TWhileStatement_CodePrint(program, options, (TWhileStatement*)p, b, fp);

        break;

    case TDoStatement_ID:

        b = TDoStatement_CodePrint(program, options, (TDoStatement*)p, false, fp);

        break;

    case TDeclaration_ID:
        b = TDeclaration_CodePrint(program, options, (TDeclaration*)p, false, fp);
        //Output_Append(fp, "\n");
        break;

    case TLabeledStatement_ID:

        b = TLabeledStatement_CodePrint(program, options, (TLabeledStatement*)p, false, fp);

        break;

    case TCompoundStatement_ID:
        b = TCompoundStatement_CodePrint(program, options, (TCompoundStatement*)p, false, fp);
        break;

    case TExpressionStatement_ID:

        b = TExpressionStatement_CodePrint(program, options, (TExpressionStatement*)p, false, fp);

        break;


    case TAsmStatement_ID:

        b = TAsmStatement_CodePrint(program, options, (TAsmStatement*)p, false, fp);

        break;

    default:
        ASSERT(false);
        break;
    }

    return b;
}



bool GetType(const char* source,
    StrBuilder* strBuilderType)
{

    while (*source  &&
        *source != '_')
    {
        StrBuilder_AppendChar(strBuilderType, *source);
        source++;
    }


    return *source == '_';
}


bool GetTypeAndFunction(const char* source,
    StrBuilder* strBuilderType,
    StrBuilder* strBuilderFunc)
{

    while (*source  &&
        *source != '_')
    {
        StrBuilder_AppendChar(strBuilderType, *source);
        source++;
    }

    while (*source)
    {
        StrBuilder_AppendChar(strBuilderFunc, *source);
        source++;
    }

    return *source == '_';
}




static bool TPostfixExpressionCore_CodePrint(TProgram* program,
    Options * options,
    TPostfixExpressionCore * p,
    bool b,
    StrBuilder* fp)
{


    b = false;

    if (p->pExpressionLeft)
    {
        b = TExpression_CodePrint(program, options, p->pExpressionLeft, "l", b, fp);
    }


    {
        bool bIsPointer = false;
        TTypeName *pTypeName = NULL;
        if (p->pTypeName)
        {
            TNodeClueList_CodePrint(options, &p->ClueList0, fp);
            Output_Append(fp, "(");
            TTypeName_CodePrint(program, options, p->pTypeName, b, fp);
            TNodeClueList_CodePrint(options, &p->ClueList1, fp);
            Output_Append(fp, ")");

            //pSpecifierQualifierList = &p->pTypeName->SpecifierQualifierList;
            bIsPointer = TPointerList_IsPointer(&p->pTypeName->Declarator.PointerList);

            //falta imprimeir typename
            //TTypeName_Print*
            b = TInitializerList_CodePrint(program,
                options,
                (TDeclarationSpecifiers*)&p->pTypeName->SpecifierQualifierList,
                bIsPointer,
                &p->InitializerList,
                b,
                fp);
        }

    }

    switch (p->token)
    {
    case TK_FULL_STOP:
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, ".");
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, p->Identifier);
        b = true;
        break;
    case TK_ARROW:
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "->");
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, p->Identifier);
        b = true;
        break;

    case TK_LEFT_SQUARE_BRACKET:
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "[");
        b = TExpression_CodePrint(program, options, p->pExpressionRight, "r", b, fp);
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, "]");
        break;

    case TK_LEFT_PARENTHESIS:
        //Do lado esquerdo vem o nome da funcao p->pExpressionLeft
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "(");
        b = TExpression_CodePrint(program, options, p->pExpressionRight, "r", b, fp);
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, ")");
        break;

    case TK_PLUSPLUS:
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "++");
        b = true;
        break;
    case TK_MINUSMINUS:
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "--");
        b = true;
        break;
        break;
    }


    b = true;
    if (p->pNext)
    {
        b = TPostfixExpressionCore_CodePrint(program, options, p->pNext, false, fp);
    }

    b = true;
    return b;
}

static bool TExpression_CodePrint(TProgram* program, Options * options, TExpression *  p,
    const char* name,
    bool b,
    StrBuilder* fp)
{
    if (p == NULL)
    {
        //ASSERT(false);
        return b;
    }

    b = false;

    switch (p->Type)
    {
        CASE(TBinaryExpression) :
        {
            TBinaryExpression* pBinaryExpression = (TBinaryExpression*)p;

            b = TExpression_CodePrint(program, options, pBinaryExpression->pExpressionLeft, "l-expr", b, fp);

            TNodeClueList_CodePrint(options, &pBinaryExpression->ClueList00, fp);
            Output_Append(fp, TokenToString(pBinaryExpression->token));

            b = TExpression_CodePrint(program, options, ((TBinaryExpression*)p)->pExpressionRight, "r-expr", b, fp);
        }
        break;

        CASE(TTernaryExpression) :
        {
            TTernaryExpression* pTernaryExpression =
                (TTernaryExpression*)p;


            b = TExpression_CodePrint(program, options, pTernaryExpression->pExpressionLeft, "l-expr", b, fp);

            TNodeClueList_CodePrint(options, &pTernaryExpression->ClueList0, fp);
            Output_Append(fp, "?");

            b = TExpression_CodePrint(program, options, pTernaryExpression->pExpressionMiddle, "m-expr", b, fp);

            TNodeClueList_CodePrint(options, &pTernaryExpression->ClueList1, fp);
            Output_Append(fp, ":");

            b = TExpression_CodePrint(program, options, pTernaryExpression->pExpressionRight, "r-expr", b, fp);
        }
        break;

        CASE(TPrimaryExpressionLiteral) :
        {
            TPrimaryExpressionLiteral* pPrimaryExpressionLiteral
                = (TPrimaryExpressionLiteral*)p;

            ForEachListItem(TPrimaryExpressionLiteralItem, pItem, &pPrimaryExpressionLiteral->List)
            {
                TNodeClueList_CodePrint(options, &pItem->ClueList0, fp);
                Output_Append(fp, pItem->lexeme);
            }
        }
        break;

        CASE(TPrimaryExpressionValue) :
        {
            TPrimaryExpressionValue* pPrimaryExpressionValue =
                (TPrimaryExpressionValue*)p;

            if (pPrimaryExpressionValue->pExpressionOpt != NULL)
            {
                TNodeClueList_CodePrint(options, &pPrimaryExpressionValue->ClueList0, fp);
                Output_Append(fp, "(");
                b = TExpression_CodePrint(program, options, pPrimaryExpressionValue->pExpressionOpt, "expr", b, fp);

                TNodeClueList_CodePrint(options, &pPrimaryExpressionValue->ClueList1, fp);
                Output_Append(fp, ")");
            }
            else
            {
                TNodeClueList_CodePrint(options, &pPrimaryExpressionValue->ClueList0, fp);
                Output_Append(fp, pPrimaryExpressionValue->lexeme);
            }
        }
        b = true;

        break;

        CASE(TPostfixExpressionCore) :
        {
            TPostfixExpressionCore* pPostfixExpressionCore =
                (TPostfixExpressionCore*)p;
            b = TPostfixExpressionCore_CodePrint(program, options, pPostfixExpressionCore, b, fp);
        }
        break;

        CASE(TUnaryExpressionOperator) :
        {
            TUnaryExpressionOperator* pTUnaryExpressionOperator =
                (TUnaryExpressionOperator*)p;

            TNodeClueList_CodePrint(options, &pTUnaryExpressionOperator->ClueList00, fp);

            if (pTUnaryExpressionOperator->token == TK_SIZEOF)
            {
                if (pTUnaryExpressionOperator->TypeName.SpecifierQualifierList.pHead != NULL)
                {
                    Output_Append(fp, "sizeof");
                    TNodeClueList_CodePrint(options, &pTUnaryExpressionOperator->ClueList1, fp);

                    Output_Append(fp, "(");
                    TTypeName_CodePrint(program, options, &pTUnaryExpressionOperator->TypeName, b, fp);

                    TNodeClueList_CodePrint(options, &pTUnaryExpressionOperator->ClueList2, fp);
                    Output_Append(fp, ")");
                }
                else
                {
                    Output_Append(fp, "sizeof");
                    b = TExpression_CodePrint(program, options, pTUnaryExpressionOperator->pExpressionRight, "expr", b, fp);
                    Output_Append(fp, "");
                }
            }
            else
            {
                Output_Append(fp, TokenToString(((TBinaryExpression*)p)->token));
                b = TExpression_CodePrint(program, options, pTUnaryExpressionOperator->pExpressionRight, "expr", b, fp);
            }
        }
        break;

        CASE(TCastExpressionType) :
        {
            TCastExpressionType * pCastExpressionType =
                (TCastExpressionType*)p;
            TNodeClueList_CodePrint(options, &pCastExpressionType->ClueList0, fp);
            Output_Append(fp, "(");

            TTypeName_CodePrint(program, options, &pCastExpressionType->TypeName, b, fp);

            //b = TTypeQualifierList_CodePrint(program, options, &pCastExpressionType->TypeName.Specifiers.TypeQualifiers, false, fp);
            //b = TTypeSpecifier_CodePrint(program, options, pCastExpressionType->TypeName.Specifiers.pTypeSpecifierOpt, b, fp);
            //b = TDeclarator_CodePrint(program, options, &pCastExpressionType->TypeName.Declarator, b, fp);

            TNodeClueList_CodePrint(options, &pCastExpressionType->ClueList1, fp);
            Output_Append(fp, ")");

            b = TExpression_CodePrint(program, options, pCastExpressionType->pExpression, "expr", b, fp);
        }
        break;

    default:

        ASSERT(false);
    }


    return b;
}



static   bool TEnumerator_CodePrint(TProgram* program, Options * options, TEnumerator* pTEnumerator, bool b, StrBuilder* fp)
{

    TNodeClueList_CodePrint(options, &pTEnumerator->ClueList0, fp);
    Output_Append(fp, pTEnumerator->Name);

    if (pTEnumerator->pExpression)
    {
        TNodeClueList_CodePrint(options, &pTEnumerator->ClueList1, fp);
        Output_Append(fp, "=");

        TExpression_CodePrint(program, options, pTEnumerator->pExpression, "expr", true, fp);


    }
    else
    {
        //vou criar uma expressionp enum?
    }

    if (pTEnumerator->bHasComma)
    {
        TNodeClueList_CodePrint(options, &pTEnumerator->ClueList2, fp);
        Output_Append(fp, ",");
    }

    return true;
}

static bool TEnumSpecifier_CodePrint(TProgram* program, Options * options, TEnumSpecifier* p, bool b, StrBuilder* fp)
{
    b = true;

    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "enum");

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, p->Name);

    TNodeClueList_CodePrint(options, &p->ClueList2, fp);

    if (p->EnumeratorList.pHead != NULL)
    {
        Output_Append(fp, "{");

        ForEachListItem(TEnumerator, pTEnumerator, &p->EnumeratorList)
        {
            TEnumerator_CodePrint(program, options, pTEnumerator, false, fp);
        }

        TNodeClueList_CodePrint(options, &p->ClueList3, fp);
        Output_Append(fp, "}");
    }

    return true;
}


static bool TStructUnionSpecifier_CodePrint(TProgram* program, Options * options, TStructUnionSpecifier* p, bool b, StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    b = true;

    if (p->bIsStruct)
        Output_Append(fp, "struct");

    else
        Output_Append(fp, "union");

    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, p->Name);

    if (p->TemplateName != NULL)
    {

        //TNodeClueList_CodePrint(options, &p->ClueList2, fp);

        AllPlugin_InstanciateType(program,
            options,
            p,
            b,
            fp);

    }
    else
    {
        if (p->StructDeclarationList.size > 0)
        {
            TNodeClueList_CodePrint(options, &p->ClueList2, fp);

            Output_Append(fp, "{");

            for (size_t i = 0; i < p->StructDeclarationList.size; i++)
            {
                TAnyStructDeclaration * pStructDeclaration = p->StructDeclarationList.pItems[i];
                b = TAnyStructDeclaration_CodePrint(program, options, pStructDeclaration, b, fp);
            }

            TNodeClueList_CodePrint(options, &p->ClueList3, fp);
            Output_Append(fp, "}");
        }
    }

    return true;
}

static bool TSingleTypeSpecifier_CodePrint(TProgram* program, Options * options, TSingleTypeSpecifier* p, bool b, StrBuilder* fp)
{

    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    b = true;
    if (p->Token != TK_IDENTIFIER)
    {
        Output_Append(fp,
            TokenToString(p->Token));
    }
    else
    {
        Output_Append(fp, p->TypedefName);
    }

    return b;
}

static bool TTypeSpecifier_CodePrint(TProgram* program, Options * options, TTypeSpecifier*  p, bool b, StrBuilder* fp)
{
    if (p == NULL)
    {
        //declaracao vazia: ;
        return true;
    }

    switch (p->Type)
    {
    case TStructUnionSpecifier_ID:
        //TAnyStructDeclaration_CodePrint();
        b = TStructUnionSpecifier_CodePrint(program, options, (TStructUnionSpecifier*)p, b, fp);
        break;

    case TEnumSpecifier_ID:
        b = TEnumSpecifier_CodePrint(program, options, (TEnumSpecifier*)p, b, fp);
        break;

    case TSingleTypeSpecifier_ID:
        b = TSingleTypeSpecifier_CodePrint(program, options, (TSingleTypeSpecifier*)p, b, fp);
        break;

    default:
        break;
    }

    return b;
}

static bool TDesignator_CodePrint(TProgram* program, Options * options, TDesignator* p, bool b, StrBuilder* fp)
{
    if (b)
        Output_Append(fp, ",");


    //
    if (p->Name)
    {
        //.identifier
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, ".");
        Output_Append(fp, p->Name);
        Output_Append(fp, "=");
        TExpression_CodePrint(program, options, p->pExpression, "index", b, fp);
    }
    else
    {
        //[constant-expression]
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        TExpression_CodePrint(program, options, p->pExpression, "index", b, fp);
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    }


    return b;
}


static bool TInitializerList_CodePrint(TProgram* program,
    Options * options,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializerList*p,
    bool b,
    StrBuilder* fp)
{
    b = false;

    if (List_HasOneItem(p) &&
        List_Back(p)->pInitializer == NULL/* &&
                                          pSpecifierQualifierList != NULL*/)
    {
        //a partir de {} e um tipo consegue gerar o final

         InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(pDeclarationSpecifiers),
            NULL,                        //<-dupla para entender o tipo
            "",
            ActionStaticInit,
            false,
            fp);


        //BuildInitialization(program, options, pDeclarationSpecifiers, bIsPointer, fp);

    }
    else
    {
        //TNodeClueList_CodePrint(&p->ClueList, fp, 0);

        //Output_Append(fp, "{");

        ForEachListItem(TInitializerListItem, pItem, p)
        {
            if (!List_IsFirstItem(p, pItem))
                Output_Append(fp, ",");

            b = TInitializerListItem_CodePrint(program,
                options,
                NULL,
                pDeclarationSpecifiers,
                bIsPointer, pItem, b, fp);
        }

        //Output_Append(fp, "}");
    }


    return true;
}
static bool TInitializerListType_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializerListType*p,
    bool b,
    StrBuilder* fp)
{

    if (p->bDefault)
    {
        TNodeClueList_CodePrint(options, &p->ClueList00, fp);

        Output_Append(fp, "_default");
        Output_Append(fp, " {");
        InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(pDeclarationSpecifiers),
            pDeclarator,                        //<-dupla para entender o tipo
            "",
            ActionStaticInit,
            false,
            fp);

        Output_Append(fp, "}");
       
        //BuildInitialization(program,
        //  options,
        //pDeclarationSpecifiers,
        //bIsPointer,
        //fp);
    }
    else
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "{");

        b = TInitializerList_CodePrint(program,
            options,
            pDeclarationSpecifiers,
            bIsPointer,
            &p->InitializerList,
            b,
            fp);

        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, "}");
    }



    return true;
}



static bool TInitializer_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializer* pTInitializer,
    bool b,
    StrBuilder* fp)
{
    if (pTInitializer == NULL)
    {
        return false;
    }
    if (pTInitializer->Type == TInitializerListType_ID)
    {
        b = TInitializerListType_CodePrint(program,
            options,
            pDeclarator,
            pDeclarationSpecifiers,
            bIsPointer,
            (TInitializerListType*)pTInitializer, b, fp);
    }
    else
    {
        b = TExpression_CodePrint(program, options, (TExpression*)pTInitializer, "", false, fp);
    }

    return b;
}



static bool TPointerList_CodePrint(TProgram* program, Options * options, TPointerList *p, bool b, StrBuilder* fp)
{
    b = false;

    ForEachListItem(TPointer, pItem, p)
    {
        b = TPointer_CodePrint(program, options, pItem, b, fp);
    }

    return true;
}



static bool TParameterList_CodePrint(TProgram* program, Options * options, TParameterList *p, bool b, StrBuilder* fp)
{
    b = false;


    ForEachListItem(TParameter, pItem, p)
    {
        //if (!List_IsFirstItem(p, pItem))
        //{
        //  TNodeClueList_CodePrint(options, &pItem->ClueList, fp);
        //            Output_Append(fp, ",");
        //}
        //TParameterDeclaration * pItem = p->pItems[i];
        b = TParameter_CodePrint(program, options, pItem, b, fp);
    }


    return true;
}

static bool TParameterTypeList_CodePrint(TProgram* program, Options * options, TParameterTypeList *p, bool b, StrBuilder* fp)
{
    //Output_Append(fp, "(");
    TParameterList_CodePrint(program, options, &p->ParameterList, b, fp);

    if (p->bVariadicArgs)
    {
        //TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        //Output_Append(fp, ",");

        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, "...");
    }
    //Output_Append(fp, ")");
    return b;
}

static bool TDirectDeclarator_CodePrint(TProgram* program, Options * options, TDirectDeclarator* pDirectDeclarator,
    bool b,
    bool bPrintName,
    StrBuilder* fp)
{
    if (pDirectDeclarator == NULL)
    {
        return false;
    }
    //fprintf(fp, "{");
    b = false;

    if (pDirectDeclarator->Identifier)
    {
        //identifier
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList0, fp);
        if (bPrintName)
        {
            Output_Append(fp, pDirectDeclarator->Identifier);
        }
        b = true;
    }
    else  if (pDirectDeclarator->pDeclarator)
    {
        //( declarator )
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList0, fp);
        Output_Append(fp, "(");

        b = TDeclarator_CodePrint(program, options, pDirectDeclarator->pDeclarator, b, bPrintName, fp);

        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList1, fp);
        Output_Append(fp, ")");
    }

    if (pDirectDeclarator->Type == TDirectDeclaratorTypeArray)
    {
        /*
        direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
        direct-declarator [ static type-qualifier-listopt assignment-expression ]
        direct-declarator [ type-qualifier-list static assignment-expression ]
        */
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList2, fp);
        Output_Append(fp, "[");
        if (pDirectDeclarator->pExpression)
        {
            b = TExpression_CodePrint(program, options, pDirectDeclarator->pExpression, "assignment-expression", b, fp);
        }
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList3, fp);
        Output_Append(fp, "]");
    }


    if (pDirectDeclarator->Type == TDirectDeclaratorTypeFunction)
    {
        //( parameter-type-list )
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList2, fp);
        Output_Append(fp, "(");
        TParameterTypeList_CodePrint(program, options, &pDirectDeclarator->Parameters, b, fp);
        TNodeClueList_CodePrint(options, &pDirectDeclarator->ClueList3, fp);
        Output_Append(fp, ")");
    }

    if (pDirectDeclarator->pDirectDeclarator)
    {
        //fprintf(fp, "\"direct-declarator\":");
        TDirectDeclarator_CodePrint(program, options, pDirectDeclarator->pDirectDeclarator, b, bPrintName, fp);
    }


    //fprintf(fp, "}");
    return b;
}

static bool TDeclarator_CodePrint(TProgram* program, Options * options, TDeclarator* p, bool b, bool bPrintName, StrBuilder* fp)
{
    b = TPointerList_CodePrint(program, options, &p->PointerList, b, fp);
    b = TDirectDeclarator_CodePrint(program, options, p->pDirectDeclarator, b, bPrintName, fp);
    return b;
}

bool TInitDeclarator_CodePrint(TProgram* program, Options * options, TInitDeclarator* p, bool b, StrBuilder* fp);



bool TStructDeclarator_CodePrint(TProgram* program,
    Options * options,
    TSpecifierQualifierList* pSpecifierQualifierList,
    TStructDeclarator* p,
    bool b,
    StrBuilder* fp)
{
    b = false;
    b = TDeclarator_CodePrint(program, options, p->pDeclarator, b, true/*bPrintName*/, fp);
    if (p->pInitializer)
    {
        TNodeClueList_CodePrint(options, &p->ClueList1, fp);
        Output_Append(fp, "_defval");

        Output_Append(fp, "(");

        Options opt = *options;
        opt.bExpandMacros = true;
        opt.bIncludeComments = false;

        TInitializer_CodePrint(program,
            &opt,
            p->pDeclarator,
            (TDeclarationSpecifiers*)pSpecifierQualifierList,
            TPointerList_IsPointer(&p->pDeclarator->PointerList),
            p->pInitializer,
            b,
            fp);
        Output_Append(fp, ")");
    }
    return true;
}

static bool TStructDeclaratorList_CodePrint(TProgram* program,
    Options * options,
    TSpecifierQualifierList* pSpecifierQualifierList,
    TStructDeclaratorList *p,
    bool b,
    StrBuilder* fp)
{
    b = false;



    ForEachListItem(TInitDeclarator, pItem, p)
    {
        if (!List_IsFirstItem(p, pItem))
        {
            TNodeClueList_CodePrint(options, &pItem->ClueList00, fp);
            Output_Append(fp, ",");
        }
        b = TStructDeclarator_CodePrint(program, options, pSpecifierQualifierList, pItem, b, fp);
    }


    return true;
}

static bool TStructDeclaration_CodePrint(TProgram* program,
    Options * options,
    TStructDeclaration* p,
    bool b,
    StrBuilder* fp)
{
    TSpecifierQualifierList_CodePrint(program, options, &p->SpecifierQualifierList, false, fp);
    //b = TTypeQualifierList_CodePrint(program, options, &p->Qualifier, false, fp);
    //b = TTypeSpecifier_CodePrint(program, options, p->pSpecifier, b, fp);
    b = TStructDeclaratorList_CodePrint(program,
        options,
        &p->SpecifierQualifierList,
        &p->DeclaratorList, b, fp);


    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, ";");

    return true;
}

static bool TAnyStructDeclaration_CodePrint(TProgram* program, Options * options, TAnyStructDeclaration* p, bool b, StrBuilder* fp)
{
    switch (p->Type)
    {
    case TStructDeclaration_ID:
        b = TStructDeclaration_CodePrint(program, options, (TStructDeclaration*)p, b, fp);
        break;

    default:
        ASSERT(false);
        break;
    }

    return b;
}

static bool StorageSpecifier_CodePrint(TProgram* program, Options * options, TStorageSpecifier* p, bool b, StrBuilder* fp)
{

    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    if (p->bIsAuto)
    {
        Output_Append(fp, "auto");
        b = true;
    }

    if (p->bIsExtern)
    {
        Output_Append(fp, "extern");
        b = true;
    }

    if (p->bIsRegister)
    {
        Output_Append(fp, "register");
        b = true;
    }

    if (p->bIsStatic)
    {
        Output_Append(fp, "static");
        b = true;
    }




    if (p->bIsThread_local)
    {
        Output_Append(fp, "[Thread_local]");
        b = true;
    }

    if (p->bIsTypedef)
    {
        Output_Append(fp, "typedef");
        b = true;
    }

    return b;
}



static bool TTemplateTypeSpecifier_CodePrint(TProgram* program, Options * options, TTemplateTypeSpecifier* p, bool b, StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    if (strcmp(p->Identifier, "List") == 0)
    {

        StrBuilder strBuilder = STRBUILDER_INIT;

        if (p->Args.pHead != NULL)
        {
            TTypeName_CodePrint(program, options, &p->Args.pHead->TypeName, false, &strBuilder);
        }
        else
        {
            //TODO
            //tem que ter 1 arg
        }


        Output_Append(fp, "struct {");
        Output_Append(fp, strBuilder.c_str);
        Output_Append(fp, "* pHead; ");
        Output_Append(fp, strBuilder.c_str);
        Output_Append(fp, "* pTail; }");

        StrBuilder_Destroy(&strBuilder);
    }
    else if (strcmp(p->Identifier, "Union") == 0)
    {
        Output_Append(fp, "struct { int type; }");
    }
    else
    {
        //ERROR
        printf("ERROR _Template\n");
        Output_Append(fp, "struct { /*error*/}");

        //TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        //Output_Append(fp, "_Template");


        //Output_Append(fp, "(");
        //Output_Append(fp, p->Identifier);
        //Output_Append(fp, ",");
        //TTypeName_CodePrint(program, options, &p->TypeName, b, fp);

        //Output_Append(fp, ")");
    }
    return b;
}

static bool TFunctionSpecifier_CodePrint(TProgram* program, Options * options, TFunctionSpecifier* p, bool b, StrBuilder* fp)
{
    if (p->bIsInline)
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "inline");
        b = true;
    }
    if (p->bIsNoReturn)
    {
        TNodeClueList_CodePrint(options, &p->ClueList0, fp);
        Output_Append(fp, "[noreturn]");
        b = true;
    }
    return b;
}


static bool TTypeQualifier_CodePrint(TProgram* program, Options * options, TTypeQualifier* p, bool b, StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);

    Output_Append(fp, TokenToString(p->Token));
    b = true;
    return b;
}

static bool TTypeQualifierList_CodePrint(TProgram* program, Options * options, TTypeQualifierList* p, bool b, StrBuilder* fp)
{
    ForEachListItem(TTypeQualifier, pItem, p)
    {
        b = TTypeQualifier_CodePrint(program, options, pItem, b, fp);
    }
    return b;
}
static bool TPointer_CodePrint(TProgram* program, Options * options, TPointer* pPointer, bool b, StrBuilder* fp)
{
    if (pPointer->bPointer)
    {
        TNodeClueList_CodePrint(options, &pPointer->ClueList0, fp);
        Output_Append(fp, "*");
    }
    else
    {
        TTypeQualifierList_CodePrint(program, options, &pPointer->Qualifier, false, fp);
    }

    return true;
}

bool TSpecifierQualifierList_CodePrint(TProgram* program,
    Options * options,
    TSpecifierQualifierList* pDeclarationSpecifiers,
    bool b,
    StrBuilder* fp)
{
    ForEachListItem(TSpecifierQualifier, pItem, pDeclarationSpecifiers)
    {
        switch (pItem->Type)
        {

            CASE(TSingleTypeSpecifier) :
                TSingleTypeSpecifier_CodePrint(program, options, (TSingleTypeSpecifier*)pItem, b, fp);
            break;

            CASE(TStorageSpecifier) :
                StorageSpecifier_CodePrint(program, options, (TStorageSpecifier*)pItem, b, fp);
            break;
            CASE(TTypeQualifier) :
                TTypeQualifier_CodePrint(program, options, (TTypeQualifier*)pItem, b, fp);
            break;
            CASE(TFunctionSpecifier) :
                TFunctionSpecifier_CodePrint(program, options, (TFunctionSpecifier*)pItem, b, fp);
            break;
            //CASE(TAlignmentSpecifier) : 
            ///TAlignmentSpecifier_CodePrint(program, options, (TAlignmentSpecifier*)pItem, b, fp);
            //break;

            CASE(TStructUnionSpecifier) :
                TStructUnionSpecifier_CodePrint(program, options, (TStructUnionSpecifier*)pItem, b, fp);
            break;

            CASE(TEnumSpecifier) :
                TEnumSpecifier_CodePrint(program, options, (TEnumSpecifier*)pItem, b, fp);
            break;

        default:
            ASSERT(false);
            break;
        }
    }

    return b;
}

bool TDeclarationSpecifiers_CodePrint(TProgram* program, Options * options, TDeclarationSpecifiers* pDeclarationSpecifiers, bool b, StrBuilder* fp)
{

    ForEachListItem(TSpecifier, pItem, pDeclarationSpecifiers)
    {
        switch (pItem->Type)
        {

            CASE(TSingleTypeSpecifier) :
                TSingleTypeSpecifier_CodePrint(program, options, (TSingleTypeSpecifier*)pItem, b, fp);
            break;

            CASE(TStructUnionSpecifier) :
                TStructUnionSpecifier_CodePrint(program, options, (TStructUnionSpecifier*)pItem, b, fp);
            break;

            CASE(TEnumSpecifier) :
                TEnumSpecifier_CodePrint(program, options, (TEnumSpecifier*)pItem, b, fp);
            break;

            CASE(TStorageSpecifier) :
                StorageSpecifier_CodePrint(program, options, (TStorageSpecifier*)pItem, b, fp);
            break;
            CASE(TTypeQualifier) :
                TTypeQualifier_CodePrint(program, options, (TTypeQualifier*)pItem, b, fp);
            break;
            CASE(TFunctionSpecifier) :
                TFunctionSpecifier_CodePrint(program, options, (TFunctionSpecifier*)pItem, b, fp);
            break;

            CASE(TTemplateTypeSpecifier) :
                TTemplateTypeSpecifier_CodePrint(program, options, (TTemplateTypeSpecifier*)pItem, b, fp);
            break;
            //CASE(TAlignmentSpecifier) : 
            ///TAlignmentSpecifier_CodePrint(program, options, (TAlignmentSpecifier*)pItem, b, fp);
            //break;

        default:
            ASSERT(false);
            break;
        }
    }

    return b;

    //pDeclarationSpecifiers

    //    b = TFunctionSpecifier_CodePrint(program, options, &pDeclarationSpecifiers->FunctionSpecifiers, b, fp);
    //  b = StorageSpecifier_CodePrint(program, options, &pDeclarationSpecifiers->StorageSpecifiers, b, fp);
    //b = TTypeQualifierList_CodePrint(program, options, &pDeclarationSpecifiers->TypeQualifiers, b, fp);
    //b = TTypeSpecifier_CodePrint(program, options, pDeclarationSpecifiers->pTypeSpecifierOpt, b, fp);
    return b;
}

bool TInitDeclarator_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitDeclarator* p,
    bool b, StrBuilder* fp)
{
    b = false;
    b = TDeclarator_CodePrint(program, options, p->pDeclarator, b, true/*bPrintName*/, fp);

    if (p->pInitializer)
    {
        TNodeClueList_CodePrint(options, &p->ClueList00, fp);
        Output_Append(fp, "=");
        b = TInitializer_CodePrint(program,
            options,
            pDeclarator,
            pDeclarationSpecifiers,
            bIsPointer,
            p->pInitializer,
            b,
            fp);
    }
    return true;
}

//bool TInitDeclarator_CodePrint(TInitDeclarator* p, bool b, StrBuilder* fp);

bool TInitDeclaratorList_CodePrint(TProgram* program,
    Options * options,

    TDeclarationSpecifiers *pDeclarationSpecifiers,
    TInitDeclaratorList *p,
    bool b,
    StrBuilder* fp)
{
    b = false;
    //fprintf(fp, "[");
    ForEachListItem(TInitDeclarator, pInitDeclarator, p)
    {
        if (!List_IsFirstItem(p, pInitDeclarator))
            Output_Append(fp, ",");

        bool bIsPointer =
            TPointerList_IsPointer(&pInitDeclarator->pDeclarator->PointerList);

        b = TInitDeclarator_CodePrint(program, options, pInitDeclarator->pDeclarator, pDeclarationSpecifiers, bIsPointer, pInitDeclarator, b, fp);
    }

    //  fprintf(fp, "]");
    return true;
}



void InstanciateDestroy2(TProgram* program,
    Options* options,
    TSpecifierQualifierList* pSpecifierQualifierList,//<-dupla para entender o tipo
    TDeclarator* pDeclatator,                        //<-dupla para entender o tipo
    const char* pInitExpressionText, //(x->p->i = 0),
    const Action action,
    bool bCanApplyFunction,
    StrBuilder* fp);

static bool DefaultFunctionDefinition_CodePrint(TProgram* program,
    Options * options,
    TDeclaration* p,
    bool b,
    StrBuilder* fp)
{

    //Retorno da funcao
    TDeclarationSpecifiers* pSpecifiers =
        &p->Specifiers;


    const char* funcName =
        TDeclarator_GetName(p->InitDeclaratorList.pHead->pDeclarator);

    //parametros
    TParameter* pFirstParameter = NULL;
    const char* firstParameterName = NULL;
    TParameterTypeList * pArgs =
        TDeclaration_GetFunctionArguments(p);
    if (pArgs)
    {
        int i = 0;
        ForEachListItem(TParameter, pParameter, &pArgs->ParameterList)
        {
            if (i == 0)
            {
                pFirstParameter = pParameter;
                firstParameterName = TParameter_GetName(pParameter);
            }
            i++;
        }
    }

    if (IsSuffix(funcName, "_Create"))
    {
        options->IdentationLevel++;
        InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(&pSpecifiers),
            p->InitDeclaratorList.pHead->pDeclarator,
            "p",
            ActionCreate,
            false,
            fp);
        options->IdentationLevel--;
    }
    else if (IsSuffix(funcName, "_Init") && pFirstParameter)
    {
        options->IdentationLevel++;
        InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(&pFirstParameter->Specifiers),
            &pFirstParameter->Declarator,
            firstParameterName,
            ActionInitContent,
            false,
            fp);
        options->IdentationLevel--;
    }
    else if (IsSuffix(funcName, "_Destroy"))
    {
        options->IdentationLevel++;
        InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(&pFirstParameter->Specifiers),
            &pFirstParameter->Declarator,
            firstParameterName,
            ActionDestroyContent,
            false,
            fp);
        options->IdentationLevel--;
    }
    else if (IsSuffix(funcName, "_Delete"))
    {
        options->IdentationLevel++;
        InstanciateDestroy2(program,
            options,
            (TSpecifierQualifierList*)(&pFirstParameter->Specifiers),
            &pFirstParameter->Declarator,
            firstParameterName,
            ActionDelete,
            false,
            fp);
        options->IdentationLevel--;
    }
    else
    {

        AllPlugin_CodePrint(program,
            options,
            p,
            b,
            fp);

    }
    //////////////////////////


    /////////////////////////

    //TParameterTypeList * pArgs =
    //      TDeclaration_GetFunctionArguments(p);
    //  if (pArgs)
    //{
    //  ForEachListItem(TParameter, pParameter, &pArgs->ParameterList)
    //{
    //  printf("%s\n", TParameter_GetName(pParameter));
    //printf("%s\n", TParameter_GetTypedefName(pParameter));
    //printf(TParameter_IsDirectPointer(pParameter) ? "true" : "false");


    // Std_Template_CodePrint(program,
    //                   options,
    //                   p,
    //                      b,
    //                    fp);


    return b;
}

static bool TDeclaration_CodePrint(TProgram* program,
    Options * options,
    TDeclaration* p,
    bool b,
    StrBuilder* fp)
{
    b = TDeclarationSpecifiers_CodePrint(program, options, &p->Specifiers, false, fp);

    b = TInitDeclaratorList_CodePrint(program,
        options,
        &p->Specifiers,
        &p->InitDeclaratorList, b, fp);

    if (p->pCompoundStatementOpt != NULL)
    {
        /*
        typedef struct Array { int data; ... } Items;
        void Items_Add(Items* p,int i) {...}
        void Items_Delete(Items* p,int i) {...}
        */

        if (p->bDefault)
        {
            TNodeClueList_CodePrint(options, &p->ClueList00, fp);
            StrBuilder_Append(fp, "_default");

            if (options->bShrink)
            {
                Output_Append(fp, ";");
            }
            else
            {
                TNodeClueList_CodePrint(options, &p->pCompoundStatementOpt->ClueList0, fp);
                Output_Append(fp, "{\n");

                DefaultFunctionDefinition_CodePrint(program,
                    options,
                    p,
                    b,
                    fp);

                Output_Append(fp, "}");

            }
            return true;
        }
        else
        {
            if (p->pCompoundStatementOpt != NULL)
            {
                //normal
                TCompoundStatement_CodePrint(program,
                    options,
                    p->pCompoundStatementOpt,
                    b,
                    fp);
            }
        }
    }
    else
    {
        if (p->bDefault)
        {

            if (!options->bShrink)
            {
                TNodeClueList_CodePrint(options, &p->ClueList00, fp);
                StrBuilder_Append(fp, "_default");
                //Output_Append(fp, " /*default*/\n");
                TNodeClueList_CodePrint(options, &p->ClueList1, fp);
                Output_Append(fp, "\n{\n");

                DefaultFunctionDefinition_CodePrint(program,
                    options,
                    p,
                    b,
                    fp);

                Output_Append(fp, "\n");
                Output_Append(fp, "}");
            }
            else
            {
                TNodeClueList_CodePrint(options, &p->ClueList1, fp);
                StrBuilder_Append(fp, " _default");
                Output_Append(fp, ";");
            }

            return true;
        }
        else
        {
            TNodeClueList_CodePrint(options, &p->ClueList1, fp);
            Output_Append(fp, ";");
        }
    }


    return true;
}

bool TTypeName_CodePrint(TProgram* program, Options * options, TTypeName* p, bool b, StrBuilder* fp)
{

    TSpecifierQualifierList_CodePrint(program, options, &p->SpecifierQualifierList, false, fp);

    //p->Declarator
    //b = TDeclarationSpecifiers_CodePrint(program, options, &p->Specifiers, false, fp);


    b = TDeclarator_CodePrint(program, options, &p->Declarator, b, true/*bPrintName*/, fp);

    return b;
}

static bool TParameter_CodePrint(TProgram* program,
    Options * options,
    TParameter* p,
    bool b,
    StrBuilder* fp)
{
    b = TDeclarationSpecifiers_CodePrint(program, options, &p->Specifiers, false, fp);
    b = TDeclarator_CodePrint(program, options, &p->Declarator, b, true/*bPrintName*/, fp);

    if (p->bHasComma)
    {
        TNodeClueList_CodePrint(options, &p->ClueList00, fp);
        Output_Append(fp, ",");
    }

    return b;
}

static bool TEofDeclaration_CodePrint(TProgram* program,
    Options * options,
    TEofDeclaration *p,
    bool b,
    StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    return true;
}

static bool TStaticAssertDeclaration_CodePrint(TProgram* program,
    Options * options,
    TStaticAssertDeclaration *p,
    bool b,
    StrBuilder* fp)
{
    TNodeClueList_CodePrint(options, &p->ClueList0, fp);
    Output_Append(fp, "_StaticAssert");
    TNodeClueList_CodePrint(options, &p->ClueList1, fp);
    Output_Append(fp, "(");
    TExpression_CodePrint(program, options, p->pConstantExpression, "", b, fp);
    Output_Append(fp, ",");
    TNodeClueList_CodePrint(options, &p->ClueList2, fp);

    TNodeClueList_CodePrint(options, &p->ClueList3, fp);
    Output_Append(fp, p->Text);


    TNodeClueList_CodePrint(options, &p->ClueList4, fp);
    Output_Append(fp, ")");
    TNodeClueList_CodePrint(options, &p->ClueList5, fp);
    Output_Append(fp, ";");
    return true;
}

static bool TAnyDeclaration_CodePrint(TProgram* program, Options * options, TAnyDeclaration *pDeclaration, bool b, StrBuilder* fp)
{
    switch (pDeclaration->Type)
    {
    case TEofDeclaration_ID:
        b = TEofDeclaration_CodePrint(program, options, (TEofDeclaration*)pDeclaration, b, fp);
        break;

    case TStaticAssertDeclaration_ID:
        b = TStaticAssertDeclaration_CodePrint(program, options, (TStaticAssertDeclaration*)pDeclaration, b, fp);
        break;

    case TDeclaration_ID:
        b = TDeclaration_CodePrint(program, options, (TDeclaration*)pDeclaration, b, fp);
        break;

    default:
        ASSERT(false);
        break;
    }

    return b;
}

static bool TDesignatorList_CodePrint(TProgram* program, Options * options, TDesignatorList *p, bool b, StrBuilder* fp)
{
    b = false;


    ForEachListItem(TDesignator, pItem, p)
    {
        if (!List_IsFirstItem(p, pItem))
        {
            Output_Append(fp, ",");
        }
        b = TDesignator_CodePrint(program, options, pItem, b, fp);
    }


    return true;
}


static bool TInitializerListItem_CodePrint(TProgram* program,
    Options * options,
    TDeclarator* pDeclarator,
    TDeclarationSpecifiers* pDeclarationSpecifiers,
    bool bIsPointer,
    TInitializerListItem* p,
    bool b,
    StrBuilder* fp)
{

    b = false;

    if (!List_IsEmpty(&p->DesignatorList))
    {
        b = TDesignatorList_CodePrint(program, options, &p->DesignatorList, b, fp);
    }

    b = TInitializer_CodePrint(program,
        options,
        pDeclarator,
        pDeclarationSpecifiers,
        bIsPointer,
        p->pInitializer,
        b,
        fp);

    return true;
}


static bool TDeclarations_CodePrint(TProgram* program, Options * options, TDeclarations *p, bool b, StrBuilder* fp)
{
    b = false;


    for (size_t i = 0; i < p->size; i++)
    {
        if (i > 0)
            Output_Append(fp, ",");

        TAnyDeclaration* pItem = p->pItems[i];
        b = TAnyDeclaration_CodePrint(program, options, pItem, b, fp);

    }

    return true;
}

/*
static void TProgram_PrintToFile(TProgram* pProgram,
const char* fileName)
{
StrBuilder * fp = fopen(fileName, "w");
TDeclarations_CodePrint(&pProgram->Declarations, false, fp);
fclose(fp);
}*/


static void TProgram_PrintFiles(TProgram* pProgram,
    StrBuilder* fp,
    const char* userpath)
{
    //TODO tem que ter um teste especial..

    //o arqquivo externo que foi incluido por um aquivo local
    //deve ser incluido..se ele foi incluido por outro nao.
    //tem que marcar se foi incluido por um da lista de includes e nao incluir

    Output_Append(fp, "/*external files*/\n");
    for (int i = 0; i < pProgram->Files2.size; i++)
    {
        TFile *pFile = pProgram->Files2.pItems[i];
        if (pFile->bDirectInclude)
        {
            //char drive[_MAX_DRIVE];
            //char dir[_MAX_DIR];
            //char fname[_MAX_FNAME];
            //char ext[_MAX_EXT];
            //SplitPath(pFile->IncludePath, drive, dir, fname, ext); // C4996

            if (pFile->bSystemLikeInclude)
            {
                Output_Append(fp, "#include <");
                Output_Append(fp, pFile->IncludePath);
                Output_Append(fp, ">\n");
            }
            else
            {
                Output_Append(fp, "#include \"");
                Output_Append(fp, pFile->IncludePath);
                Output_Append(fp, "\"\n");
            }

        }
    }
    Output_Append(fp, "\n");
}


void TProgram_PrintCodeToFile(TProgram* pProgram,
    Options* options,
    const char* outFileName,
    const char* inputFileName)
{
    FILE * fp = fopen(outFileName, "w");

    if (fp == NULL)
    {
        printf("cannot open output file %s", outFileName);
        return;
    }

    bool b = false;

    int k = 0;

    //for (int i = 0; i < pProgram->Files2.size; i++)
    //{
    //  TFile *pFile = pProgram->Files2.pItems[i];
    //  printf("\"%s\"\n", pFile->FullPath);
    //}

    StrBuilder sb = STRBUILDER_INIT;

    for (size_t i = 0; i < pProgram->Declarations.size; i++)
    {
        TAnyDeclaration* pItem = pProgram->Declarations.pItems[i];


        b = TAnyDeclaration_CodePrint(pProgram, options, pItem, b, &sb);


        fprintf(fp, "%s", sb.c_str);




        StrBuilder_Clear(&sb);
        k++;
    }
    StrBuilder_Destroy(&sb);
    fclose(fp);
}






void InstanciateDestroy2(TProgram* program,
    Options* options,
    TSpecifierQualifierList* pSpecifierQualifierList,//<-dupla para entender o tipo
    TDeclarator* pDeclatator,                        //<-dupla para entender o tipo
    const char* pInitExpressionText, //(x->p->i = 0)    
    const Action action,
    bool bCanApplyFunction,
    StrBuilder* fp)
{

    bool bDeclaratorIsPointer = pDeclatator ? TDeclarator_IsPointer(pDeclatator) : false;
    bool bDeclaratorIsAutoPointer = pDeclatator ? TDeclarator_IsAutoPointer(pDeclatator) : false;

    TSpecifier* pMainSpecifier =
        TSpecifierQualifierList_GetMainSpecifier(pSpecifierQualifierList);

    if (pMainSpecifier == NULL)
    {
        //error
        return;
    }

    if (pMainSpecifier->Type == TSingleTypeSpecifier_ID)
    {
        TSingleTypeSpecifier* pSingleTypeSpecifier =
            (TSingleTypeSpecifier*)pMainSpecifier;

        if (pSingleTypeSpecifier->Token == TK_IDENTIFIER)
        {
            bool bComplete = false;
            if (action == ActionDestroy)
            {
                if (bDeclaratorIsPointer)
                {
                }
                else
                {
                    if (bCanApplyFunction)
                    {
                        //vamos procurar pela funcao conceito _Destroy
                        TDeclaration* pDeclarationDestroy =
                            SymbolMap_FindObjFunction(&program->GlobalScope,
                                pSingleTypeSpecifier->TypedefName,
                                "Destroy");
                        if (pDeclarationDestroy)
                        {
                            StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                                "%s_Destroy(&%s);",
                                pSingleTypeSpecifier->TypedefName,
                                pInitExpressionText);
                            bComplete = true;
                        }
                    }
                }

            }

            if (!bComplete)
            {
                TDeclaration* pTypedefTargetDeclaration
                    = SymbolMap_FindTypedefDeclarationTarget(&program->GlobalScope,
                        pSingleTypeSpecifier->TypedefName);

                if (pTypedefTargetDeclaration)
                {
                    //achou declaracao

                    //procura o declarator
                    TDeclarator* pTypedefTargetDeclarator =
                        TDeclaration_FindDeclarator(pTypedefTargetDeclaration, pSingleTypeSpecifier->TypedefName);

                    bool bTypedefTargetDeclaratorIsPointer =
                        TDeclarator_IsPointer(pTypedefTargetDeclarator);

                    bool bTypedefTargetDeclaratorIsAutoPointer =
                        TDeclarator_IsAutoPointer(pTypedefTargetDeclarator);

                    //tODO teria que criar um  declarator resultante
                    TDeclarator declarator = TDECLARATOR_INIT;
                    //TPointerList_add
                    //declarator.PointerList

                    ForEachListItem(TPointer, pItem, &pDeclatator->PointerList)
                    {
                        TPointer * pNew = TPointer_Create();
                        pNew->bPointer = pItem->bPointer;
                        pNew->Qualifier = pItem->Qualifier;
                        List_Add(&declarator.PointerList, pNew);
                    }

                    ForEachListItem(TPointer, pItem, &pTypedefTargetDeclarator->PointerList)
                    {
                        TPointer * pNew = TPointer_Create();
                        pNew->bPointer = pItem->bPointer;
                        pNew->Qualifier = pItem->Qualifier;
                        List_Add(&declarator.PointerList, pNew);
                    }

                    Action action2 = action;
                    //passa a informacao do tipo correto agora
                    InstanciateDestroy2(program,
                        options,
                        (TSpecifierQualifierList*)&pTypedefTargetDeclaration->Specifiers,
                        &declarator,
                        pInitExpressionText,
                        action2,
                        bCanApplyFunction,
                        fp);

                    TDeclarator_Destroy(&declarator);
                }
                else
                {
                    //nao achou a declaracao
                }
            }
        }
        else
        {
            //nao eh typedef, deve ser int, double etc..
            if (action == ActionDestroy)
            {
                if (bDeclaratorIsAutoPointer)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "free(%s);", pInitExpressionText);
                }
                else
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "//%s = 0;", pInitExpressionText);
                }
            }
            else if (action == ActionInit)
            {
                if (bDeclaratorIsAutoPointer || bDeclaratorIsPointer)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "%s = NULL;", pInitExpressionText);
                }
                else
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "%s = 0;", pInitExpressionText);
                }
            }
            else if (action == ActionStaticInit)
            {
                if (bDeclaratorIsPointer)
                {
                    StrBuilder_AppendFmt(fp, "/*%s=*/NULL", pInitExpressionText);
                    
                }
                else
                {
                    StrBuilder_AppendFmt(fp, "/*%s=*/0", pInitExpressionText);                    
                }
            }
        }
    }
    else if (pMainSpecifier->Type == TStructUnionSpecifier_ID)
    {
        TStructUnionSpecifier* pStructUnionSpecifier =
            (TStructUnionSpecifier*)pMainSpecifier;

        //Indica se consegui fazer sem entrar na struct
        bool bComplete = false;

        if (action == ActionDestroy || action == ActionDestroyContent)
        {
            if (bDeclaratorIsPointer)
            {
                if (bDeclaratorIsAutoPointer)
                {
                    //nao eh ponteiro
                    if (bCanApplyFunction)
                    {
                        //vamos procurar pela funcao conceito _Delete
                        TDeclaration* pDeclarationDestroy =
                            SymbolMap_FindObjFunction(&program->GlobalScope,
                                pStructUnionSpecifier->Name,
                                "Delete");
                        if (pDeclarationDestroy)
                        {
                            StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                                "%s_Delete(%s);",
                                pStructUnionSpecifier->Name,
                                pInitExpressionText);
                            bComplete = true;
                        }
                        else
                        {
                            //se nao achou delete procura a destroy 
                            //e depois chama free
                            TDeclaration* pDeclarationDestroy =
                                SymbolMap_FindObjFunction(&program->GlobalScope,
                                    pStructUnionSpecifier->Name,
                                    "Destroy");
                            if (pDeclarationDestroy)
                            {
                                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                                    "%s_Destroy(%s);",
                                    pStructUnionSpecifier->Name,
                                    pInitExpressionText);
                                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                                    "free(%s);",
                                    pInitExpressionText);
                                bComplete = true;
                            }
                        }
                    }
                }
                else
                {
                    if (action != ActionDestroyContent)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "//%s = NULL;", pInitExpressionText);
                        bComplete = true;
                    }
                }

            }
            else
            {
                //nao eh ponteiro
                if (bCanApplyFunction)
                {
                    //vamos procurar pela funcao conceito _Destroy
                    TDeclaration* pDeclarationDestroy =
                        SymbolMap_FindObjFunction(&program->GlobalScope,
                            pStructUnionSpecifier->Name,
                            "Destroy");
                    if (pDeclarationDestroy)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "%s_Destroy(&%s);",
                            pStructUnionSpecifier->Name,
                            pInitExpressionText);
                        bComplete = true;
                    }
                }
            }
        }
        else if (action == ActionDelete)
        {
            //nao eh ponteiro
            if (bCanApplyFunction)
            {
                //vamos procurar pela funcao conceito _Delete
                TDeclaration* pDeclarationDestroy =
                    SymbolMap_FindObjFunction(&program->GlobalScope,
                        pStructUnionSpecifier->Name,
                        "Delete");
                if (pDeclarationDestroy)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "%s_Delete(%s);",
                        pStructUnionSpecifier->Name,
                        pInitExpressionText);
                    bComplete = true;
                }
                else
                {
                    //se nao achou delete procura a destroy 
                    //e depois chama free
                    TDeclaration* pDeclarationDestroy =
                        SymbolMap_FindObjFunction(&program->GlobalScope,
                            pStructUnionSpecifier->Name,
                            "Destroy");
                    if (pDeclarationDestroy)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * 1,
                            "%s_Destroy(%s);",
                            pStructUnionSpecifier->Name,
                            pInitExpressionText);
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "free(%s);",
                            pInitExpressionText);
                        bComplete = true;
                    }
                }
            }
            else
            {
                //nao procurar pela delete mas procurar pela destroy
                //posso procurar pela destroy
                //se nao achou delete procura a destroy 
                //e depois chama free
                TDeclaration* pDeclarationDestroy =
                    SymbolMap_FindObjFunction(&program->GlobalScope,
                        pStructUnionSpecifier->Name,
                        "Destroy");
                if (pDeclarationDestroy)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "if (%s){\n",
                        pInitExpressionText);
                    options->IdentationLevel++;
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "%s_Destroy(%s);",
                        pStructUnionSpecifier->Name,
                        pInitExpressionText);
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "free(%s);",
                        pInitExpressionText);
                    options->IdentationLevel--;
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "}\n",
                        pInitExpressionText);

                    bComplete = true;
                }
            }
        }
        else if (action == ActionInit)
        {
            if (bDeclaratorIsPointer)
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                    "%s = NULL;",
                    pInitExpressionText);
                bComplete = true;
            }
            else
            {
                TDeclaration* pDeclarationInit =
                    SymbolMap_FindObjFunction(&program->GlobalScope,
                        pStructUnionSpecifier->Name,
                        "Init");
                if (pDeclarationInit)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "%s_Init(&%s);",
                        pStructUnionSpecifier->Name,
                        pInitExpressionText);
                    bComplete = true;
                }
            }
        }
        else if (action == ActionInitContent)
        {
            if (bCanApplyFunction)
            {                
                TDeclaration* pDeclarationInit =
                    SymbolMap_FindObjFunction(&program->GlobalScope,
                        pStructUnionSpecifier->Name,
                        "Init");
                if (pDeclarationInit)
                {
                    if (bDeclaratorIsPointer)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "%s_Init(%s);",
                            pStructUnionSpecifier->Name,
                            pInitExpressionText);
                    }
                    else
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "%s_Init(&%s);",
                            pStructUnionSpecifier->Name,
                            pInitExpressionText);
                    }
                }                   
                bComplete = true;
            }
        }
        else if (action == ActionCreate)
        {
            if (true /*bCanApplyFunction*/)
            {
                TDeclaration* pDeclarationInit =
                    SymbolMap_FindObjFunction(&program->GlobalScope,
                        pStructUnionSpecifier->Name,
                        "Init");
                if (pDeclarationInit)
                {   
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "struct %s *p = malloc(sizeof * p);",
                        pStructUnionSpecifier->Name);
                    
                    
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "if (p)");
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "{");
                    options->IdentationLevel++;
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "%s_Init(p);",
                            pStructUnionSpecifier->Name);
                        
                        options->IdentationLevel--;

                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                            "}");
                        
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,                        
                            "return p;");

                        bComplete = true;
                }
                
            }
        }
        else if (action == ActionStaticInit)
        {
            if (bDeclaratorIsPointer)
            {
                StrBuilder_AppendFmt(fp, "/*%s=*/NULL", pInitExpressionText);
                bComplete = true;
            }
        }
        //Exemplos
        //struct Y *pY e � para destruir o conteudo
        //struct Y *pY e NAO � para destruir o conteudo
        //struct Y * auto pY; e � para destruir o conteudo ou nao
        //struct Y y; //eh para destruir o conteudo
        if (!bComplete) //se for para entrar na struct
        {
            if (pStructUnionSpecifier &&
                pStructUnionSpecifier->Name != NULL)
            {
                //se nao eh completa tenta achar
                //vou procurar a definicao completa da struct
                pStructUnionSpecifier =
                    SymbolMap_FindStructUnion(&program->GlobalScope, pStructUnionSpecifier->Name);
            }

            if (pStructUnionSpecifier->StructDeclarationList.size > 0)
            {


                if (action == ActionDelete)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "if (%s)", pInitExpressionText);
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "{", pInitExpressionText);
                    options->IdentationLevel++;
                }
                else if (action == ActionDestroy)
                {
                    if (bDeclaratorIsAutoPointer)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "if (%s)", pInitExpressionText);
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "{");
                        options->IdentationLevel++;
                    }
                }
                else if (action == ActionCreate)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "struct %s *p = malloc(sizeof * p);",
                        pStructUnionSpecifier->Name);

                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "if (p)");
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "{");
                    options->IdentationLevel++;
                }
                else if (action == ActionStaticInit)
                {
                    if (bCanApplyFunction)
                    {
                        //o primeiro nao precisa do {
                        
                        StrBuilder_AppendFmt(fp, "/*%s=*/{", pInitExpressionText);
                    }
                }

                //ok tem a definicao completa da struct
                for (int i = 0; i < pStructUnionSpecifier->StructDeclarationList.size; i++)
                {
                    if (action == ActionStaticInit && i > 0)
                    {
                        StrBuilder_Append(fp, ", ");
                    }

                    TAnyStructDeclaration* pAnyStructDeclaration =
                        pStructUnionSpecifier->StructDeclarationList.pItems[i];

                    TStructDeclaration* pStructDeclaration =
                        TAnyStructDeclaration_As_TStructDeclaration(pAnyStructDeclaration);

                    if (pStructDeclaration != NULL)
                    {
                        TStructDeclarator* pStructDeclarator =
                            pStructDeclaration->DeclaratorList.pHead;

                        bool bStructDeclaratorIsPointer =
                            TPointerList_IsPointer(&pStructDeclarator->pDeclarator->PointerList);

                        bool bStructDeclaratorIsAutoPointer =
                            TPointerList_IsAutoPointer(&pStructDeclarator->pDeclarator->PointerList);

                        StrBuilder strVariableName = STRBUILDER_INIT;


                        while (pStructDeclarator)
                        {
                            StrBuilder_Clear(&strVariableName);

                            const char* structDeclaratorName =
                                TDeclarator_GetName(pStructDeclarator->pDeclarator);
                            if (action != ActionStaticInit)
                            {
                                if (pInitExpressionText)
                                    StrBuilder_Set(&strVariableName, pInitExpressionText);

                                if (bDeclaratorIsPointer)
                                {
                                    StrBuilder_Append(&strVariableName, "->");
                                }
                                else
                                {
                                    StrBuilder_Append(&strVariableName, ".");
                                }
                            }
                            else
                            {
                                StrBuilder_Append(&strVariableName, ".");
                            }

                            StrBuilder_Append(&strVariableName, structDeclaratorName);

                            Action action2 = action;
                            if (action == ActionDestroyContent ||
                                action == ActionDelete)
                            {
                                //destroy normal
                                action2 = ActionDestroy;
                            }
                            else if (action == ActionInitContent)
                            {
                                action2 = ActionInit;
                            }
                            else if (action == ActionCreate)
                            {
                                action2 = ActionInit;
                            }

                            InstanciateDestroy2(program,
                                options,
                                &pStructDeclaration->SpecifierQualifierList,
                                pStructDeclarator->pDeclarator,
                                strVariableName.c_str,
                                action2,
                                true,
                                fp);

                            

                            pStructDeclarator = (pStructDeclarator)->pNext;
                        }

                        StrBuilder_Destroy(&strVariableName);
                    }
                }

                if (action == ActionDestroy)
                {
                    if (bDeclaratorIsAutoPointer)
                    {
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "free(%s);", pInitExpressionText);
                        options->IdentationLevel--;
                        StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "}");

                    }
                }
                else if (action == ActionDelete)
                {
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "free(%s);", pInitExpressionText);
                    options->IdentationLevel--;
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "}");

                }
                else if (action == ActionCreate)
                {
                    options->IdentationLevel--;
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "}");
                    
                    StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel,
                        "return p;");                    
                }
                else if (action == ActionStaticInit)
                {
                    if (bCanApplyFunction)
                    {
                        //o primeiro nao tem 
                        StrBuilder_Append(fp, "}");
                    }
                }
            }
            else
            {
                //error nao tem a definicao completa da struct
            }
            if (action != ActionStaticInit)
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "");
            }
        }//complete

    }
    else if (pMainSpecifier->Type == TEnumSpecifier_ID)
    {
        TEnumSpecifier *pEnumSpecifier =
            TSpecifier_As_TEnumSpecifier(pMainSpecifier);
        

        //nao eh typedef, deve ser int, double etc..
        if (action == ActionDestroy)
        {
            if (bDeclaratorIsAutoPointer)
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "free(%s);", pInitExpressionText);
            }
            else
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "//%s = 0;", pInitExpressionText);
            }
        }
        else if (action == ActionInit)
        {
            //TODO achar a definicao completa do enum
            const char* firstValue =
                pEnumSpecifier->EnumeratorList.pHead ? pEnumSpecifier->EnumeratorList.pHead->Name :
                "0";

            if (bDeclaratorIsPointer)
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "%s = NULL;", pInitExpressionText);
            }
            else
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "%s = %s;", pInitExpressionText, firstValue);
            }
        }
        else if (action == ActionInitContent)
        {
            //TODO achar a definicao completa do enum
            const char* firstValue =
                pEnumSpecifier->EnumeratorList.pHead ? pEnumSpecifier->EnumeratorList.pHead->Name :
                "0";

            if (bDeclaratorIsPointer)
            {
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "*%s = NULL;", pInitExpressionText);
            }
            else
            {
                
                StrBuilder_AppendFmtLn(fp, 4 * options->IdentationLevel, "%s = %s;", pInitExpressionText, firstValue);
            }
        }
        else if (action == ActionStaticInit)
        {
            //TODO achar a definicao completa do enum
            const char* firstValue =
                pEnumSpecifier->EnumeratorList.pHead ? pEnumSpecifier->EnumeratorList.pHead->Name :
                "0";

            if (bDeclaratorIsPointer)
            {
                StrBuilder_AppendFmt(fp, "/*%s=*/NULL", pInitExpressionText);
            }
            else
            {
                StrBuilder_AppendFmt(fp, "/*%s=*/%s", pInitExpressionText, firstValue);                
            }
        }
    }
    else
    {
        ASSERT(false);
    }

}



bool IsSuffix(const char* s, const char* suffix)
{
    bool bResult = false;
    int len = strlen(s);
    int len2 = strlen(suffix);
    if (len > len2)
    {
        const char* pEndPart = &s[len - len2];
        if (strcmp(pEndPart, suffix) == 0)
        {
            bResult = true;
        }
    }
    return bResult;

}

TStructUnionSpecifier* GetStructSpecifier(TProgram* program, TDeclarationSpecifiers* specifiers)
{
    TStructUnionSpecifier* pTStructUnionSpecifier =
        TSpecifier_As_TStructUnionSpecifier(specifiers->pHead);

    if (pTStructUnionSpecifier == NULL)
    {
        TSingleTypeSpecifier *pSingleTypeSpecifier =
            TSpecifier_As_TSingleTypeSpecifier(specifiers->pHead);

        if (pSingleTypeSpecifier != NULL &&
            pSingleTypeSpecifier->Token == TK_IDENTIFIER)
        {
            const char * typedefName = pSingleTypeSpecifier->TypedefName;

            TDeclaration * pDeclaration = TProgram_GetFinalTypeDeclaration(program, typedefName);
            if (pDeclaration)
            {
                pTStructUnionSpecifier =
                    TSpecifier_As_TStructUnionSpecifier(pDeclaration->Specifiers.pHead->pNext);

            }
        }
    }

    return pTStructUnionSpecifier;
}

