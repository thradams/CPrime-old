#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "Scanner.h"
#include "Path.h"
#include <stdarg.h>
//Define to include modications




#define List_Add(pList, pItem) \
if ((pList)->pHead == NULL) {\
    (pList)->pHead = (pItem); \
    (pList)->pTail = (pItem); \
}\
else {\
      (pList)->pTail->pNext = (pItem); \
      (pList)->pTail = (pItem); \
  }

void Declarator(struct Parser* ctx, bool bAbstract, struct TDeclarator** ppTDeclarator2);


enum Tokens Parser_Match(struct Parser* parser, struct TScannerItemList* listOpt);
enum Tokens Parser_MatchToken(struct Parser* parser,
    enum Tokens tk,
                              struct TScannerItemList* listOpt);
bool TTypeSpecifier_IsFirst(struct Parser* ctx, enum Tokens token, const char* lexeme);
//static bool Is_Type_QualifierFirst(enum Tokens token);
void Specifier_Qualifier_List(struct Parser* ctx, struct TSpecifierQualifierList* pSpecifierQualifierList);
static bool TTypeQualifier_IsFirst(enum Tokens token);


static bool IsPreprocessorTokenPhase(enum Tokens token)
{
    return
        token == TK_SPACES ||
        token == TK_COMMENT ||
        token == TK_OPEN_COMMENT ||
        token == TK_CLOSE_COMMENT ||
        token == TK_LINE_COMMENT ||
        token == TK_BREAKLINE ||
        token == TK_BACKSLASHBREAKLINE ||
        //enum Tokens para linhas do pre processador
        token == TK_PRE_INCLUDE ||
        token == TK_PRE_PRAGMA ||
        token == TK_PRE_IF ||
        token == TK_PRE_ELIF ||
        token == TK_PRE_IFNDEF ||
        token == TK_PRE_IFDEF ||
        token == TK_PRE_ENDIF ||
        token == TK_PRE_ELSE ||
        token == TK_PRE_ERROR ||
        token == TK_PRE_LINE ||
        token == TK_PRE_UNDEF ||
        token == TK_PRE_DEFINE ||
        //fim tokens preprocessador
        token == TK_MACRO_CALL ||
        token == TK_MACRO_EOF ||
        token == TK_FILE_EOF;
}




bool Declaration(struct Parser* ctx, struct TAnyDeclaration** ppDeclaration);

int IsTypeName(struct Parser* ctx, enum Tokens token, const char* lexeme);



bool Parser_InitString(struct Parser* parser,
                       const char* name,
                       const char* Text)
{
    parser->ParserOptions.bNoImplicitTag = false;
    TScannerItemList_Init(&parser->ClueList);
    parser->IncludeLevel = 0;
    ///////
    SymbolMap_Init(&parser->GlobalScope);
    parser->pCurrentScope = &parser->GlobalScope;
    parser->bPreprocessorEvalFlag = false;
    /////////

    //Map_Init(&parser->TypeDefNames, SYMBOL_BUCKETS_SIZE);
    parser->bError = false;

    StrBuilder_Init(&parser->ErrorMessage);


    /////////
    Scanner_InitString(&parser->Scanner, name, Text);

    //sair do BOF
    struct TScannerItemList clueList0 = { 0 };
    Parser_Match(parser, &clueList0);
    TScannerItemList_Destroy(&clueList0);

    return true;
}

bool Parser_InitFile(struct Parser* parser, const char* fileName)
{

    parser->ParserOptions.bNoImplicitTag = false;
    parser->IncludeLevel = 0;
    parser->bPreprocessorEvalFlag = false;
    TScannerItemList_Init(&parser->ClueList);

    /////
    SymbolMap_Init(&parser->GlobalScope);
    parser->pCurrentScope = &parser->GlobalScope;

    //////
    //Map_Init(&parser->TypeDefNames, SYMBOL_BUCKETS_SIZE);
    parser->bError = false;


    StrBuilder_Init(&parser->ErrorMessage);
    Scanner_Init(&parser->Scanner);


    ////////
    Scanner_IncludeFile(&parser->Scanner, fileName, FileIncludeTypeFullPath, false);

    //Scanner_Match(&parser->Scanner);
    //sair do BOF
    struct TScannerItemList clueList0 = { 0 };
    Parser_Match(parser, &clueList0);
    TScannerItemList_Destroy(&clueList0);

    return true;
}

void Parser_PushFile(struct Parser* parser, const char* fileName)
{
    Scanner_IncludeFile(&parser->Scanner, fileName, FileIncludeTypeFullPath, false);


    struct TScannerItemList clueList0 = { 0 };
    Parser_Match(parser, &clueList0);
    //assert(clueList0.pHead == NULL);
    TScannerItemList_Destroy(&clueList0);

}





void Parser_Destroy(struct Parser* parser)
{
    TScannerItemList_Destroy(&parser->ClueList);

    SymbolMap_Destroy(&parser->GlobalScope);

    //Map_Destroy(&parser->TypeDefNames, NULL);
    StrBuilder_Destroy(&parser->ErrorMessage);
    Scanner_Destroy(&parser->Scanner);
}

static const char* GetName()
{
    static char buffer[2];
    buffer[0] = '\0';
    //static int i = 0;
    //strcat(buffer, "__noname");
    //char bn[20];
    //_itoa(i, bn, 10);
    //strcat(buffer, bn);
    //i++;
    return buffer;
}

bool Parser_HasError(struct Parser* pParser)
{
    return pParser->bError || pParser->Scanner.bError;
}

void SetWarning(struct Parser* parser, const char* fmt, ...)
{
    struct StrBuilder warningMessage = STRBUILDER_INIT;
    Scanner_GetFilePositionString(&parser->Scanner, &warningMessage);
    va_list args;
    va_start(args, fmt);
    StrBuilder_AppendFmtV(&warningMessage, fmt, args);
    va_end(args);

    printf("%s\n", warningMessage.c_str);

    StrBuilder_Destroy(&warningMessage);
}

void SetError(struct Parser* parser, const char* fmt, ...)
{
    //    //assert(false);
    if (!Parser_HasError(parser))
    {
        Scanner_GetFilePositionString(&parser->Scanner, &parser->ErrorMessage);
        parser->bError = true;
        va_list args;
        va_start(args, fmt);
        StrBuilder_AppendFmtV(&parser->ErrorMessage, fmt, args);
        va_end(args);
    }

    else
    {
        //ja esta com erro entao eh ruido...
        parser->bError = true;
    }
}


int GetCurrentLine(struct Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return -1;
    }

    return Scanner_LineAt(&parser->Scanner, 0);
}

int GetFileIndex(struct Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return -1;
    }

    return Scanner_FileIndexAt(&parser->Scanner, 0);
}

static void GetPosition(struct Parser* ctx, struct TFilePos* pPosition)
{
    pPosition->Line = GetCurrentLine(ctx);
    pPosition->FileIndex = GetFileIndex(ctx);
}

enum Tokens Parser_LookAheadToken(struct Parser* parser)
{
    enum Tokens token = TK_ERROR;

    if (!Parser_HasError(parser))
    {
        for (int i = 1; i < 10; i++)
        {
            token = Scanner_TokenAt(&parser->Scanner, i);
            bool bActive = Scanner_IsActiveAt(&parser->Scanner, i);
            if (bActive && !IsPreprocessorTokenPhase(token))
            {
                break;
            }
        }
    }

    return token;
}


const char* Parser_LookAheadLexeme(struct Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return "";
    }

    const char* lexeme = NULL;

    if (!Parser_HasError(parser))
    {
        for (int i = 1; i < 10; i++)
        {
            enum Tokens token = Scanner_TokenAt(&parser->Scanner, i);
            bool bActive = Scanner_IsActiveAt(&parser->Scanner, i);
            if (bActive && !IsPreprocessorTokenPhase(token))
            {
                lexeme = Scanner_LexemeAt(&parser->Scanner, i);
                break;
            }
        }
    }

    return lexeme;
}


enum Tokens Parser_CurrentToken(struct Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return TK_ERROR;
    }

    enum Tokens token = Scanner_TokenAt(&parser->Scanner, 0);

    if (IsPreprocessorTokenPhase(token))
    {
        SetError(parser, "!IsPreprocessorTokenPhase");
    }
    return token;
}

enum Tokens Parser_MatchEx(struct Parser* parser, struct TScannerItemList* listOpt, bool bKeepComment)
{
    ////assert(listOpt != NULL);
    enum Tokens token = TK_EOF;
    if (!Parser_HasError(parser))
    {

        if (listOpt)
        {
            TScannerItemList_Swap(listOpt, &parser->ClueList);
        }

        Scanner_Match(&parser->Scanner);

        token = Scanner_TokenAt(&parser->Scanner, 0);
        while (token != TK_EOF &&
               token != TK_NONE &&
               (!Scanner_IsActiveAt(&parser->Scanner, 0) ||
               IsPreprocessorTokenPhase(token)))
        {
            //so adiciona os espacos no cara atual
            struct ScannerItem* pNew = ScannerItem_Create();
            LocalStrBuilder_Set(&pNew->lexeme, Scanner_LexemeAt(&parser->Scanner, 0));
            pNew->token = Scanner_TokenAt(&parser->Scanner, 0);
            TScannerItemList_PushBack(&parser->ClueList, pNew);


            Scanner_Match(&parser->Scanner);
            token = Scanner_TokenAt(&parser->Scanner, 0);
        }
    }

    return token;
}
enum Tokens Parser_Match(struct Parser* parser, struct TScannerItemList* listOpt)
{
    return Parser_MatchEx(parser, listOpt, true);
}

enum Tokens Parser_MatchToken(struct Parser* parser,
    enum Tokens tk,
                              struct TScannerItemList* listOpt)
{
    if (Parser_HasError(parser))
    {
        return TK_EOF;
    }

    enum Tokens currentToken = Parser_CurrentToken(parser);

    if (tk != currentToken)
    {
        SetError(parser, "Unexpected token - %s", TokenToString(tk));
        return TK_EOF;
    }

    Parser_Match(parser, listOpt);
    return Parser_CurrentToken(parser);
}

const char* GetCompletationMessage(struct Parser* parser)
{
    const char* pMessage = "ok";

    if (Parser_HasError(parser))
    {
        if (parser->Scanner.bError)
        {
            pMessage = parser->Scanner.ErrorString.c_str;
        }

        else
        {
            pMessage = parser->ErrorMessage.c_str;
        }
    }

    return pMessage;
}

const char* Lexeme(struct Parser* parser)
{
    if (Parser_HasError(parser))
    {
        //assert(false);
        return "";
    }

    return Scanner_LexemeAt(&parser->Scanner, 0);
}

bool ErrorOrEof(struct Parser* parser)
{
    return Parser_HasError(parser) ||
        Parser_CurrentToken(parser) == TK_EOF;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


void Expression0(struct Parser* ctx, struct TExpression**);
void CastExpression(struct Parser* ctx, struct TExpression**);
void GenericSelection(struct Parser* ctx);
void ArgumentExpressionList(struct Parser* ctx, struct TExpression**);
void AssignmentExpression(struct Parser* ctx, struct TExpression**);
void Initializer_List(struct Parser* ctx, struct TInitializerList* pInitializerList);



bool IsFirstOfPrimaryExpression(enum Tokens token)
{
    bool bResult = false;

    switch (token)
    {
        case TK_IDENTIFIER:
        case TK_STRING_LITERAL:
        case TK_CHAR_LITERAL:
        case TK_DECIMAL_INTEGER:
        case TK_HEX_INTEGER:
        case TK_FLOAT_NUMBER:
        case TK_LEFT_PARENTHESIS:

            //////////
            //extensions
        case TK_LEFT_SQUARE_BRACKET: //lambda-expression
                                     /////////

                                     //desde que nao seja cast
        case TK__GENERIC:
            bResult = true;
            break;

        default:
            break;
    }

    return bResult;
}

void PrimaryExpressionLiteral(struct Parser* ctx, struct TExpression** ppPrimaryExpression)
{
    enum Tokens token = Parser_CurrentToken(ctx);
    //const char* lexeme = Lexeme(ctx);

    struct TPrimaryExpressionLiteral* pPrimaryExpressionLiteral
        = TPrimaryExpressionLiteral_Create();

    *ppPrimaryExpression = (struct TExpression*)pPrimaryExpressionLiteral;

    while (token == TK_STRING_LITERAL)
    {
        struct TPrimaryExpressionLiteralItem* pPrimaryExpressionLiteralItem
            = TPrimaryExpressionLiteralItem_Create();
        const char* lexeme2 = Lexeme(ctx);
        PTR_STRING_REPLACE(pPrimaryExpressionLiteralItem->lexeme, lexeme2);

        token = Parser_Match(ctx,
                             &pPrimaryExpressionLiteralItem->ClueList0);

        TPrimaryExpressionLiteralItemList_Add(&pPrimaryExpressionLiteral->List, pPrimaryExpressionLiteralItem);
    }
}

void Compound_Statement(struct Parser* ctx, struct TStatement** ppStatement);

void Parameter_Type_List(struct Parser* ctx, struct TParameterTypeList* pParameterList);

void LambdaExpression(struct Parser* ctx, struct TExpression** ppPrimaryExpression)
{
    //c++
    /*
    lambda-expression:
    lambda-introducer lambda-declaratoropt compound-statement

    lambda-introducer:
    [ lambda-captureopt]

    lambda-capture:
    capture-default
    capture-list
    capture-default , capture-list

    capture-default:
    &=

    capture-list:
    capture ...opt
    capture-list , capture ...opt

    capture:
    simple-capture
    init-capture

    simple-capture:
    identifier
    & identifier
    this

    init-capture:
    identifier initializer
    & identifier initializer

    lambda-declarator:
    ( parameter-declaration-clause ) mutableopt
    exception-specificationopt attribute-specifier-seqopt trailing-return-typeopt
    */

    /*
    lambda-expression:
    [] ( parameters opt ) compound-statement
    [] compound-statement
    */
    struct TPrimaryExpressionLambda* pPrimaryExpressionLambda = TPrimaryExpressionLambda_Create();

    *ppPrimaryExpression = (struct TExpression*)pPrimaryExpressionLambda; //out

    Parser_MatchToken(ctx,
                      TK_LEFT_SQUARE_BRACKET,
                      &pPrimaryExpressionLambda->ClueList0);

    Parser_MatchToken(ctx,
                      TK_RIGHT_SQUARE_BRACKET,
                      &pPrimaryExpressionLambda->ClueList1);

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_LEFT_PARENTHESIS)
    {
        token = Parser_MatchToken(ctx,
                                  TK_LEFT_PARENTHESIS,
                                  &pPrimaryExpressionLambda->ClueList2);

        pPrimaryExpressionLambda->pParameterTypeListOpt =
            TParameterTypeList_Create();

        if (token != TK_RIGHT_PARENTHESIS)
        {
            Parameter_Type_List(ctx, pPrimaryExpressionLambda->pParameterTypeListOpt);
        }

        Parser_MatchToken(ctx,
                          TK_RIGHT_PARENTHESIS,
                          &pPrimaryExpressionLambda->ClueList3);
    }

    Compound_Statement(ctx, (struct TStatement**)&pPrimaryExpressionLambda->pCompoundStatement);
}

void PrimaryExpression(struct Parser* ctx, struct TExpression** ppPrimaryExpression)
{
    *ppPrimaryExpression = NULL;



    /*
    (6.5.1) primary-expression:
    identifier
    constant
    char-literal
    ( expression )
    generic-selection
    */
    * ppPrimaryExpression = NULL; //out

    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);
    //PreprocessorTokenIndex(ctx);
    //-2 nem eh macro
    //-1 inicio de macro
    //-3 fim de macro


    if (!IsFirstOfPrimaryExpression(token))
    {
        SetError(ctx, "unexpected error IsFirstOfPrimaryExpression");
    }

    switch (token)
    {

        case TK_LEFT_SQUARE_BRACKET:
            LambdaExpression(ctx, ppPrimaryExpression);
            break;

        case TK_STRING_LITERAL:
            PrimaryExpressionLiteral(ctx, ppPrimaryExpression);
            break;

        case TK_IDENTIFIER:
        {
            struct TTypePointer* pTypePointer = SymbolMap_Find(ctx->pCurrentScope, lexeme);
            if (pTypePointer == NULL)
            {
                if (!ctx->bPreprocessorEvalFlag)
                {
                    if (strcmp(lexeme, "__FUNCTION__") == 0)
                    {
                        //identificador magico
                    }
                    else
                    {
                        SetWarning(ctx, "Warning: '%s': undeclared identifier\n", lexeme);
                    }
                }
            }



            struct TPrimaryExpressionValue* pPrimaryExpressionValue
                = TPrimaryExpressionValue_Create();

            pPrimaryExpressionValue->token = token;
            PTR_STRING_REPLACE(pPrimaryExpressionValue->lexeme, lexeme);

            if (pTypePointer && pTypePointer->Type == TDeclaration_ID)
            {
                //eh uma variavel que aponta para uma declaracao
                pPrimaryExpressionValue->pDeclaration = (struct TDeclaration*)pTypePointer;
            }

            if (pTypePointer && pTypePointer->Type == TParameter_ID)
            {
                //eh uma variavel que aponta para um  parametro
                pPrimaryExpressionValue->pParameter = (struct TParameter*)pTypePointer;
            }

            Parser_Match(ctx,
                         &pPrimaryExpressionValue->ClueList0);
            *ppPrimaryExpression = (struct TExpression*)pPrimaryExpressionValue;
        }
        break;

        case TK_CHAR_LITERAL:
        case TK_DECIMAL_INTEGER:
        case TK_HEX_INTEGER:
        case TK_FLOAT_NUMBER:
        {
            struct TPrimaryExpressionValue* pPrimaryExpressionValue
                = TPrimaryExpressionValue_Create();

            pPrimaryExpressionValue->token = token;
            PTR_STRING_REPLACE(pPrimaryExpressionValue->lexeme, Lexeme(ctx));


            Parser_Match(ctx,
                         &pPrimaryExpressionValue->ClueList0);
            *ppPrimaryExpression = (struct TExpression*)pPrimaryExpressionValue;
        }
        break;

        case TK_LEFT_PARENTHESIS:
        {
            struct TPrimaryExpressionValue* pPrimaryExpressionValue
                = TPrimaryExpressionValue_Create();

            Parser_Match(ctx,
                         &pPrimaryExpressionValue->ClueList0);

            struct TExpression* pExpression;
            Expression0(ctx, &pExpression);

            //    //TNodeClueList_MoveToEnd(&pPrimaryExpressionValue->ClueList, &ctx->Scanner.ClueList);
            Parser_MatchToken(ctx,
                              TK_RIGHT_PARENTHESIS,
                              &pPrimaryExpressionValue->ClueList1);


            pPrimaryExpressionValue->token = token;
            PTR_STRING_REPLACE(pPrimaryExpressionValue->lexeme, Lexeme(ctx));
            pPrimaryExpressionValue->pExpressionOpt = pExpression;


            *ppPrimaryExpression = (struct TExpression*)pPrimaryExpressionValue;
        }
        break;

        case TK__GENERIC:
            GenericSelection(ctx);
            break;

        default:
            SetError(ctx, "unexpected error");
    }

    if (*ppPrimaryExpression == NULL)
    {
        SetError(ctx, "unexpected error NULL");
    }

}

void GenericSelection(struct Parser* ctx)
{
    //_Generic

    /*
    (6.5.1.1) generic-selection:
    _Generic ( assignment-expression , generic-assoc-list )
    */
}

void GenericAssocList(struct Parser* ctx)
{

    //type-name default

    /*
    (6.5.1.1) generic-assoc-list:
    generic-association
    generic-assoc-list , generic-association
    */
}

void GenericAssociation(struct Parser* ctx)
{
    //type-name default

    /*
    (6.5.1.1) generic-association:
    type-name : assignment-expression
    default : assignment-expression
    */
}

void TTypeName_Swap(struct TTypeName* a, struct TTypeName* b)
{
    struct TTypeName temp = *a;
    *a = *b;
    *b = temp;
}

void TTypeName_Init(struct TTypeName* p) /*@default*/
{
    p->Type = TTypeName_ID;
    p->SpecifierQualifierList.pData = NULL;
    p->SpecifierQualifierList.Size = 0;
    p->SpecifierQualifierList.Capacity = 0;
    TDeclarator_Init(&p->Declarator);
}



void TypeName(struct Parser* ctx, struct TTypeName* pTypeName)
{
    /*
    type-name:
    specifier-qualifier-list abstract-declaratoropt
    */

    Specifier_Qualifier_List(ctx, &pTypeName->SpecifierQualifierList);

    struct TDeclarator* pDeclarator = NULL;
    Declarator(ctx, true, &pDeclarator);
    if (pDeclarator)
    {
        TDeclarator_Swap(&pTypeName->Declarator, pDeclarator);
        TDeclarator_Delete(pDeclarator);
    }
}


static void PostfixExpressionCore(struct Parser* ctx, struct TPostfixExpressionCore* pPostfixExpressionCore)
{
    //ja entra aqui sendo uma  postfix-expression antes
    /*
    (6.5.2) postfix-expression:

    postfix-expression [ expression ]
    postfix-expression ( argument-expression-listopt )
    postfix-expression . identifier
    postfix-expression -> identifier
    postfix-expression ++
    postfix-expression --

    */

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_LEFT_PARENTHESIS:
        {
            //aqui eu posso marcar a funcao como usada.

            pPostfixExpressionCore->token = token;


            //  postfix-expression ( argument-expression-listopt )    
            token = Parser_Match(ctx,
                                 &pPostfixExpressionCore->ClueList0);

            if (token != TK_RIGHT_PARENTHESIS)
            {
                ArgumentExpressionList(ctx, &pPostfixExpressionCore->pExpressionRight);
            }

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS,
                              &pPostfixExpressionCore->ClueList1);
        }
        break;

        case TK_LEFT_SQUARE_BRACKET:
        {
            pPostfixExpressionCore->token = token;
            // postfix-expression [ expression ]

            Parser_MatchToken(ctx, TK_LEFT_SQUARE_BRACKET,
                              &pPostfixExpressionCore->ClueList0);

            Expression0(ctx, &pPostfixExpressionCore->pExpressionRight);

            Parser_MatchToken(ctx, TK_RIGHT_SQUARE_BRACKET, &pPostfixExpressionCore->ClueList1);
        }
        break;

        case TK_FULL_STOP:
        {
            // postfix-expression . identifier
            pPostfixExpressionCore->token = token;

            Parser_Match(ctx, &pPostfixExpressionCore->ClueList0);

            PTR_STRING_REPLACE(pPostfixExpressionCore->Identifier, Lexeme(ctx));

            Parser_MatchToken(ctx, TK_IDENTIFIER,
                              &pPostfixExpressionCore->ClueList1);
        }
        break;

        case TK_ARROW:
        {
            // postfix-expression -> identifier
            pPostfixExpressionCore->token = token;

            Parser_Match(ctx, &pPostfixExpressionCore->ClueList0);

            PTR_STRING_REPLACE(pPostfixExpressionCore->Identifier, Lexeme(ctx));

            Parser_MatchToken(ctx, TK_IDENTIFIER, &pPostfixExpressionCore->ClueList1);
        }
        break;

        case TK_PLUSPLUS:
        {
            pPostfixExpressionCore->token = token;
            //postfix-expression ++

            Parser_Match(ctx, &pPostfixExpressionCore->ClueList0);
        }
        break;

        case TK_MINUSMINUS:
        {
            //  postfix-expression --
            pPostfixExpressionCore->token = token;
            Parser_Match(ctx, &pPostfixExpressionCore->ClueList0);
        }
        break;

        default:
            // //assert(false);
            break;
    }


    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_LEFT_PARENTHESIS:
        case TK_LEFT_SQUARE_BRACKET:
        case TK_FULL_STOP:
        case TK_ARROW:
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:
        {
            struct TPostfixExpressionCore* pPostfixExpressionCoreNext =
                TPostfixExpressionCore_Create();
            PostfixExpressionCore(ctx, pPostfixExpressionCoreNext);

            if (pPostfixExpressionCore->pNext != NULL)
            {
                SetError(ctx, "unexpected error");
            }
            pPostfixExpressionCore->pNext = pPostfixExpressionCoreNext;
        }
        break;
        default:
            //assert(false);
            break;
    }


}

void PostfixExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    *ppExpression = NULL;//out

                         /*
                         (6.5.2) postfix-expression:
                         postfix-expression ( argument-expression-listopt )
                         ( type-name ) { initializer-list }
                         ( type-name ) { initializer-list , }

                         primary-expression
                         postfix-expression [ expression ]
                         postfix-expression . identifier
                         postfix-expression -> identifier
                         postfix-expression ++
                         postfix-expression --

                         */

    enum Tokens token = Parser_CurrentToken(ctx);



    if (token == TK_LEFT_PARENTHESIS)
    {
        const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
        enum Tokens lookAheadToken = Parser_LookAheadToken(ctx);

        if (IsTypeName(ctx, lookAheadToken, lookAheadlexeme))
        {
            // ( type-name ) { initializer-list }

            struct TPostfixExpressionCore* pTPostfixExpressionCore =
                TPostfixExpressionCore_Create();

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pTPostfixExpressionCore->ClueList0);

            struct TTypeName typeName;
            TTypeName_Init(&typeName);

            TypeName(ctx, &typeName);
            TTypeName_Destroy(&typeName);

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pTPostfixExpressionCore->ClueList1);

            Parser_MatchToken(ctx, TK_LEFT_CURLY_BRACKET, &pTPostfixExpressionCore->ClueList2);

            Initializer_List(ctx, &pTPostfixExpressionCore->InitializerList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET, &pTPostfixExpressionCore->ClueList3);

            if (Parser_CurrentToken(ctx) == TK_COMMA)
            {
                Parser_Match(ctx, &pTPostfixExpressionCore->ClueList4);
            }

            *ppExpression = (struct TExpression*)pTPostfixExpressionCore;
        }

        else
        {
            //primary-expression
            struct TExpression* pPrimaryExpression;
            PrimaryExpression(ctx, &pPrimaryExpression);
            *ppExpression = pPrimaryExpression;
        }
    }
    else
    {
        //tem que ser?
        //assert(IsFirstOfPrimaryExpression(token));

        //primary-expression
        struct TExpression* pPrimaryExpression;
        PrimaryExpression(ctx, &pPrimaryExpression);
        *ppExpression = pPrimaryExpression;
    }

    token = Parser_CurrentToken(ctx);
    if (IsFirstOfPrimaryExpression(token))
    {

        // //assert(false); //pergunta deve continuar? ta certo?

        struct TPostfixExpressionCore* pPostfixExpressionCore =
            TPostfixExpressionCore_Create();
        pPostfixExpressionCore->pExpressionLeft = *ppExpression;
        PostfixExpressionCore(ctx, pPostfixExpressionCore);
        *ppExpression = (struct TExpression*)pPostfixExpressionCore;
    }
    else
    {
        switch (token)
        {
            case TK_LEFT_PARENTHESIS:
            {
                struct TPostfixExpressionCore* pPostfixExpressionCore =
                    TPostfixExpressionCore_Create();
                pPostfixExpressionCore->pExpressionLeft = *ppExpression;

                char* /*@auto*/ lexemeCopy = NULL;

                struct TPrimaryExpressionValue* ppri =
                    TExpression_As_TPrimaryExpressionValue(*ppExpression);
                if (ppri)
                {
                    PTR_STRING_REPLACE(lexemeCopy, ppri->lexeme);
                }
                PostfixExpressionCore(ctx, pPostfixExpressionCore);
                *ppExpression = (struct TExpression*)pPostfixExpressionCore;



                Free(lexemeCopy);
            }
            break;
            case TK_LEFT_SQUARE_BRACKET:
            case TK_FULL_STOP:
            case TK_ARROW:
            case TK_PLUSPLUS:
            case TK_MINUSMINUS:
            {
                struct TPostfixExpressionCore* pPostfixExpressionCore =
                    TPostfixExpressionCore_Create();
                pPostfixExpressionCore->pExpressionLeft = *ppExpression;
                PostfixExpressionCore(ctx, pPostfixExpressionCore);
                *ppExpression = (struct TExpression*)pPostfixExpressionCore;
            }
            break;
            default:
                //assert(false);
                break;
        }

    }

}

void ArgumentExpressionList(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.2) argument-expression-list:
    assignment-expression
    argument-expression-list , assignment-expression
    */
    struct TExpression* pAssignmentExpression;
    AssignmentExpression(ctx, &pAssignmentExpression);
    *ppExpression = pAssignmentExpression;

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        struct TBinaryExpression* pExpr =
            TBinaryExpression_Create();

        Parser_Match(ctx, &pExpr->ClueList0);// &pAssignmentExpression->ClueList);

        struct TExpression* pAssignmentExpressionRight;
        AssignmentExpression(ctx, &pAssignmentExpressionRight);


        GetPosition(ctx, &pExpr->Position);
        pExpr->token = TK_COMMA;
        pExpr->pExpressionLeft = pAssignmentExpression;
        pExpr->pExpressionRight = pAssignmentExpressionRight;



        *ppExpression = (struct TExpression*)pExpr;
    }

    token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        struct TBinaryExpression* pExpr =
            TBinaryExpression_Create();

        Parser_Match(ctx, &pExpr->ClueList0);

        GetPosition(ctx, &pExpr->Position);
        pExpr->token = TK_COMMA;
        pExpr->pExpressionLeft = *ppExpression;

        struct TExpression* pExpressionRight;
        ArgumentExpressionList(ctx, &pExpressionRight);
        pExpr->pExpressionRight = pExpressionRight;

        *ppExpression = (struct TExpression*)pExpr;
    }
}



static bool IsTypeQualifierToken(enum Tokens token)
{
    bool bResult = false;
    switch (token)
    {
        //type-qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
        case TK__ATOMIC:
            //
            bResult = true;
            break;
#ifdef LANGUAGE_EXTENSIONS

            //type-qualifier-extensions 


        case TK_AUTO:
        case TK_LEFT_SQUARE_BRACKET:


            bResult = true;
            break;
#endif
        default:
            //assert(false);
            break;
    }
    return bResult;
}

int IsTypeName(struct Parser* ctx, enum Tokens token, const char* lexeme)
{

    int bResult = false;

    if (lexeme == NULL)
    {
        return false;
    }

    switch (token)
    {

        case TK_IDENTIFIER:
            bResult = SymbolMap_IsTypeName(ctx->pCurrentScope, lexeme);
            //        bResult = DeclarationsMap_IsTypeDef(&ctx->Symbols, lexeme);
            break;

            //type-qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
        case TK__ATOMIC:

#ifdef LANGUAGE_EXTENSIONS

        case TK_AUTO:


#endif

            //type-specifier
        case TK_VOID:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_LONG:
            //microsoft
        case TK__INT8:
        case TK__INT16:
        case TK__INT32:
        case TK__INT64:
        case TK__WCHAR_T:
            //
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK__BOOL:
        case TK__COMPLEX:
        case TK_STRUCT:
        case TK_UNION:
        case TK_ENUM:
            bResult = true;
            break;
        default:
            //assert(false);
            break;
    }

    return bResult;
}

void UnaryExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    *ppExpression = NULL; //out

                          /*(6.5.3) unary-expression:
                          postfix-expression
                          ++ unary-expression
                          -- unary-expression
                          unary-operator cast-expression
                          sizeof unary-expression
                          sizeof ( type-name )
                          alignof ( type-name )

                          unary-operator: one of
                          & * + - ~ !
                          */

    enum Tokens token0 = Parser_CurrentToken(ctx);
    enum Tokens tokenAhead = Parser_LookAheadToken(ctx);
    const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);

    if (IsTypeName(ctx, tokenAhead, lookAheadlexeme))
    {
        //first para postfix-expression
        struct TExpression* pPostfixExpression;
        PostfixExpression(ctx, &pPostfixExpression);
        *ppExpression = (struct TExpression*)(pPostfixExpression);
        return;
    }

    else if (IsFirstOfPrimaryExpression(token0))
    {
        //primary-expression é first para postfix-expression

        struct TExpression* pPostfixExpression;
        PostfixExpression(ctx, &pPostfixExpression);
        *ppExpression = (struct TExpression*)(pPostfixExpression);
        return;
    }

    switch (token0)
    {
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:
        {
            struct TUnaryExpressionOperator* pUnaryExpressionOperator =
                TUnaryExpressionOperator_Create();

            Parser_Match(ctx, &pUnaryExpressionOperator->ClueList0);

            struct TExpression* pUnaryExpression;
            UnaryExpression(ctx, &pUnaryExpression);


            pUnaryExpressionOperator->token = token0;
            pUnaryExpressionOperator->pExpressionRight = pUnaryExpression;
            *ppExpression = (struct TExpression*)pUnaryExpressionOperator;
        }
        break;

        //unary-operator cast-expression
        case TK_AMPERSAND:
        case TK_ASTERISK:
        case TK_PLUS_SIGN:
        case TK_HYPHEN_MINUS:
        case TK_TILDE:
        case TK_EXCLAMATION_MARK:
        {
            struct TUnaryExpressionOperator* pUnaryExpressionOperator =
                TUnaryExpressionOperator_Create();

            Parser_Match(ctx, &pUnaryExpressionOperator->ClueList0);
            struct TExpression* pCastExpression;
            CastExpression(ctx, &pCastExpression);

            pUnaryExpressionOperator->token = token0;
            pUnaryExpressionOperator->pExpressionRight = pCastExpression;
            *ppExpression = (struct TExpression*)pUnaryExpressionOperator;

        }
        break;

        //////////////

        case TK_SIZEOF:
        {
            struct TUnaryExpressionOperator* pUnaryExpressionOperator =
                TUnaryExpressionOperator_Create();
            *ppExpression = (struct TExpression*)pUnaryExpressionOperator;

            pUnaryExpressionOperator->token = token0;

            Parser_MatchToken(ctx, TK_SIZEOF, &pUnaryExpressionOperator->ClueList0);

            if (Parser_CurrentToken(ctx) == TK_LEFT_PARENTHESIS)
            {
                const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
                enum Tokens lookAheadToken = Parser_LookAheadToken(ctx);

                if (IsTypeName(ctx, lookAheadToken, lookAheadlexeme))
                {
                    //sizeof(type-name)
                    Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pUnaryExpressionOperator->ClueList1);
                    TypeName(ctx, &pUnaryExpressionOperator->TypeName);
                    Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pUnaryExpressionOperator->ClueList2);
                }
                else
                {
                    //sizeof unary-expression
                    struct TExpression* pTUnaryExpression;
                    UnaryExpression(ctx, &pTUnaryExpression);
                    pUnaryExpressionOperator->pExpressionRight = pTUnaryExpression;
                }
            }
            else
            {
                //sizeof do tipo desta expressao
                struct TExpression* pTUnaryExpression;
                UnaryExpression(ctx, &pTUnaryExpression);
                pUnaryExpressionOperator->pExpressionRight = pTUnaryExpression;
            }
        }
        break;

        case TK__ALINGOF:
            //Match
            //assert(false);
            break;
        case TK_EOF:
            break;
            //TODO ver tudo que pode ser follow
        default:
            ////assert(false);
            //        SetUnexpectedError(ctx, "Assert", "");
            //aqui nao eh erro necessariamente
            break;
    }

}

void CastExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    *ppExpression = NULL; //out

                          /*
                          cast-expression:
                          unary-expression
                          ( type-name ) cast-expression
                          */

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_LEFT_PARENTHESIS)
    {
        const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
        enum Tokens lookAheadToken = Parser_LookAheadToken(ctx);

        if (IsTypeName(ctx, lookAheadToken, lookAheadlexeme))
        {
            struct TScannerItemList tempList0 = { 0, 0 };
            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &tempList0);

            struct TTypeName typeName;
            TTypeName_Init(&typeName);
            TypeName(ctx, &typeName);

            struct TScannerItemList tempList1 = { 0, 0 };
            token = Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &tempList1);

            if (token == TK_LEFT_CURLY_BRACKET)
            {
                //se isso acontecer, quer dizer que eh uma postfix-expression
                //entao deveria ter sido expandido com
                //unary-expression -> postfix-expression ->
                /*
                (6.5.2) postfix-expression:
                ...
                ( type-name ) { initializer-list }
                ( type-name ) { initializer-list , }
                */

                Parser_MatchToken(ctx, TK_LEFT_CURLY_BRACKET, NULL);

                struct TPostfixExpressionCore* pTPostfixExpressionCore =
                    TPostfixExpressionCore_Create();
                pTPostfixExpressionCore->pTypeName = TTypeName_Create();
                TTypeName_Swap(pTPostfixExpressionCore->pTypeName, &typeName);

                //pTPostfixExpressionCore->pInitializerList = TInitializerList_Create();
                Initializer_List(ctx, &pTPostfixExpressionCore->InitializerList);

                //Initializer_List(ctx, pTPostfixExpressionCore->pInitializerList);


                if (Parser_CurrentToken(ctx) == TK_COMMA)
                {
                    Parser_Match(ctx, NULL);
                }
                Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET, NULL);
                *ppExpression = (struct TExpression*)pTPostfixExpressionCore;
                //PostfixExpressionCore(ctx, pTPostfixExpressionCore);
            }
            else
            {
                struct TCastExpressionType* pCastExpressionType =
                    TCastExpressionType_Create();

                TScannerItemList_Swap(&tempList0, &pCastExpressionType->ClueList0);
                TScannerItemList_Swap(&tempList1, &pCastExpressionType->ClueList1);

                struct TExpression* pCastExpression;
                CastExpression(ctx, &pCastExpression);

                TTypeName_Swap(&pCastExpressionType->TypeName, &typeName);
                pCastExpressionType->pExpression = pCastExpression;
                *ppExpression = (struct TExpression*)pCastExpressionType;

            }

            TTypeName_Destroy(&typeName);
            TScannerItemList_Destroy(&tempList0);
            TScannerItemList_Destroy(&tempList1);
        }
        else
        {
            struct TExpression* pUnaryExpression;
            UnaryExpression(ctx, &pUnaryExpression);
            *ppExpression = pUnaryExpression;
        }
    }
    else
    {
        struct TExpression* pUnaryExpression;
        UnaryExpression(ctx, &pUnaryExpression);

        *ppExpression = pUnaryExpression;
    }
}


void MultiplicativeExpression(struct Parser* ctx, struct TExpression** ppExpression)
{


    /*
    (6.5.5) multiplicative-expression:
    cast-expression                                // identifier  constant  char-literal  (  _Generic ++	  -- 	 & * + - ~ ! 		 sizeof 		 sizeof		 alignof
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */

    struct TExpression* pExpressionLeft;
    CastExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_PERCENT_SIGN:
        case TK_SOLIDUS:
        case TK_ASTERISK:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();

            GetPosition(ctx, &pBinaryExpression->Position);
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;

            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            CastExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_PERCENT_SIGN:
        case TK_SOLIDUS:
        case TK_ASTERISK:
        {
            struct TBinaryExpression* pBinaryExpression =
                TBinaryExpression_Create();

            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            MultiplicativeExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void AdditiveExpression(struct Parser* ctx, struct TExpression** ppExpression)
{

    /*
    (6.5.6) additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */

    struct TExpression* pExpressionLeft;
    MultiplicativeExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_PLUS_SIGN:
        case TK_HYPHEN_MINUS:
        {
            struct TBinaryExpression* pBinaryExpression =
                TBinaryExpression_Create();
            GetPosition(ctx, &pBinaryExpression->Position);
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            MultiplicativeExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_PLUS_SIGN:
        case TK_HYPHEN_MINUS:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            AdditiveExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

}

void ShiftExpression(struct Parser* ctx, struct TExpression** ppExpression)
{

    /*(6.5.7) shift-expression:
    additive-expression
    shift-expression << additive-expression
    shift-expression >> additive-expression
    */


    struct TExpression* pExpressionLeft;
    AdditiveExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_GREATERGREATER:
        case TK_LESSLESS:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            AdditiveExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_GREATERGREATER:
        case TK_LESSLESS:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            ShiftExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void RelationalExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*
    (6.5.8) relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
    */

    struct TExpression* pExpressionLeft;
    ShiftExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_LESS_THAN_SIGN:
        case TK_GREATER_THAN_SIGN:
        case TK_GREATEREQUAL:
        case TK_LESSEQUAL:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            ShiftExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_LESS_THAN_SIGN:
        case TK_GREATER_THAN_SIGN:
        case TK_GREATEREQUAL:
        case TK_LESSEQUAL:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            RelationalExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

}

void EqualityExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.9) equality-expression:
    relational-expression
    equality-expression == relational-expression
    equality-expression != relational-expression
    */

    struct TExpression* pExpressionLeft;
    RelationalExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_EQUALEQUAL:
        case TK_NOTEQUAL:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            RelationalExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_EQUALEQUAL:
        case TK_NOTEQUAL:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            EqualityExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void AndExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.10) AND-expression:
    equality-expression
    AND-expression & equality-expression
    */

    struct TExpression* pExpressionLeft;
    EqualityExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_AMPERSAND:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            EqualityExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_AMPERSAND:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            AndExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void ExclusiveOrExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*
    (6.5.11) exclusive-OR-expression:
    AND-expression
    exclusive-OR-expression ^ AND-expression
    */

    struct TExpression* pExpressionLeft;
    AndExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_CIRCUMFLEX_ACCENT:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            AndExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_CIRCUMFLEX_ACCENT:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);


            struct TExpression* pExpressionRight;
            ExclusiveOrExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

}

void InclusiveOrExpression(struct Parser* ctx, struct TExpression** ppExpression)
{

    /*
    (6.5.12) inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */

    struct TExpression* pExpressionLeft;
    ExclusiveOrExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_VERTICAL_LINE:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            ExclusiveOrExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_VERTICAL_LINE:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            InclusiveOrExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void LogicalAndExpression(struct Parser* ctx, struct TExpression** ppExpression)
{

    /*
    (6.5.13) logical-AND-expression:
    inclusive-OR-expression
    logical-AND-expression && inclusive-OR-expression
    */

    struct TExpression* pExpressionLeft;
    InclusiveOrExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_ANDAND:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;
            GetPosition(ctx, &pBinaryExpression->Position);


            Parser_Match(ctx, &pBinaryExpression->ClueList0);


            struct TExpression* pExpressionRight;
            InclusiveOrExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_ANDAND:
        {
            struct TBinaryExpression* pBinaryExpression =
                TBinaryExpression_Create();

            GetPosition(ctx, &pBinaryExpression->Position);
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            LogicalAndExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}

void LogicalOrExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.14) logical-OR-expression:
    logical-AND-expression
    logical-OR-expression || logical-AND-expression
    */


    struct TExpression* pExpressionLeft;
    LogicalAndExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
        case TK_OROR:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            GetPosition(ctx, &pBinaryExpression->Position);
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            LogicalAndExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_OROR:
        {
            struct TBinaryExpression* pBinaryExpression = TBinaryExpression_Create();
            GetPosition(ctx, &pBinaryExpression->Position);
            pBinaryExpression->token = token;
            pBinaryExpression->pExpressionLeft = *ppExpression;


            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            struct TExpression* pExpressionRight;
            LogicalOrExpression(ctx, &pExpressionRight);
            pBinaryExpression->pExpressionRight = pExpressionRight;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;
        default:
            //assert(false);
            break;
    }
}


void ConditionalExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.15) conditional-expression:
    logical-OR-expression
    logical-OR-expression ? expression : conditional-expression
    */
    struct TExpression* pLogicalOrExpressionLeft;
    LogicalOrExpression(ctx, &pLogicalOrExpressionLeft);
    *ppExpression = pLogicalOrExpressionLeft;

    if (Parser_CurrentToken(ctx) == TK_QUESTION_MARK)
    {
        struct TTernaryExpression* pTernaryExpression =
            TTernaryExpression_Create();

        Parser_Match(ctx, &pTernaryExpression->ClueList0);

        struct TExpression* pTExpression;
        Expression0(ctx, &pTExpression);

        Parser_MatchToken(ctx, TK_COLON, &pTernaryExpression->ClueList1);

        struct TExpression* pConditionalExpressionRight;
        ConditionalExpression(ctx, &pConditionalExpressionRight);


        pTernaryExpression->token = TK_QUESTION_MARK;
        pTernaryExpression->pExpressionLeft = pLogicalOrExpressionLeft;
        pTernaryExpression->pExpressionMiddle = pTExpression;
        pTernaryExpression->pExpressionRight = pConditionalExpressionRight;
        *ppExpression = (struct TExpression*)pTernaryExpression;
    }


}

void AssignmentExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    /*(6.5.16) assignment-expression:
    conditional-expression
    unary-expression assignment-operator assignment-expression

    (6.5.16) assignment-operator: one of
    = *= /= %= += -= <<= >>= &= ^= |=
    */


    //Não sei se eh  conditional-expression ou
    //unary-expression
    //Mas a conditional-expression faz tambem a
    //unary-expression
    struct TExpression* pConditionalExpressionLeft;
    ConditionalExpression(ctx, &pConditionalExpressionLeft);
    *ppExpression = pConditionalExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {

        case TK_EQUALS_SIGN:
        case TK_MULTIEQUAL:
        case TK_DIVEQUAL:
        case TK_PERCENT_EQUAL:
        case TK_PLUSEQUAL:
        case TK_MINUS_EQUAL:
        case TK_LESSLESSEQUAL:
        case TK_GREATERGREATEREQUAL:
        case TK_ANDEQUAL:
        case TK_CARETEQUAL:
        case TK_OREQUAL:
        {
            struct TBinaryExpression* pBinaryExpression =
                TBinaryExpression_Create();

            Parser_Match(ctx, &pBinaryExpression->ClueList0);

            //Significa que o anterior deve ser do tipo  unary-expression
            //embora tenhamos feito o parser de conditional-expression
            //se nao for é erro.
            struct TExpression* pAssignmentExpressionRight;
            AssignmentExpression(ctx, &pAssignmentExpressionRight);

            GetPosition(ctx, &pBinaryExpression->Position);

            pBinaryExpression->pExpressionLeft = *ppExpression;
            pBinaryExpression->pExpressionRight = pAssignmentExpressionRight;
            pBinaryExpression->token = token;
            *ppExpression = (struct TExpression*)pBinaryExpression;
        }
        break;

        default:
            //É apenas conditional-expression
            break;
    }
}

void Expression0(struct Parser* ctx, struct TExpression** ppExpression)
{
    *ppExpression = NULL; //out
                          /*
                          (6.5.17) expression:
                          assignment-expression
                          expression , assignment-expression
                          */
    struct TExpression* pAssignmentExpressionLeft;
    AssignmentExpression(ctx, &pAssignmentExpressionLeft);
    *ppExpression = pAssignmentExpressionLeft;

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        struct TExpression* pAssignmentExpressionRight;
        Parser_Match(ctx, NULL);
        Expression0(ctx, &pAssignmentExpressionRight);

        struct TBinaryExpression* pBinaryExpression =
            TBinaryExpression_Create();
        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->pExpressionLeft = *ppExpression;
        pBinaryExpression->pExpressionRight = pAssignmentExpressionRight;
        pBinaryExpression->token = TK_COMMA;

        *ppExpression = (struct TExpression*)pBinaryExpression;
    }
}

void ConstantExpression(struct Parser* ctx, struct TExpression** ppExpression)
{
    *ppExpression = NULL; //out

                          /*
                          (6.6) constant-expression:
                          conditional-expression
                          */
    ConditionalExpression(ctx, ppExpression);
}


///////////////////////////////////////////////////////////////////////////////


void Designator(struct Parser* ctx, struct TDesignator* pDesignator);
void Designator_List(struct Parser* ctx, struct TDesignatorList* pDesignatorList);
void Designation(struct Parser* ctx, struct TDesignatorList* pDesignatorList);
void Initializer_List(struct Parser* ctx, struct TInitializerList* pInitializerList);
//void Assignment_Expression(struct Parser* ctx, struct TExpressionStatement* pExpressionStatement, enum Tokens endToken1, enum Tokens endToken2);
bool Statement(struct Parser* ctx, struct TStatement** ppStatement);
//void Constant_Expression(struct Parser* ctx, enum Tokens endToken1, enum Tokens endToken2, ScannerItemStack* outStack);
void Compound_Statement(struct Parser* ctx, struct TStatement** ppStatement);
//void Expression(struct Parser* ctx, struct TExpression** pExpression, enum Tokens endToken1, enum Tokens endToken2);
void Parameter_Declaration(struct Parser* ctx, struct TParameter* pParameterDeclaration);
bool Declaration(struct Parser* ctx, struct TAnyDeclaration** ppDeclaration);
void Type_Qualifier_List(struct Parser* ctx, struct TTypeQualifierList* pQualifiers);
void Declaration_Specifiers(struct Parser* ctx, struct TDeclarationSpecifiers* pDeclarationSpecifiers);

void Type_Specifier(struct Parser* ctx, struct TTypeSpecifier** ppTypeSpecifier);
bool Type_Qualifier(struct Parser* ctx, struct TTypeQualifier* pQualifier);
void Initializer(struct Parser* ctx,
                 struct TInitializer** ppInitializer,
                 enum Tokens endToken1,
                 enum Tokens endToken2);


void Expression_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    expression-statement:
    expressionopt;
    */
    struct TExpressionStatement* pExpression = TExpressionStatement_Create();
    *ppStatement = (struct TStatement*)pExpression;
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token != TK_SEMICOLON)
    {
        Expression0(ctx, &pExpression->pExpression);
    }

    Parser_MatchToken(ctx, TK_SEMICOLON, &pExpression->ClueList0);
}

void Selection_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    selection-statement:
    if ( expression ) statement
    if ( expression ) statement else statement
    switch ( expression ) statement
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_IF:
        {
            struct TIfStatement* pIfStatement = TIfStatement_Create();
            *ppStatement = (struct TStatement*)pIfStatement;

            Parser_Match(ctx, &pIfStatement->ClueList0);

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pIfStatement->ClueList1);


            Expression0(ctx, &pIfStatement->pConditionExpression);

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pIfStatement->ClueList2);

            Statement(ctx, &pIfStatement->pStatement);
            token = Parser_CurrentToken(ctx);

            if (token == TK_ELSE)
            {
                Parser_Match(ctx, &pIfStatement->ClueList3);
                Statement(ctx, &pIfStatement->pElseStatement);
            }
        }
        break;

        case TK_SWITCH:
        {
            struct TSwitchStatement* pSelectionStatement = TSwitchStatement_Create();
            *ppStatement = (struct TStatement*)pSelectionStatement;


            Parser_Match(ctx, &pSelectionStatement->ClueList0);


            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pSelectionStatement->ClueList1);

            Expression0(ctx, &pSelectionStatement->pConditionExpression);


            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pSelectionStatement->ClueList2);

            Statement(ctx, &pSelectionStatement->pExpression);
        }
        break;

        default:
            //assert(false);
            break;
    }
}

void Jump_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    jump-statement:
    goto identifier ;
    continue ;
    break ;
    return expressionopt ;
    */
    //jump-statement
    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_GOTO:
        {
            struct TJumpStatement* pJumpStatement = TJumpStatement_Create();
            pJumpStatement->token = token;
            *ppStatement = (struct TStatement*)pJumpStatement;

            Parser_Match(ctx, &pJumpStatement->ClueList0);
            PTR_STRING_REPLACE(pJumpStatement->Identifier, Lexeme(ctx));
            Parser_MatchToken(ctx, TK_IDENTIFIER, &pJumpStatement->ClueList1);
            Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
        }
        break;

        case TK_CONTINUE:
        {
            struct TJumpStatement* pJumpStatement = TJumpStatement_Create();
            pJumpStatement->token = token;
            *ppStatement = (struct TStatement*)pJumpStatement;
            Parser_Match(ctx, &pJumpStatement->ClueList0);

            Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
        }
        break;

        case TK_BREAK:
        {
            struct TJumpStatement* pJumpStatement = TJumpStatement_Create();
            pJumpStatement->token = token;
            *ppStatement = (struct TStatement*)pJumpStatement;

            Parser_Match(ctx, &pJumpStatement->ClueList0);
            Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
        }
        break;

        case TK_RETURN:
        {
            struct TJumpStatement* pJumpStatement = TJumpStatement_Create();
            pJumpStatement->token = token;
            *ppStatement = (struct TStatement*)pJumpStatement;

            token = Parser_Match(ctx, &pJumpStatement->ClueList0);

            if (token != TK_SEMICOLON)
            {
                Expression0(ctx, &pJumpStatement->pExpression);
            }

            Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
        }
        break;

        default:
            //assert(false);
            break;
    }
}

void Iteration_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    iteration-statement:
    while ( expression ) statement
    do statement while ( expression ) ;
    for ( expressionopt ; expressionopt ; expressionopt ) statement
    for ( declaration expressionopt ; expressionopt ) statement
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_WHILE:
        {
            struct TWhileStatement* pWhileStatement = TWhileStatement_Create();
            *ppStatement = (struct TStatement*)pWhileStatement;

            Parser_Match(ctx, &pWhileStatement->ClueList0);

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pWhileStatement->ClueList1);

            Expression0(ctx, &pWhileStatement->pExpression);

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pWhileStatement->ClueList2);

            Statement(ctx, &pWhileStatement->pStatement);
        }
        break;

        case TK_DO:
        {
            struct TDoStatement* pDoStatement = TDoStatement_Create();
            *ppStatement = (struct TStatement*)pDoStatement;

            Parser_Match(ctx, &pDoStatement->ClueList0); //do

            Statement(ctx, &pDoStatement->pStatement);

            Parser_MatchToken(ctx, TK_WHILE, &pDoStatement->ClueList1); //while

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pDoStatement->ClueList2); //(

            Expression0(ctx, &pDoStatement->pExpression);

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pDoStatement->ClueList3); //)

            Parser_MatchToken(ctx, TK_SEMICOLON, &pDoStatement->ClueList4); //;
        }
        break;

        case TK_FOR:
        {
            struct TForStatement* pIterationStatement = TForStatement_Create();
            *ppStatement = (struct TStatement*)pIterationStatement;

            Parser_Match(ctx, &pIterationStatement->ClueList0);

            token = Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pIterationStatement->ClueList1);

            //primeira expressao do for
            if (token != TK_SEMICOLON)
            {
                //
                //for (expressionopt; expressionopt; expressionopt) statement
                //for (declaration expressionopt; expressionopt) statement

                bool bHasDeclaration = Declaration(ctx, &pIterationStatement->pInitDeclarationOpt);

                if (bHasDeclaration)
                {
                    token = Parser_CurrentToken(ctx);

                    if (token != TK_SEMICOLON)
                    {
                        //Esta eh a 2 expressao do for, a declaracao ja comeu 1
                        Expression0(ctx, &pIterationStatement->pExpression2);

                        Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList2);
                    }
                    else
                    {
                        //segunda expressao vazia                        
                        Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList2);
                    }
                }

                else
                {
                    token = Parser_CurrentToken(ctx);

                    if (token != TK_SEMICOLON)
                    {
                        //primeira expressao do for
                        Expression0(ctx, &pIterationStatement->pExpression1);


                        Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList2);
                    }

                    token = Parser_CurrentToken(ctx);

                    if (token != TK_SEMICOLON)
                    {
                        //segunda expressao do for
                        Expression0(ctx, &pIterationStatement->pExpression2);

                        Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList3);
                    }

                    else
                    {
                        //segunda expressao vazia                        
                        Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList3);
                    }

                }
            }

            else
            {
                //primeira expressao do for vazia

                Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList2);

                token = Parser_CurrentToken(ctx);

                if (token != TK_SEMICOLON)
                {
                    //Esta eh a 2 expressao do for, a declaracao ja comeu 1
                    Expression0(ctx, &pIterationStatement->pExpression2);


                    Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList3);
                }

                else
                {
                    //segunda expressao do for vazia tb
                    Parser_MatchToken(ctx, TK_SEMICOLON, &pIterationStatement->ClueList3);
                }
            }

            token = Parser_CurrentToken(ctx);

            //terceira expressao do for
            if (token != TK_RIGHT_PARENTHESIS)
            {
                Expression0(ctx, &pIterationStatement->pExpression3);
            }

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pIterationStatement->ClueList4);

            Statement(ctx, &pIterationStatement->pStatement);
        }
        break;

        default:
            //assert(false);
            break;
    }
}


void Labeled_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    labeled-statement:
    identifier : statement (ver Labeled_StatementLabel)
    case constant-expression : statement
    default : statement
    */
    struct TLabeledStatement* pLabeledStatement = TLabeledStatement_Create();
    *ppStatement = (struct TStatement*)pLabeledStatement;
    enum Tokens token = Parser_CurrentToken(ctx);
    pLabeledStatement->token = token;

    if (token == TK_IDENTIFIER)
    {
        //aqui nao eh um tipo
        PTR_STRING_REPLACE(pLabeledStatement->Identifier, Lexeme(ctx));

        Parser_Match(ctx, &pLabeledStatement->ClueList0);

        Parser_MatchToken(ctx, TK_COLON, &pLabeledStatement->ClueList1);

        Statement(ctx, &pLabeledStatement->pStatementOpt);
    }

    else if (token == TK_CASE)
    {

        Parser_Match(ctx, &pLabeledStatement->ClueList0);

        ConstantExpression(ctx, &pLabeledStatement->pExpression);


        Parser_MatchToken(ctx, TK_COLON, &pLabeledStatement->ClueList1);

        Statement(ctx, &pLabeledStatement->pStatementOpt);
    }

    else if (token == TK_DEFAULT)
    {

        Parser_Match(ctx, &pLabeledStatement->ClueList0);

        Parser_MatchToken(ctx, TK_COLON, &pLabeledStatement->ClueList1);

        Statement(ctx, &pLabeledStatement->pStatementOpt);
    }
}

void Asm_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    __asm assembly-instruction ;opt
    __asm { assembly-instruction-list };opt
    */
    struct TAsmStatement* pAsmStatement = TAsmStatement_Create();
    *ppStatement = (struct TStatement*)pAsmStatement;
    Parser_MatchToken(ctx, TK__ASM, NULL);
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_LEFT_CURLY_BRACKET)
    {
        Parser_Match(ctx, NULL);

        for (; ;)
        {
            if (ErrorOrEof(ctx))
            {
                break;
            }

            token = Parser_CurrentToken(ctx);

            if (token == TK_RIGHT_CURLY_BRACKET)
            {
                Parser_Match(ctx, NULL);
                break;
            }

            Parser_Match(ctx, NULL);
        }
    }

    else
    {
        //sem ;
        //    __asm int 0x2c
        //chato
        token = Parser_CurrentToken(ctx);

        for (; ;)
        {
            if (ErrorOrEof(ctx))
            {
                break;
            }

            token = Parser_CurrentToken(ctx);

            if (token == TK_RIGHT_CURLY_BRACKET)
            {
                //__asm mov al, 2   __asm mov dx, 0xD007   __asm out dx, al
                //chute na verdade..
                //dificil saber aonde termina
                //https://msdn.microsoft.com/en-us/library/45yd4tzz.aspx
                break;
            }

            if (token == TK_SEMICOLON)
            {
                break;
            }

            Parser_Match(ctx, NULL);
        }
    }

    //opcional
    token = Parser_CurrentToken(ctx);

    if (token == TK_SEMICOLON)
    {
        Parser_Match(ctx, NULL);
    }
}

bool Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    //assert(*ppStatement == NULL);

    if (Parser_HasError(ctx))
    {
        return false;
    }

    bool bResult = false;
    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);

    switch (token)
    {
        case TK__ASM:
            bResult = true;
            Asm_Statement(ctx, ppStatement);
            break;

        case TK_LEFT_CURLY_BRACKET:
        {
            bResult = true;
            Compound_Statement(ctx, ppStatement);
        }
        break;

        case TK_CASE:
        case TK_DEFAULT:
            bResult = true;
            Labeled_Statement(ctx, ppStatement);
            break;

        case TK_SWITCH:
            bResult = true;
            Selection_Statement(ctx, ppStatement);
            break;

        case TK_IF:
            bResult = true;
            Selection_Statement(ctx, ppStatement);
            break;

            //case TK_ELSE:
            ////assert(false);
            //Ele tem que estar fazendo os statement do IF!
            //bResult = true;
            //Parser_Match(ctx, NULL); //else
            //poderia retornar uma coisa so  p dizer q eh else
            //Statement(ctx, obj);
            //break;

            //iteration-statement
        case TK_WHILE:
        case TK_FOR:
        case TK_DO:
            bResult = true;
            Iteration_Statement(ctx, ppStatement);
            break;

            //jump-statement
        case TK_GOTO:
        case TK_CONTINUE:
        case TK_BREAK:
        case TK_RETURN:
            bResult = true;
            Jump_Statement(ctx, ppStatement);
            break;

            //lista de first para expressões
            //expression-statement
        case TK_LEFT_SQUARE_BRACKET://lamda todo isprimeiryfirst
        case TK_LEFT_PARENTHESIS:
        case TK_SEMICOLON:

        case TK_DECIMAL_INTEGER:
        case TK_FLOAT_NUMBER:
        case TK_STRING_LITERAL:


            //unary
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:

        case TK_SIZEOF:

            //unary-operator
        case TK_AMPERSAND:
        case TK_ASTERISK:
        case TK_PLUS_SIGN:
        case TK_HYPHEN_MINUS:
        case TK_TILDE:
        case TK_EXCLAMATION_MARK:
#ifdef LANGUAGE_EXTENSIONS
            //unary-operator-extension
        case TK_ANDAND: //&&
#endif
            bResult = true;
            Expression_Statement(ctx, ppStatement);
            break;

        case TK_IDENTIFIER:

            if (IsTypeName(ctx, TK_IDENTIFIER, lexeme))
            {
                //É uma declaracao
            }
            else
            {
                if (Parser_LookAheadToken(ctx) == TK_COLON)
                {
                    //era um label..
                    Labeled_Statement(ctx, ppStatement);
                }

                else
                {
                    Expression_Statement(ctx, ppStatement);
                }

                bResult = true;
            }

            break;

        case TK_INLINE:
        case TK__INLINE: //microscoft
        case TK__NORETURN:
        case TK__ALIGNAS:

            //type-qualifier
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
        case TK__ATOMIC:


        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK__THREAD_LOCAL:
        case TK_AUTO:
        case TK_REGISTER:
        case TK_VOID:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_LONG:
            //microsoft
        case TK__INT8:
        case TK__INT16:
        case TK__INT32:
        case TK__INT64:
        case TK__WCHAR_T:
            /////////
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK__BOOL:
        case TK__COMPLEX:

        case TK_STRUCT:
        case TK_UNION:
        case TK_ENUM:
            bResult = false;
            break;

        default:

            SetError(ctx, "unexpected error");
            //bResult = true;
            //SetType(pStatement, "expression-statement");
            //Expression_Statement(ctx, pStatement);
            break;
    }

    return bResult;
}

void Block_Item(struct Parser* ctx, struct TBlockItem** ppBlockItem)
{
    /*
    block-item:
    declaration
    statement
    */
    *ppBlockItem = NULL;
    struct TStatement* pStatement = NULL;

    if (Statement(ctx, &pStatement))
    {
        *ppBlockItem = (struct TBlockItem*)pStatement;
        //assert(*ppBlockItem != NULL);
    }

    else
    {


        struct TAnyDeclaration* pDeclaration;
        Declaration(ctx, &pDeclaration);
        *ppBlockItem = (struct TBlockItem*)pDeclaration;
        //assert(*ppBlockItem != NULL);
    }
}

void Block_Item_List(struct Parser* ctx, struct TBlockItemList* pBlockItemList)
{
    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        struct TBlockItem* pBlockItem = NULL;
        Block_Item(ctx, &pBlockItem);
        TBlockItemList_PushBack(pBlockItemList, pBlockItem);
        enum Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_RIGHT_CURLY_BRACKET)
        {
            //terminou
            break;
        }

        if (ErrorOrEof(ctx))
            break;
    }
}

void Compound_Statement(struct Parser* ctx, struct TStatement** ppStatement)
{
    /*
    compound-statement:
    { block-item-listopt }
    */
    struct TCompoundStatement* pCompoundStatement = TCompoundStatement_Create();
    *ppStatement = (struct TStatement*)pCompoundStatement;

    struct SymbolMap BlockScope = SYMBOLMAP_INIT;

    BlockScope.pPrevious = ctx->pCurrentScope;
    ctx->pCurrentScope = &BlockScope;

    Parser_MatchToken(ctx, TK_LEFT_CURLY_BRACKET, &pCompoundStatement->ClueList0);

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token != TK_RIGHT_CURLY_BRACKET)
    {
        Block_Item_List(ctx, &pCompoundStatement->BlockItemList);
    }

    Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET, &pCompoundStatement->ClueList1);

    //SymbolMap_Print(ctx->pCurrentScope);

    ctx->pCurrentScope = BlockScope.pPrevious;
    SymbolMap_Destroy(&BlockScope);
}

void Struct_Or_Union(struct Parser* ctx,
                     struct TStructUnionSpecifier* pStructUnionSpecifier)
{
    /*
    struct-or-union:
    struct
    union
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_STRUCT:
            pStructUnionSpecifier->Token = token;
            Parser_Match(ctx, &pStructUnionSpecifier->ClueList0);
            break;

        case TK_UNION:
            pStructUnionSpecifier->Token = token;
            Parser_Match(ctx, &pStructUnionSpecifier->ClueList0);
            break;

        default:
            //assert(false);
            break;
    }
}

void Static_Assert_Declaration(struct Parser* ctx, struct TStaticAssertDeclaration* pStaticAssertDeclaration)
{
    /*
    static_assert-declaration:
    _Static_assert ( constant-expression , char-literal ) ;
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__STATIC_ASSERT)
    {
        Parser_Match(ctx, &pStaticAssertDeclaration->ClueList0);

        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pStaticAssertDeclaration->ClueList1);

        ConstantExpression(ctx,
                           &pStaticAssertDeclaration->pConstantExpression);

        Parser_MatchToken(ctx, TK_COMMA, &pStaticAssertDeclaration->ClueList2);

        PTR_STRING_REPLACE(pStaticAssertDeclaration->Text, Lexeme(ctx));
        Parser_MatchToken(ctx, TK_STRING_LITERAL, &pStaticAssertDeclaration->ClueList3);

        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pStaticAssertDeclaration->ClueList4);

        Parser_MatchToken(ctx, TK_SEMICOLON, &pStaticAssertDeclaration->ClueList5);
    }
}

void Specifier_Qualifier_List(struct Parser* ctx, struct TSpecifierQualifierList* pSpecifierQualifierList)
{
    /*specifier-qualifier-list:
    type-specifier specifier-qualifier-listopt
    type-qualifier specifier-qualifier-listopt
    */
    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);
    if (TTypeSpecifier_IsFirst(ctx, token, lexeme))
    {
        if (TSpecifierQualifierList_CanAdd(pSpecifierQualifierList, token, lexeme))
        {
            struct TTypeSpecifier* pTypeSpecifier = NULL;
            Type_Specifier(ctx, &pTypeSpecifier);
            if (pTypeSpecifier != NULL)
            {
                //ATENCAO
                TSpecifierQualifierList_PushBack(pSpecifierQualifierList, (struct TSpecifierQualifier*)pTypeSpecifier);
            }
        }
        else
        {
            SetError(ctx, "invalid specifier-qualifier-list");
        }

    }
    else if (TTypeQualifier_IsFirst(token))
    {
        struct TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
        Type_Qualifier(ctx, pTypeQualifier);
        TSpecifierQualifierList_PushBack(pSpecifierQualifierList, TTypeQualifier_As_TSpecifierQualifier(pTypeQualifier));
    }
    else
    {
        SetError(ctx, "internal error 01 %s", TokenToString(token));
    }


    token = Parser_CurrentToken(ctx);
    lexeme = Lexeme(ctx);
    if (TTypeQualifier_IsFirst(token))
    {
        Specifier_Qualifier_List(ctx, pSpecifierQualifierList);
    }
    else if (TTypeSpecifier_IsFirst(ctx, token, lexeme))
    {
        if (TSpecifierQualifierList_CanAdd(pSpecifierQualifierList, token, lexeme))
        {
            /*
            typedef int X;
            void F(int X ); //X vai ser variavel e nao tipo
            */

            Specifier_Qualifier_List(ctx, pSpecifierQualifierList);
        }
    }

}


void Struct_Declarator(struct Parser* ctx,
                       TStructDeclarator** ppTDeclarator2)
{
    /**
    struct-declarator:
    declarator
    declaratoropt : constant-expression
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COLON)
    {
        //AST TODO
        ////TNodeClueList_MoveToEnd(&ppTDeclarator2->ClueList, &ctx->Scanner.ClueList);

        Parser_Match(ctx, NULL);// &ppTDeclarator2->ClueList);
        struct TExpression* p = NULL;
        ConstantExpression(ctx, &p);
        TExpression_Delete(p);
    }

    else
    {
        struct TInitDeclarator* pInitDeclarator =
            TInitDeclarator_Create();

        *ppTDeclarator2 = pInitDeclarator;

        Declarator(ctx, false, &pInitDeclarator->pDeclarator);

        token = Parser_CurrentToken(ctx);



        if (token == TK_COLON)
        {
            Parser_Match(ctx, &pInitDeclarator->ClueList0);

            struct TExpression* p = NULL;
            ConstantExpression(ctx, &p);
            TExpression_Delete(p);
        }
#ifdef LANGUAGE_EXTENSIONS


        if (token == TK_EQUALS_SIGN)
        {
            Parser_Match(ctx, &pInitDeclarator->ClueList1); //_defval ou =
            Initializer(ctx, &pInitDeclarator->pInitializer, TK_SEMICOLON, TK_SEMICOLON);

            token = Parser_CurrentToken(ctx);

        }

#endif
    }
}

void Struct_Declarator_List(struct Parser* ctx,

                            struct TStructDeclaratorList* pStructDeclarationList)
{
    /*
    struct-declarator-list:
    struct-declarator
    struct-declarator-list , struct-declarator
    */
    TStructDeclarator* pTDeclarator2 = NULL;// TDeclarator_Create();

    Struct_Declarator(ctx, &pTDeclarator2);
    TStructDeclaratorList_Add(pStructDeclarationList, pTDeclarator2);

    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        enum Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_COMMA)
        {
            //Tem mais
            Parser_Match(ctx, &pTDeclarator2->ClueList0);
            //ANNOTATED AQUI TEM O COMENTARIO /*@= 1*/

            Struct_Declarator_List(ctx, pStructDeclarationList);
        }
        else if (token == TK_SEMICOLON)
        {
            //em ctx cluelist
            //ANNOTATED AQUI TEM O COMENTARIO /*@= 1*/
            break;
        }
        else
        {
            if (token == TK_RIGHT_CURLY_BRACKET)
            {
                SetError(ctx, "syntax error: missing ';' before '}'");
            }
            else
            {
                SetError(ctx, "syntax error: expected ',' or ';'");
            }

            break;
        }
    }
}

void Struct_Declaration(struct Parser* ctx,
                        struct TAnyStructDeclaration** ppStructDeclaration)
{
    /**
    struct-declaration:
    specifier-qualifier-list struct-declarator-listopt ;
    static_assert-declaration
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token != TK__STATIC_ASSERT)
    {
        struct TStructDeclaration* pStructDeclarationBase = TStructDeclaration_Create();
        *ppStructDeclaration = (struct TAnyStructDeclaration*)pStructDeclarationBase;
        Specifier_Qualifier_List(ctx,
                                 &pStructDeclarationBase->SpecifierQualifierList);
        token = Parser_CurrentToken(ctx);

        if (token != TK_SEMICOLON)
        {
            Struct_Declarator_List(ctx,
                                   &pStructDeclarationBase->DeclaratorList);

            Parser_MatchToken(ctx, TK_SEMICOLON, &pStructDeclarationBase->ClueList1);
            //TODO AQUI TEM O COMENTARIO /*@= 1*/
        }
        else
        {
            Parser_MatchToken(ctx, TK_SEMICOLON, &pStructDeclarationBase->ClueList1);
        }
    }

    else
    {
        struct TStaticAssertDeclaration* pStaticAssertDeclaration = TStaticAssertDeclaration_Create();
        *ppStructDeclaration = (struct TAnyStructDeclaration*)pStaticAssertDeclaration;
        Static_Assert_Declaration(ctx, pStaticAssertDeclaration);
    }
}

void Struct_Declaration_List(struct Parser* ctx,
                             struct TStructDeclarationList* pStructDeclarationList)
{
    /*
    struct-declaration-list:
    struct-declaration
    struct-declaration-list struct-declaration
    */
    if (ErrorOrEof(ctx))
    {
        return;
    }

    struct TAnyStructDeclaration* pStructDeclaration = NULL;
    Struct_Declaration(ctx, &pStructDeclaration);
    TStructDeclarationList_PushBack(pStructDeclarationList, pStructDeclaration);


    enum Tokens token = Parser_CurrentToken(ctx);
    if (token != TK_RIGHT_CURLY_BRACKET)
    {
        //Tem mais?
        Struct_Declaration_List(ctx, pStructDeclarationList);
    }
}

void UnionSetItem(struct Parser* ctx, struct TUnionSet* p)
{
    /*
    _union-set-item:
    struct Identifier
    union Identifier
    Identifier
    */
    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);

    struct TUnionSetItem* pItem = TUnionSetItem_Create();

    if (token == TK_IDENTIFIER)
    {
        PTR_STRING_REPLACE(pItem->Name, lexeme);
        Parser_Match(ctx, &pItem->ClueList1);
        TUnionSet_PushBack(p, pItem);
    }
    else if (token == TK_STRUCT ||
             token == TK_UNION)
    {
        Parser_Match(ctx, &pItem->ClueList0);

        PTR_STRING_REPLACE(pItem->Name, lexeme);
        Parser_MatchToken(ctx, TK_IDENTIFIER, &pItem->ClueList1);
        TUnionSet_PushBack(p, pItem);
    }
    else
    {
        SetError(ctx, "invalid token for union set");
    }
}

void UnionSetList(struct Parser* ctx, struct TUnionSet* p)
{
    /*
    _union-set-list:
    _union-set-item
    _union-set-item | _union-set-list
    */
    enum Tokens token = Parser_CurrentToken(ctx);
    UnionSetItem(ctx, p);

    token = Parser_CurrentToken(ctx);
    if (token == TK_VERTICAL_LINE)
    {
        p->pTail->TokenFollow = token;
        Parser_Match(ctx, &p->pTail->ClueList2);
        UnionSetList(ctx, p);
    }
}

void UnionSet(struct Parser* ctx, struct TUnionSet* pUnionSet)
{
    /*
    _union-set:
    < _union-set-list >
    */

    enum Tokens token = Parser_CurrentToken(ctx);
    //const char* lexeme = Lexeme(ctx);


    if (token == TK_LESS_THAN_SIGN)
    {
        Parser_Match(ctx, &pUnionSet->ClueList0);


        UnionSetList(ctx, pUnionSet);

        Parser_MatchToken(ctx, TK_GREATER_THAN_SIGN,
                          &pUnionSet->ClueList1);
    }


}

void Struct_Or_Union_Specifier(struct Parser* ctx,
                               struct TStructUnionSpecifier* pStructUnionSpecifier)
{
    /*
    struct-or-union-specifier:
    struct-or-union identifieropt { struct-declaration-list }
    struct-or-union identifier
    */

    /*
    struct-or-union-specifier:
    struct-or-union )opt identifieropt { struct-declaration-list }
    struct-or-union )opt identifier
    */

    //aqui teria que ativar o flag
    Struct_Or_Union(ctx, pStructUnionSpecifier);//TODO


    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);




    if (token == TK_LESS_THAN_SIGN)
    {
        UnionSet(ctx, &pStructUnionSpecifier->UnionSet);
        token = Parser_CurrentToken(ctx);
    }


    token = Parser_CurrentToken(ctx);
    lexeme = Lexeme(ctx);

    if (token == TK_IDENTIFIER)
    {
        //ANNOTATED AQUI TEM O COMENTARIO /*Box | Circle*/ antes nome da struct
        PTR_STRING_REPLACE(pStructUnionSpecifier->Tag, lexeme);
        Parser_Match(ctx, &pStructUnionSpecifier->ClueList1);
    }

    if (pStructUnionSpecifier->Tag != NULL)
    {
        SymbolMap_SetAt(ctx->pCurrentScope, pStructUnionSpecifier->Tag, (struct TTypePointer*)pStructUnionSpecifier);
    }

    token = Parser_CurrentToken(ctx);
    if (token == TK_LEFT_CURLY_BRACKET)
    {
        //ANNOTATED AQUI TEM O COMENTARIO /*Box | Circle*/ antes do {
        Parser_Match(ctx, &pStructUnionSpecifier->ClueList2);

        Struct_Declaration_List(ctx,
                                &pStructUnionSpecifier->StructDeclarationList);

        Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                          &pStructUnionSpecifier->ClueList3);
    }
    else
    {
        //struct X *
        // SetError2(ctx, "expected name or {", "");
    }
}

void Enumeration_Constant(struct Parser* ctx,
                          struct TEnumerator* pEnumerator2)
{
    const char* lexeme = Lexeme(ctx);
    PTR_STRING_REPLACE(pEnumerator2->Name, lexeme);
    Parser_MatchToken(ctx, TK_IDENTIFIER, &pEnumerator2->ClueList0);
}

bool EnumeratorC(struct Parser* ctx, struct TEnumerator* pEnumerator2)
{
    /*
    enumerator:
    enumeration-constant
    enumeration-constant = constant-expression
    */
    bool bValueAssigned = false;
    Enumeration_Constant(ctx, pEnumerator2);


    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_EQUALS_SIGN)
    {
        Parser_Match(ctx, &pEnumerator2->ClueList1);
        ConstantExpression(ctx, &pEnumerator2->pConstantExpression);
        bValueAssigned = true;
    }

    return bValueAssigned;
}

void Enumerator_List(struct Parser* ctx,
                     struct TEnumeratorList* pEnumeratorList2)
{
    if (ErrorOrEof(ctx))
    {
        return;
    }

    /*
    enumerator-list:
    enumerator
    enumerator-list , enumerator
    */

    struct TEnumerator* pEnumerator2 = TEnumerator_Create();
    List_Add(pEnumeratorList2, pEnumerator2);

    EnumeratorC(ctx, pEnumerator2);
    SymbolMap_SetAt(ctx->pCurrentScope, pEnumerator2->Name, (struct TTypePointer*)pEnumerator2);

    enum Tokens token = Parser_CurrentToken(ctx);

    //tem mais?
    if (token == TK_COMMA)
    {
        Parser_Match(ctx, &pEnumerator2->ClueList2);
        token = Parser_CurrentToken(ctx);
        pEnumerator2->bHasComma = true;

        if (token != TK_RIGHT_CURLY_BRACKET)
        {
            Enumerator_List(ctx, pEnumeratorList2);
        }
    }
}

void Enum_Specifier(struct Parser* ctx, struct TEnumSpecifier* pEnumSpecifier2)
{
    /*
    enum-specifier:
    enum identifieropt { enumerator-list }
    enum identifieropt { enumerator-list, }
    enum identifier
    */

    Parser_MatchToken(ctx, TK_ENUM, &pEnumSpecifier2->ClueList0);

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_IDENTIFIER)
    {
        const char* lexeme = Lexeme(ctx);
        PTR_STRING_REPLACE(pEnumSpecifier2->Tag, lexeme);
        Parser_Match(ctx, &pEnumSpecifier2->ClueList1);


        SymbolMap_SetAt(ctx->pCurrentScope, pEnumSpecifier2->Tag, (struct TTypePointer*)pEnumSpecifier2);
    }

    else
    {
        const char* name = GetName();
        PTR_STRING_REPLACE(pEnumSpecifier2->Tag, name);
        Parser_Match(ctx, &pEnumSpecifier2->ClueList2);
    }


    if (token == TK_IDENTIFIER)
    {
        //Ja fez Match do identifier
        token = Parser_CurrentToken(ctx);

        if (token == TK_LEFT_CURLY_BRACKET)
        {
            Parser_Match(ctx, &pEnumSpecifier2->ClueList2);

            Enumerator_List(ctx, &pEnumSpecifier2->EnumeratorList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                              &pEnumSpecifier2->ClueList3);
        }

        else
        {
            //enum identifier
            //                  ^
        }
    }
    else if (token == TK_LEFT_CURLY_BRACKET)
    {
        //ja foi feito o Match do {
        Enumerator_List(ctx, &pEnumSpecifier2->EnumeratorList);

        Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                          &pEnumSpecifier2->ClueList3);
    }

    else
    {
        SetError(ctx, "expected enum name or {");
    }
}

bool TFunctionSpecifier_IsFirst(enum Tokens token)
{
    /*
    function-specifier:
    inline
    _Noreturn
    */
    bool bResult = false;

    switch (token)
    {
        case TK_INLINE:
        case TK__INLINE://microsoft
        case TK__FORCEINLINE://microsoft
        case TK__NORETURN:
            bResult = true;
            break;

        default:
            break;
    }

    return bResult;
}

void Function_Specifier(struct Parser* ctx,
                        struct TFunctionSpecifier* pFunctionSpecifier)
{
    /*
    function-specifier:
    inline
    _Noreturn
    */

    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK__INLINE://microsoft
        case TK__FORCEINLINE://microsoft
        case TK_INLINE:
        case TK__NORETURN:
            pFunctionSpecifier->Token = token;
            Parser_Match(ctx, &pFunctionSpecifier->ClueList0);
            break;

        default:
            break;
    }
}

bool TStorageSpecifier_IsFirst(enum Tokens token)
{
    bool bResult = false;
    /*
    storage-class-specifier:
    typedef
    extern
    static
    _Thread_local
    auto
    register
    */
    switch (token)
    {
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK__THREAD_LOCAL:
        case TK_AUTO:
        case TK_REGISTER:
            bResult = true;
            break;
        default:
            break;
    }

    return bResult;
}

void Storage_Class_Specifier(struct Parser* ctx,

                             struct TStorageSpecifier* pStorageSpecifier)
{
    /*
    storage-class-specifier:
    typedef
    extern
    static
    _Thread_local
    auto
    register
    */

    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK__THREAD_LOCAL:
        case TK_AUTO:
        case TK_REGISTER:
            pStorageSpecifier->Token = token;
            Parser_Match(ctx, &pStorageSpecifier->ClueList0);
            break;

        default:
            break;
    }
}

void Parameter_List(struct Parser* ctx,
                    struct TParameterList* pParameterList)
{
    /*
    parameter-list:
    parameter-declaration
    parameter-list, parameter-declaration
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    struct TParameter* pParameter = TParameter_Create();
    List_Add(pParameterList, pParameter);
    Parameter_Declaration(ctx, pParameter);

    //Tem mais?
    token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        //a virgula fica no anterior
        pParameter->bHasComma = true;
        Parser_Match(ctx, &pParameter->ClueList0);

        token = Parser_CurrentToken(ctx);
        if (token != TK_DOTDOTDOT)
        {
            Parameter_List(ctx, pParameterList);
        }
    }
}

void Parameter_Declaration(struct Parser* ctx,
                           struct TParameter* pParameterDeclaration)
{

    //ctx->pCurrentParameterScope

    /*
    parameter-declaration:
    declaration-specifiers declarator
    declaration-specifiers abstract-declaratoropt
    */
    Declaration_Specifiers(ctx,
                           &pParameterDeclaration->Specifiers);

    struct TDeclarator* pDeclarator = NULL;
    Declarator(ctx, true, &pDeclarator);
    if (pDeclarator)
    {
        TDeclarator_Swap(&pParameterDeclaration->Declarator, pDeclarator);
        TDeclarator_Delete(pDeclarator);
    }
}

void Parameter_Type_List(struct Parser* ctx,
                         struct TParameterTypeList* pParameterList)
{
    /*
    parameter-type-list:
    parameter-list
    parameter-list , ...
    */

    Parameter_List(ctx, &pParameterList->ParameterList);

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_DOTDOTDOT)
    {
        pParameterList->bVariadicArgs = true;
        //TODO ADD this parameter
        Parser_Match(ctx, &pParameterList->ClueList1);
    }
}


void Direct_Declarator(struct Parser* ctx, bool bAbstract, struct TDirectDeclarator** ppDeclarator2)
{
    *ppDeclarator2 = NULL; //out

                           /*
                           direct-declarator:
                           identifier
                           ( declarator )
                           direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
                           direct-declarator [ static type-qualifier-listopt assignment-expression ]
                           direct-declarator [ type-qualifier-list static assignment-expression ]
                           direct-declarator [ type-qualifier-listopt * ]
                           direct-declarator ( parameter-type-list )
                           direct-declarator ( identifier-listopt )
                           */
    struct TDirectDeclarator* pDirectDeclarator = NULL;

    if (ErrorOrEof(ctx))
        return;


    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        case TK_LEFT_PARENTHESIS:
        {
            //assert(pDirectDeclarator == NULL);
            pDirectDeclarator = TDirectDeclarator_Create();

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pDirectDeclarator->ClueList0);

            Declarator(ctx, bAbstract, &pDirectDeclarator->pDeclarator);
            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pDirectDeclarator->ClueList1);

            //Para indicar que eh uma ( declarator )
            pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeDeclarator;
            // ) para nao confundir com funcao (
        }
        break;

        case TK_IDENTIFIER:
        {
            //identifier
            pDirectDeclarator = TDirectDeclarator_Create();

            //Para indicar que eh uma identificador

            pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeIdentifier;

            const char* lexeme = Lexeme(ctx);
            PTR_STRING_REPLACE(pDirectDeclarator->Identifier, lexeme);
            pDirectDeclarator->Position.Line = GetCurrentLine(ctx);
            pDirectDeclarator->Position.FileIndex = GetFileIndex(ctx);
            Parser_Match(ctx, &pDirectDeclarator->ClueList0);
        }
        break;

        default:
            ////assert(false);
            break;
    }



    if (pDirectDeclarator == NULL)
    {
        //Por enquanto esta funcao esta sendo usada para
        //abstract declarator que nao tem nome.
        //vou criar aqui por enquanto um cara vazio
        pDirectDeclarator = TDirectDeclarator_Create();
        PTR_STRING_REPLACE(pDirectDeclarator->Identifier, "");
        pDirectDeclarator->Position.Line = GetCurrentLine(ctx);
        pDirectDeclarator->Position.FileIndex = GetFileIndex(ctx);

        //Para indicar que eh uma identificador

        pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeIdentifier;

        //Quando tiver abstract declarator vai ser 
        //bug cair aqui
    }

    *ppDeclarator2 = pDirectDeclarator;

    for (;;)
    {
        //assert(pDirectDeclarator != NULL);

        token = Parser_CurrentToken(ctx);
        switch (token)
        {
            case TK_LEFT_PARENTHESIS:

                /*
                direct-declarator ( parameter-type-list )
                direct-declarator ( identifier-listopt )
                */
                //      pDirectDeclarator->token = token;
                //      //assert(pDirectDeclarator->pParametersOpt == NULL);
                //      pDirectDeclarator->pParametersOpt = TParameterList_Create();
                token = Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pDirectDeclarator->ClueList2);

                //Para indicar que eh uma funcao

                pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeFunction;

                if (token != TK_RIGHT_PARENTHESIS)
                {
                    //opt
                    Parameter_Type_List(ctx, &pDirectDeclarator->Parameters);
                }
                Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pDirectDeclarator->ClueList3);
                break;

            case TK_LEFT_SQUARE_BRACKET:
                /*
                direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
                direct-declarator [ static type-qualifier-listopt assignment-expression ]
                direct-declarator [ type-qualifier-list static assignment-expression ]
                direct-declarator [ type-qualifier-listopt * ]
                */

                ////assert(pDirectDeclarator->pParametersOpt == NULL);
                //pDirectDeclarator->pParametersOpt = TParameterList_Create();

                //Para indicar que eh um array

                pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeArray;

                token = Parser_MatchToken(ctx, TK_LEFT_SQUARE_BRACKET, &pDirectDeclarator->ClueList2);
                if (token == TK_STATIC)
                {
                }
                else if (token == TK_AUTO)
                {
                    //int a[auto];
                    pDirectDeclarator->DeclaratorType = TDirectDeclaratorTypeAutoArray;
                    Parser_MatchToken(ctx, TK_AUTO, &pDirectDeclarator->ClueList3);
                }
                else
                {
                    if (token != TK_RIGHT_SQUARE_BRACKET)
                    {
                        //assert(pDirectDeclarator->pExpression == NULL);
                        AssignmentExpression(ctx, &pDirectDeclarator->pExpression);
                    }
                    else
                    {
                        //array vazio é permitido se for o ultimo cara da struct          
                        //struct X { int ElementCount;  int Elements[]; };           
                    }
                }

                Parser_MatchToken(ctx, TK_RIGHT_SQUARE_BRACKET, &pDirectDeclarator->ClueList3);

                break;
            default:
                //assert(false);
                break;
        }

        token = Parser_CurrentToken(ctx);
        if (token != TK_LEFT_PARENTHESIS && token != TK_LEFT_SQUARE_BRACKET)
        {
            break;
        }
        else
        {
            struct TDirectDeclarator* pDirectDeclaratorNext = TDirectDeclarator_Create();
            pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
            pDirectDeclarator = pDirectDeclaratorNext;
        }
    }

    token = Parser_CurrentToken(ctx);
    if (token == TK_LEFT_PARENTHESIS)
    {
        //tem mais
        struct TDirectDeclarator* pDirectDeclaratorNext = NULL;
        Direct_Declarator(ctx, bAbstract, &pDirectDeclaratorNext);
        pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
    }
    else if (!bAbstract && token == TK_IDENTIFIER)
    {
        //tem mais
        struct TDirectDeclarator* pDirectDeclaratorNext = NULL;
        Direct_Declarator(ctx, bAbstract, &pDirectDeclaratorNext);
        pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
    }


}

static bool TTypeQualifier_IsFirst(enum Tokens token)
{
    bool bResult = false;

    switch (token)
    {

        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
        case TK__ATOMIC:
            bResult = true;
            break;

#ifdef LANGUAGE_EXTENSIONS
            //type-qualifier-extensions 
        case TK_LEFT_SQUARE_BRACKET:
        case TK_AUTO:

            bResult = true;
            break;
#endif
        default:
            break;
    }

    return bResult;
}

#ifdef LANGUAGE_EXTENSIONS
void Size_Qualifier(struct Parser* ctx, struct TTypeQualifier* pQualifier)
{
    enum Tokens token = Parser_CurrentToken(ctx);
    pQualifier->Token = token;
    //token = Parser_Match(ctx, );

    token = Parser_MatchToken(ctx, TK_LEFT_SQUARE_BRACKET, &pQualifier->ClueList0);

    switch (token)
    {
        case TK_IDENTIFIER:
            PTR_STRING_REPLACE(pQualifier->SizeIdentifier, Lexeme(ctx));
            token = Parser_MatchToken(ctx, TK_IDENTIFIER, NULL);
            break;
        default:
            break;
    }


    token = Parser_MatchToken(ctx, TK_RIGHT_SQUARE_BRACKET, NULL);

}
#endif


bool Type_Qualifier(struct Parser* ctx, struct TTypeQualifier* pQualifier)
{
    /*
    type-qualifier:
    const
    restrict
    volatile
    _Atomic
    */

    //extensions
    /*
    auto
    _size(identifier)
    _size(int)
    */

    bool bResult = false;
    enum Tokens token = Parser_CurrentToken(ctx);

    //const char* lexeme = Lexeme(ctx);
    switch (token)
    {

        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
        case TK__ATOMIC:
            pQualifier->Token = token;
            Parser_Match(ctx, &pQualifier->ClueList0);
            bResult = true;
            break;

#ifdef LANGUAGE_EXTENSIONS

        case TK_LEFT_SQUARE_BRACKET:

            Size_Qualifier(ctx, pQualifier);
            bResult = true;
            break;

        case TK_AUTO:


            pQualifier->Token = token;
            Parser_Match(ctx, &pQualifier->ClueList0);
            bResult = true;
            break;
#endif
        default:
            break;
    }

    return bResult;
}

void Type_Qualifier_List(struct Parser* ctx,
                         struct TTypeQualifierList* pQualifiers)
{
    /*
    type-qualifier-list:
    type-qualifier
    type-qualifier-list type-qualifier
    */

    struct TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
    Type_Qualifier(ctx, pTypeQualifier);
    TTypeQualifierList_PushBack(pQualifiers, pTypeQualifier);

    if (IsTypeQualifierToken(Parser_CurrentToken(ctx)))
    {
        Type_Qualifier_List(ctx, pQualifiers);
    }

}


void Pointer(struct Parser* ctx, struct TPointerList* pPointerList)
{
    /*
    pointer:
    * type-qualifier-listopt
    * type-qualifier-listopt pointer
    */

    struct TPointer* pPointer = TPointer_Create();

    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_ASTERISK)
    {
        //ANNOTATED AQUI VAI TER AUTO SIZEOF
        TPointerList_PushBack(pPointerList, pPointer);
        Parser_Match(ctx, &pPointer->ClueList0);
    }
    else
    {
        //Erro
        SetError(ctx, "pointer error");
    }

    token = Parser_CurrentToken(ctx);


    //Opcional
    if (IsTypeQualifierToken(token))
    {
        Type_Qualifier_List(ctx, &pPointer->Qualifier);
    }

    token = Parser_CurrentToken(ctx);

    //Tem mais?
    if (token == TK_ASTERISK)
    {
        Pointer(ctx, pPointerList);
    }
}

//pag 123 C
void Declarator(struct Parser* ctx, bool bAbstract, struct TDeclarator** ppTDeclarator2)
{
    //assert(*ppTDeclarator2 == NULL);
    *ppTDeclarator2 = NULL; //out

    struct TDeclarator* pDeclarator = TDeclarator_Create();
    /*
    declarator:
    pointeropt direct-declarator
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_ASTERISK)
    {
        Pointer(ctx, &pDeclarator->PointerList);
    }

    //assert(pDeclarator->pDirectDeclarator == NULL);
    Direct_Declarator(ctx, bAbstract, &pDeclarator->pDirectDeclarator);

    *ppTDeclarator2 = pDeclarator;
}


bool TAlignmentSpecifier_IsFirst(enum Tokens token)
{
    /*
    alignment - specifier:
    _Alignas(type - name)
    _Alignas(constant - expression)
    */
    return (token == TK__ALIGNAS);
}

bool Alignment_Specifier(struct Parser* ctx,
                         struct TAlignmentSpecifier* pAlignmentSpecifier)
{
    bool bResult = false;
    /*
    alignment - specifier:
    _Alignas(type - name)
    _Alignas(constant - expression)
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__ALIGNAS)
    {
        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, NULL);
        //assert(false);//TODO
        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL);
        bResult = true;
    }

    return bResult;
}


bool TTypeSpecifier_IsFirst(struct Parser* ctx, enum Tokens token, const char* lexeme)
{
    /*
    type-specifier:
    void
    char
    short
    int
    long
    float
    double
    signed
    unsigned
    _Bool
    _Complex
    atomic-type-specifier
    struct-or-union-specifier
    enum-specifier
    typedef-name
    */

    bool bResult = false;

    switch (token)
    {
        case TK_VOID:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_LONG:
            //microsoft
        case TK__INT8:
        case TK__INT16:
        case TK__INT32:
        case TK__INT64:
        case TK__WCHAR_T:
            /////
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK__BOOL:
        case TK__COMPLEX:
        case TK__ATOMIC:

        case TK_STRUCT:
        case TK_UNION:
        case TK_ENUM:
            bResult = true;
            break;

        case TK_IDENTIFIER:
            bResult = IsTypeName(ctx, TK_IDENTIFIER, lexeme);
            break;

        default:
            break;
    }

    return bResult;
}


void AtomicTypeSpecifier(struct Parser* ctx,
                         struct TTypeSpecifier** ppTypeSpecifier)
{
    //assert(false); //tODO criar struct TAtomicTypeSpecifier
                   /*
                   atomic-type-specifier:
                   _Atomic ( type-name )
                   */
    struct TAtomicTypeSpecifier* pAtomicTypeSpecifier =
        TAtomicTypeSpecifier_Create();

    *ppTypeSpecifier = TAtomicTypeSpecifier_As_TTypeSpecifier(pAtomicTypeSpecifier);

    Parser_MatchToken(ctx, TK__ATOMIC, &pAtomicTypeSpecifier->ClueList0);

    Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pAtomicTypeSpecifier->ClueList1);

    TypeName(ctx, &pAtomicTypeSpecifier->TypeName);

    Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pAtomicTypeSpecifier->ClueList2);
}

void Type_Specifier(struct Parser* ctx, struct TTypeSpecifier** ppTypeSpecifier)
{
    /*
    type-specifier:
    void
    char
    short
    int
    long
    float
    double
    signed
    unsigned
    _Bool
    _Complex
    atomic-type-specifier
    struct-or-union-specifier
    enum-specifier
    typedef-name
    */


    //bool bResult = false;

    const char* lexeme = Lexeme(ctx);
    enum Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        //type - specifier
        case TK_VOID:
        case TK_CHAR:
        case TK_SHORT:
        case TK_INT:
        case TK_LONG:
            //microsoft
        case TK__INT8:
        case TK__INT16:
        case TK__INT32:
        case TK__INT64:
        case TK__WCHAR_T:
            /////////
        case TK_FLOAT:
        case TK_DOUBLE:
        case TK_SIGNED:
        case TK_UNSIGNED:
        case TK__BOOL:
        case TK__COMPLEX:
        {
            struct TSingleTypeSpecifier* pSingleTypeSpecifier =
                TSingleTypeSpecifier_Create();

            pSingleTypeSpecifier->Token2 = token;
            //bResult = true;

            Parser_Match(ctx, &pSingleTypeSpecifier->ClueList0);

            *ppTypeSpecifier = (struct TTypeSpecifier*)pSingleTypeSpecifier;
        }
        break;

        //atomic-type-specifier
        case TK__ATOMIC:
            //bResult = true;
            AtomicTypeSpecifier(ctx, ppTypeSpecifier);
            break;

        case TK_STRUCT:
        case TK_UNION:
        {
            //assert(*ppTypeSpecifier == NULL);
            //bResult = true;
            struct TStructUnionSpecifier* pStructUnionSpecifier = TStructUnionSpecifier_Create();

            *ppTypeSpecifier = (struct TTypeSpecifier*)pStructUnionSpecifier;
            Struct_Or_Union_Specifier(ctx, pStructUnionSpecifier);
        }
        break;

        case TK_ENUM:
        {
            //assert(*ppTypeSpecifier == NULL);
            //bResult = true;
            struct TEnumSpecifier* pEnumSpecifier2 = TEnumSpecifier_Create();
            *ppTypeSpecifier = (struct TTypeSpecifier*)pEnumSpecifier2;
            Enum_Specifier(ctx, pEnumSpecifier2);
        }
        break;

        case TK_IDENTIFIER:
        {
            int bIsTypedef = IsTypeName(ctx, TK_IDENTIFIER, lexeme);
            if (bIsTypedef)
            {
                struct TSingleTypeSpecifier* pSingleTypeSpecifier = TSingleTypeSpecifier_Create();

                if (bIsTypedef == 2 /*struct*/)
                    pSingleTypeSpecifier->Token2 = TK_STRUCT;
                else if (bIsTypedef == 3 /*union*/)
                    pSingleTypeSpecifier->Token2 = TK_UNION;
                else if (bIsTypedef == 4 /*enum*/)
                    pSingleTypeSpecifier->Token2 = TK_ENUM;

                else
                    pSingleTypeSpecifier->Token2 = token;

                PTR_STRING_REPLACE(pSingleTypeSpecifier->TypedefName, lexeme);
                //bResult = true;

                Parser_Match(ctx, &pSingleTypeSpecifier->ClueList0);
                *ppTypeSpecifier = (struct TTypeSpecifier*)pSingleTypeSpecifier;

            }
            else
            {
                //assert(false); //temque chegar aqui limpo ja
                SetError(ctx, "internal error 2");
            }
        }
        break;

        default:
            break;
    }

    //token = Parser_CurrentToken(ctx);
    //if (token == TK_VERTICAL_LINE)
    //{
    //criar uma lista
    //}
}

bool Declaration_Specifiers_IsFirst(struct Parser* ctx, enum Tokens token, const char* lexeme)
{
    /*
    declaration-specifiers:
    storage-class-specifier declaration-specifiersopt
    type-specifier          declaration-specifiersopt
    type-qualifier          declaration-specifiersopt
    function-specifier      declaration-specifiersopt
    alignment-specifier     declaration-specifiersopt
    */
    bool bResult =
        TStorageSpecifier_IsFirst(token) ||
        TTypeSpecifier_IsFirst(ctx, token, lexeme) ||
        TTypeQualifier_IsFirst(token) ||
        TFunctionSpecifier_IsFirst(token) ||
        TFunctionSpecifier_IsFirst(token);

    return bResult;
}

void Declaration_Specifiers(struct Parser* ctx,
                            struct TDeclarationSpecifiers* pDeclarationSpecifiers)
{
    /*
    declaration-specifiers:
    storage-class-specifier declaration-specifiersopt
    type-specifier          declaration-specifiersopt
    type-qualifier          declaration-specifiersopt
    function-specifier      declaration-specifiersopt
    alignment-specifier     declaration-specifiersopt
    */
    enum Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);

    if (TStorageSpecifier_IsFirst(token))
    {
        struct TStorageSpecifier* pStorageSpecifier = TStorageSpecifier_Create();

        Storage_Class_Specifier(ctx, pStorageSpecifier);

        TDeclarationSpecifiers_PushBack(pDeclarationSpecifiers, TStorageSpecifier_As_TDeclarationSpecifier(pStorageSpecifier));
    }
    else if (TTypeSpecifier_IsFirst(ctx, token, lexeme))
    {
        if (TDeclarationSpecifiers_CanAddSpeficier(pDeclarationSpecifiers,
            token,
            lexeme))
        {
            struct TTypeSpecifier* pTypeSpecifier = NULL;
            Type_Specifier(ctx, &pTypeSpecifier);
            //ATENCAO
            TDeclarationSpecifiers_PushBack(pDeclarationSpecifiers, (struct TDeclarationSpecifier*)pTypeSpecifier);
        }
        else
        {
            SetError(ctx, "double typedef");
        }

    }
    else if (TTypeQualifier_IsFirst(token))
    {
        struct TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
        Type_Qualifier(ctx, pTypeQualifier);
        //ATENCAO
        TDeclarationSpecifiers_PushBack(pDeclarationSpecifiers, (struct TDeclarationSpecifier*)TTypeQualifier_As_TSpecifierQualifier(pTypeQualifier));
    }
    else if (TFunctionSpecifier_IsFirst(token))
    {
        struct TFunctionSpecifier* pFunctionSpecifier = TFunctionSpecifier_Create();
        Function_Specifier(ctx, pFunctionSpecifier);
        TDeclarationSpecifiers_PushBack(pDeclarationSpecifiers, TFunctionSpecifier_As_TDeclarationSpecifier(pFunctionSpecifier));
    }
    else if (TAlignmentSpecifier_IsFirst(token))
    {
        //assert(false);
        //struct TAlignmentSpecifier* pAlignmentSpecifier = TAlignmentSpecifier_Create();
        //List_Add(pDeclarationSpecifiers, TAlignmentSpecifier_As_TDeclarationSpecifier(pAlignmentSpecifier));
    }
    else
    {
        SetError(ctx, "internal error 3");
    }

    token = Parser_CurrentToken(ctx);
    lexeme = Lexeme(ctx);

    //Tem mais?
    if (Declaration_Specifiers_IsFirst(ctx, token, lexeme))
    {
        if (TDeclarationSpecifiers_CanAddSpeficier(pDeclarationSpecifiers,
            token,
            lexeme))
        {
            Declaration_Specifiers(ctx, pDeclarationSpecifiers);
        }

    }

}



void Initializer(struct Parser* ctx,
                 struct TInitializer** ppInitializer,
                 enum Tokens endToken1,
                 enum Tokens endToken2)
{
    //assert(*ppInitializer == NULL);
    /*
    initializer:
    assignment-expression
    { initializer-list }
    { initializer-list , }
    */

    /*
    initializer:
    default
    assignment-expression
    _defaultopt { initializer-list }
    _defaultopt { initializer-list , }
    */

    enum Tokens token = Parser_CurrentToken(ctx);
    if (
        token == TK_DEFAULT)
    {
        struct TInitializerListType* pTInitializerList =
            TInitializerListType_Create();

        pTInitializerList->bDefault = true;

        *ppInitializer = (struct TInitializer*)pTInitializerList;

        Parser_Match(ctx, &pTInitializerList->ClueList0);
        token = Parser_CurrentToken(ctx);


        if (token == TK_LEFT_CURLY_BRACKET)
        {
            Parser_Match(ctx, &pTInitializerList->ClueList1);

            Initializer_List(ctx, &pTInitializerList->InitializerList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                              &pTInitializerList->ClueList2);
        }
    }
    else
    {
        if (token == TK_LEFT_CURLY_BRACKET)
        {

            struct TInitializerListType* pTInitializerList =
                TInitializerListType_Create();


            *ppInitializer = (struct TInitializer*)pTInitializerList;

            Parser_Match(ctx, &pTInitializerList->ClueList1);
            //ANNOTATED AQUI DIZ DEFAULT 

            Initializer_List(ctx, &pTInitializerList->InitializerList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                              &pTInitializerList->ClueList2);
        }

        else
        {
            struct TExpression* pExpression = NULL;
            AssignmentExpression(ctx, &pExpression);
            *ppInitializer = (struct TInitializer*)pExpression;
        }
    }
}

void Initializer_List(struct Parser* ctx, struct TInitializerList* pInitializerList)
{
    /*
    initializer-list:
    designationopt initializer
    initializer-list , designationopt initializer
    */
    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        enum Tokens token = Parser_CurrentToken(ctx);
        if (token == TK_RIGHT_CURLY_BRACKET)
        {
            //Empty initializer
            break;
        }

        struct TInitializerListItem* pTInitializerListItem = TInitializerListItem_Create();
        List_Add(pInitializerList, pTInitializerListItem);

#ifdef LANGUAGE_EXTENSIONS

#endif
        if (token == TK_LEFT_SQUARE_BRACKET ||
            token == TK_FULL_STOP)
        {
            Designation(ctx, &pTInitializerListItem->DesignatorList);
        }

        Initializer(ctx, &pTInitializerListItem->pInitializer, TK_COMMA, TK_RIGHT_CURLY_BRACKET);
        //push
        token = Parser_CurrentToken(ctx);

        if (token == TK_COMMA)
        {
            //TNodeClueList_MoveToEnd(&pTInitializerListItem->ClueList, &ctx->Scanner.ClueList);
            Parser_Match(ctx, &pTInitializerListItem->ClueList);
            //tem mais
        }

        else
        {
            break;
        }
    }
}

void Designation(struct Parser* ctx, struct TDesignatorList* pDesignatorList)
{
    /*
    designation:
    designator-list =
    */
    Designator_List(ctx, pDesignatorList);
    Parser_MatchToken(ctx, TK_EQUALS_SIGN, NULL);//tODO
}

void Designator_List(struct Parser* ctx, struct TDesignatorList* pDesignatorList)
{
    // http://www.drdobbs.com/the-new-c-declarations-initializations/184401377
    /*
    designator-list:
    designator
    designator-list designator
    */
    struct TDesignator* pDesignator = TDesignator_Create();
    Designator(ctx, pDesignator);
    TDesignatorList_PushBack(pDesignatorList, pDesignator);

    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        enum Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_LEFT_SQUARE_BRACKET ||
            token == TK_FULL_STOP)
        {
            struct TDesignator* pDesignatorNew = TDesignator_Create();
            Designator(ctx, pDesignatorNew);
            List_Add(pDesignatorList, pDesignatorNew);
        }

        else
        {
            break;
        }
    }
}

void Designator(struct Parser* ctx, struct TDesignator* p)
{
    /*
    designator:
    [ constant-expression ]
    . identifier
    */
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_LEFT_SQUARE_BRACKET)
    {
        Parser_Match(ctx, &p->ClueList0);

        ConstantExpression(ctx, &p->pExpression);

        Parser_Match(ctx, &p->ClueList1);
        Parser_MatchToken(ctx, TK_RIGHT_SQUARE_BRACKET, NULL);
    }

    else if (token == TK_FULL_STOP)
    {
        //TNodeClueList_MoveToEnd(&p->ClueList, &ctx->Scanner.ClueList);
        Parser_Match(ctx, &p->ClueList0);

        PTR_STRING_REPLACE(p->Name, Lexeme(ctx));
        Parser_MatchToken(ctx, TK_IDENTIFIER, NULL);
    }
}

void Init_Declarator(struct Parser* ctx,
                     struct TInitDeclarator** ppDeclarator2)
{
    /*
    init-declarator:
    declarator
    declarator = initializer
    */
    struct TInitDeclarator* pInitDeclarator =
        TInitDeclarator_Create();

    Declarator(ctx, false, &pInitDeclarator->pDeclarator);
    enum Tokens token = Parser_CurrentToken(ctx);

    const char* declaratorName = TInitDeclarator_FindName(pInitDeclarator);
    if (declaratorName)
    {
        //Fica em um contexto que vive so durante a declaracao
        //depois eh substituido

        SymbolMap_SetAt(ctx->pCurrentScope, declaratorName, (struct TTypePointer*)pInitDeclarator);
    }

    //Antes do =
    *ppDeclarator2 = pInitDeclarator;

    if (token == TK_EQUALS_SIGN)
    {
        //assert(*ppDeclarator2 != NULL);

        Parser_Match(ctx, &pInitDeclarator->ClueList0);

        Initializer(ctx, &pInitDeclarator->pInitializer, TK_SEMICOLON, TK_SEMICOLON);
        ////TNodeClueList_MoveToEnd(&pInitDeclarator->ClueList, &ctx->Scanner.ClueList);
    }
}

void Init_Declarator_List(struct Parser* ctx,
                          struct TInitDeclaratorList* pInitDeclaratorList)
{
    /*
    init-declarator-list:
    init-declarator
    init-declarator-list , init-declarator
    */

    struct TInitDeclarator* pInitDeclarator = NULL;
    Init_Declarator(ctx, &pInitDeclarator);
    List_Add(pInitDeclaratorList, pInitDeclarator);

    //Tem mais?
    enum Tokens token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        Parser_Match(ctx, &pInitDeclarator->ClueList0);

        Init_Declarator_List(ctx, pInitDeclaratorList);
    }

}

void Parse_Declarations(struct Parser* ctx, struct TDeclarations* declarations);

void GroupDeclaration(struct Parser* ctx,
                      struct TGroupDeclaration** ppGroupDeclaration)
{
    /*
    CPRIME
    group-declaration:
    default identifier { declarations opt }
    */

    struct TGroupDeclaration* p = TGroupDeclaration_Create();
    *ppGroupDeclaration = p;

    Parser_Match(ctx, &p->ClueList0);//default


    PTR_STRING_REPLACE(p->Identifier, Lexeme(ctx));
    Parser_MatchToken(ctx, TK_IDENTIFIER, &p->ClueList1);//identifier

    Parser_MatchToken(ctx, TK_LEFT_CURLY_BRACKET, &p->ClueList2);

    enum Tokens token = Parser_CurrentToken(ctx);
    if (token != TK_RIGHT_CURLY_BRACKET)
    {
        Parse_Declarations(ctx, &p->Declarations);
    }
    else
    {
        //vazio
    }
    Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET, NULL);
}

void PopBack(struct TScannerItemList* clueList)
{
    if (clueList->pHead &&
        clueList->pHead->pNext == clueList->pTail)
    {
        ScannerItem_Delete(clueList->pTail);
        clueList->pTail = clueList->pHead;
        clueList->pHead->pNext = 0;
    }
}
bool HasCommentedKeyword(struct TScannerItemList* clueList, const char* keyword)
{
    bool bResult = false;
    struct ScannerItem* pCurrent = clueList->pTail;
    if (pCurrent &&
        pCurrent->token == TK_COMMENT)
    {
        bResult = strncmp(pCurrent->lexeme.c_str + 2, keyword, strlen(keyword)) == 0;
    }
    return bResult;
}

static bool IsDefaultFunctionTag(const char* functionTag)
{
    if (functionTag)
    {
        if (strcmp(functionTag, "init") == 0 ||
            strcmp(functionTag, "delete") == 0 ||
            strcmp(functionTag, "destroy") == 0 ||
            strcmp(functionTag, "create") == 0)
        {
            return true;
        }
    }
    return false;
}

bool  Declaration(struct Parser* ctx,
                  struct TAnyDeclaration** ppDeclaration)
{
    /*
    declaration:
    declaration-specifiers;
    declaration-specifiers init-declarator-list ;
    static_assert-declaration
    */



    bool bHasDeclaration = false;
    enum Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__STATIC_ASSERT)
    {
        struct TStaticAssertDeclaration* pStaticAssertDeclaration = TStaticAssertDeclaration_Create();
        *ppDeclaration = (struct TAnyDeclaration*)pStaticAssertDeclaration;
        Static_Assert_Declaration(ctx, pStaticAssertDeclaration);
        bHasDeclaration = true;

    }
    else if (token == TK_DEFAULT)
    {
        struct TGroupDeclaration* p = NULL;
        GroupDeclaration(ctx, &p);
        *ppDeclaration = (struct TAnyDeclaration*)p;//moved
        bHasDeclaration = true;
    }
    else
    {
        struct TDeclaration* pFuncVarDeclaration = TDeclaration_Create();

        if (token == TK_SEMICOLON)
        {
            //declaracao vazia como ;
            bHasDeclaration = true;
            //Match(ctx);
        }
        else
        {
            if (Declaration_Specifiers_IsFirst(ctx, Parser_CurrentToken(ctx), Lexeme(ctx)))
            {
                Declaration_Specifiers(ctx, &pFuncVarDeclaration->Specifiers);
                bHasDeclaration = true;
            }
        }

        if (bHasDeclaration)
        {
            *ppDeclaration = (struct TAnyDeclaration*)pFuncVarDeclaration;
            pFuncVarDeclaration->FileIndex = GetFileIndex(ctx);
            pFuncVarDeclaration->Line = GetCurrentLine(ctx);
            ////assert(pFuncVarDeclaration->FileIndex >= 0);

            token = Parser_CurrentToken(ctx);

            if (token == TK_SEMICOLON)
            {
                Parser_Match(ctx, &pFuncVarDeclaration->ClueList1);
            }

            else
            {
                //Pega os parametros das funcoes mas nao usa
                //se nao for uma definicao de funcao


                //////////////////////
                /////vou criar um escopo para declarators
                // int* p = malloc(sizeof p);
                //                        ^
                //                       p esta no contexto
                // mas nao tem toda declaracao

                struct SymbolMap BlockScope = SYMBOLMAP_INIT;

                BlockScope.pPrevious = ctx->pCurrentScope;
                ctx->pCurrentScope = &BlockScope;


                //Agora vem os declaradores que possuem os ponteiros
                Init_Declarator_List(ctx, &pFuncVarDeclaration->InitDeclaratorList);


                ctx->pCurrentScope = BlockScope.pPrevious;
                SymbolMap_Destroy(&BlockScope);

                ////////////////////////

                token = Parser_CurrentToken(ctx);

                //colocar os declaradores nos simbolos
                //agora ele monta a tabela com a declaracao toda
                for (struct TInitDeclarator* pInitDeclarator = pFuncVarDeclaration->InitDeclaratorList.pHead;
                     pInitDeclarator != NULL;
                     pInitDeclarator = pInitDeclarator->pNext)
                {
                    const char* declaratorName = TInitDeclarator_FindName(pInitDeclarator);



                    if (declaratorName != NULL)
                    {
                        SymbolMap_SetAt(ctx->pCurrentScope, declaratorName, (struct TTypePointer*)pFuncVarDeclaration);
                    }

                    //ctx->
                }

                if (token == TK_COLON)
                {
                    Parser_Match(ctx, &pFuncVarDeclaration->ClueList00); //:
                    token = Parser_CurrentToken(ctx);

                    assert(pFuncVarDeclaration->FunctionTag == NULL);
                    pFuncVarDeclaration->FunctionTag = StrDup(Lexeme(ctx));

                    if (!IsDefaultFunctionTag(pFuncVarDeclaration->FunctionTag))
                    {
                        SetWarning(ctx, "Unknown tag '%s'", pFuncVarDeclaration->FunctionTag);
                    }

                    Parser_Match(ctx, &pFuncVarDeclaration->ClueList001);
                    token = Parser_CurrentToken(ctx);

                    SymbolMap_SetAt(ctx->pCurrentScope,
                                    pFuncVarDeclaration->FunctionTag,
                                    (struct TTypePointer*)pFuncVarDeclaration);
                }
                else if (!ctx->ParserOptions.bNoImplicitTag)
                {
                    const char* funcName = TDeclaration_GetFunctionName(pFuncVarDeclaration);
                    const char* functionTag = NULL;

                    //auto tag
                    if (funcName && IsSuffix(funcName, "init"))
                    {
                        //tem que ter 1 argumento
                        functionTag = "init";
                    }
                    else if (funcName && IsSuffix(funcName, "destroy"))
                    {
                        //tem que ter 1 argumento
                        functionTag = "destroy";
                    }
                    else if (funcName && IsSuffix(funcName, "delete"))
                    {
                        //tem que ter 1 argumento
                        functionTag = "delete";
                    }
                    else if (funcName && IsSuffix(funcName, "create"))
                    {
                        //tem que ter 0 argumentos
                        functionTag = "create";
                    }
                    else
                    {

                    }
                    if (functionTag)
                    {
                        assert(pFuncVarDeclaration->FunctionTag == NULL);
                        pFuncVarDeclaration->FunctionTag = StrDup(functionTag);
                        pFuncVarDeclaration->bAutoTag = true;
                        SymbolMap_SetAt(ctx->pCurrentScope,
                                        functionTag,
                                        (struct TTypePointer*)pFuncVarDeclaration);
                    }

                }

                //
                if (token == TK_DEFAULT)
                {
                    /*
                    6.9.1) function-definition:
                    declaration-specifiers declarator declaration-listopt defaultopt compound-statement
                    */

                    pFuncVarDeclaration->bDefault = true;
                    Parser_Match(ctx, &pFuncVarDeclaration->ClueList0);
                    token = Parser_CurrentToken(ctx);
                }

                if (token == TK_LEFT_CURLY_BRACKET)
                {
                    //Ativa o escopo dos parametros
                    //Adiconar os parametros em um escopo um pouco a cima.
                    struct SymbolMap BlockScope2 = SYMBOLMAP_INIT;

                    struct TInitDeclarator* pDeclarator3 =
                        pFuncVarDeclaration->InitDeclaratorList.pHead;

                    for (struct TParameter* pParameter = pDeclarator3->pDeclarator->pDirectDeclarator->Parameters.ParameterList.pHead;
                         pParameter != NULL;
                         pParameter = pParameter->pNext)
                    {
                        const char* parameterName = TDeclarator_GetName(&pParameter->Declarator);
                        if (parameterName != NULL)
                        {
                            SymbolMap_SetAt(&BlockScope2, parameterName, (struct TTypePointer*)pParameter);
                        }
                        else
                        {
                            //parametro sem nome
                        }
                    }

                    BlockScope2.pPrevious = ctx->pCurrentScope;
                    ctx->pCurrentScope = &BlockScope2;


                    //SymbolMap_Print(ctx->pCurrentScope);
                    /*
                    6.9.1) function-definition:
                    declaration-specifiers declarator declaration-listopt compound-statement
                    */
                    struct TStatement* pStatement;
                    Compound_Statement(ctx, &pStatement);
                    //TODO cast

                    ctx->pCurrentScope = BlockScope2.pPrevious;
                    SymbolMap_Destroy(&BlockScope2);


                    pFuncVarDeclaration->pCompoundStatementOpt = (struct TCompoundStatement*)pStatement;
                }

                else
                {

                    //ANNOTATED AQUI TEM O COMENTARIO /*@default*/ antes do ;
                    Parser_MatchToken(ctx, TK_SEMICOLON, &pFuncVarDeclaration->ClueList1);
                }


            }

            // StrBuilder_Swap(&pFuncVarDeclaration->PreprocessorAndCommentsString,
            // &ctx->Scanner.PreprocessorAndCommentsString);


        }
        else
        {
            TDeclaration_Delete(pFuncVarDeclaration);
        }
    }


    return bHasDeclaration;
}


void Parse_Declarations(struct Parser* ctx, struct TDeclarations* declarations)
{
    int declarationIndex = 0;

    while (!ErrorOrEof(ctx))
    {

        struct TAnyDeclaration* pDeclarationOut = NULL;

        bool bHasDecl = Declaration(ctx, &pDeclarationOut);
        if (bHasDecl)
        {


            //TDeclarations_Destroy(&ctx->Templates);
            //TDeclarations_Init(&ctx->Templates);

            //printf("%s", ctx->Scanner.PreprocessorAndCommentsString);
            // StrBuilder_Clear(&ctx->Scanner.PreprocessorAndCommentsString);

            //Cada Declaration poderia ter out uma lista struct TDeclarations
            //publica que vai ser inserida aqui.
            //
            TDeclarations_PushBack(declarations, pDeclarationOut);


            declarationIndex++;
        }
        else
        {
            if (Parser_CurrentToken(ctx) == TK_EOF)
            {
                //ok
                Parser_Match(ctx, NULL);
            }
            else
            {
                //nao ter mais declaracao nao eh erro
                //SetError(ctx, "declaration expected");
            }
            break;
        }

        if (Parser_CurrentToken(ctx) == TK_EOF)
        {
            break;
        }

        if (Parser_HasError(ctx))
            break;
    }

    if (Parser_CurrentToken(ctx) == TK_EOF)
    {
        struct TEofDeclaration* pEofDeclaration =
            TEofDeclaration_Create();
        //ok
        Parser_Match(ctx, &pEofDeclaration->ClueList0);
        TDeclarations_PushBack(declarations, (struct TAnyDeclaration*)pEofDeclaration);
    }


}

void Parser_Main(struct Parser* ctx, struct TDeclarations* declarations)
{
    Parse_Declarations(ctx, declarations);
}

static void TFileMapToStrArray(TFileMap* map, struct TFileArray* arr)
{
    TFileArray_Reserve(arr, map->nCount);
    arr->Size = map->nCount;

    for (int i = 0; i < map->nHashTableSize; i++)
    {
        struct MapItem2* data = map->pHashTable[i];

        for (struct MapItem2* pCurrent = data;
             pCurrent;
             pCurrent = pCurrent->pNext)
        {
            struct TFile* pFile = (struct TFile*)pCurrent->pValue;

            if (pFile->FileIndex >= 0 &&
                pFile->FileIndex < (int)arr->Size)
            {
                arr->pItems[pFile->FileIndex] = pFile;
                pCurrent->pValue = NULL; //movido para array
            }
        }
    }
}

bool BuildSyntaxTreeFromFile(const char* filename,
                             const char* configFileName /*optional*/,
                             struct Options* options,
                             struct SyntaxTree* pSyntaxTree)
{
    bool bResult = false;

    struct Parser parser;


    if (configFileName != NULL)
    {
        //opcional   
        char* /*@auto*/ fullConfigFilePath = NULL;
        GetFullPath(configFileName, &fullConfigFilePath);

        Parser_InitFile(&parser, fullConfigFilePath);
        Parser_Main(&parser, &pSyntaxTree->Declarations);

        //apaga declaracoes eof por ex
        TDeclarations_Destroy(&pSyntaxTree->Declarations);
        TDeclarations_Init(&pSyntaxTree->Declarations);

        //Some com o arquivo de configclea
        TScannerItemList_Clear(&parser.ClueList);
        BasicScannerStack_Pop(&parser.Scanner.stack);
        //Some com o arquivo de config
        Free(fullConfigFilePath);
    }

    char* /*@auto*/ fullFileNamePath = NULL;
    GetFullPath(filename, &fullFileNamePath);

    parser.Scanner.pOptions = options; //TODO

    //Copy parser options
    parser.ParserOptions.bNoImplicitTag = options->bNoImplicitTag;

    if (filename != NULL)
    {
        if (configFileName == NULL)
        {
            Parser_InitFile(&parser, fullFileNamePath);
        }
        else
        {
            Parser_PushFile(&parser, fullFileNamePath);
        }
        Parser_Main(&parser, &pSyntaxTree->Declarations);
    }


    //all sources...
    if (options->bAmalgamate)
    {
        struct FileNodeMap map = { 0 };

        //Inserts the initial file
        FileNodeMap_Insert(&map, FileNode_Create(fullFileNamePath));


        for (;;)
        {


            struct FileNode* pSources = parser.Scanner.Sources.pHead;
            parser.Scanner.Sources.pHead = NULL;
            parser.Scanner.Sources.pTail = NULL;

            struct FileNode* pCurrent = pSources;
            while (pCurrent)
            {
                struct FileNode* pNext = pCurrent->pNext;
                if (FileNodeMap_Lookup(&map, pCurrent->Key) == 0)
                {
                    //processar

                    //inserar na lista ja processados          
                    FileNodeMap_Insert(&map, pCurrent);

                    TScannerItemList_Destroy(&parser.ClueList);
                    TScannerItemList_Init(&parser.ClueList);

                    struct ScannerItem* pNew = ScannerItem_Create();
                    pNew->token = TK_PRE_INCLUDE;
                    LocalStrBuilder_Append(&pNew->lexeme, "source");
                    TScannerItemList_PushBack(&parser.ClueList, pNew);


                    Parser_PushFile(&parser, pCurrent->Key);

                    printf("source %s\n", pCurrent->Key);


                    Parser_Main(&parser, &pSyntaxTree->Declarations);
                }
                else
                {
                    FileNode_Free(pCurrent); //nao pode deletar os proximos
                }

                pCurrent = pNext;
            }
            if (parser.Scanner.Sources.pHead == NULL)
                break;
        }
        FileNodeMap_Destroy(&map);
    }


    TFileMapToStrArray(&parser.Scanner.FilesIncluded, &pSyntaxTree->Files2);
    printf("%s\n", GetCompletationMessage(&parser));
    SymbolMap_Swap(&parser.GlobalScope, &pSyntaxTree->GlobalScope);

    if (Parser_HasError(&parser))
    {
        Scanner_PrintDebug(&parser.Scanner);
    }

    MacroMap_Swap(&parser.Scanner.Defines2, &pSyntaxTree->Defines);

    bResult = !Parser_HasError(&parser);

    Parser_Destroy(&parser);
    Free(fullFileNamePath);

    return bResult;
}



bool BuildSyntaxTreeFromString(const char* sourceCode,
                               struct Options* options,
                               struct SyntaxTree* pSyntaxTree)
{
    bool bResult = false;

    struct Parser parser;

    Parser_InitString(&parser, "source", sourceCode);

    parser.ParserOptions.bNoImplicitTag = options->bNoImplicitTag;

    Parser_Main(&parser, &pSyntaxTree->Declarations);


    TFileMapToStrArray(&parser.Scanner.FilesIncluded, &pSyntaxTree->Files2);
    printf("%s\n", GetCompletationMessage(&parser));
    SymbolMap_Swap(&parser.GlobalScope, &pSyntaxTree->GlobalScope);

    if (Parser_HasError(&parser))
    {
        Scanner_PrintDebug(&parser.Scanner);
    }

    MacroMap_Swap(&parser.Scanner.Defines2, &pSyntaxTree->Defines);

    bResult = !Parser_HasError(&parser);

    Parser_Destroy(&parser);

    return bResult;
}

