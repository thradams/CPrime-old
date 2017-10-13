#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "Scanner.h"
#include "Path.h"
#include <stdarg.h>
//Define to include modications

void Declarator(Parser* ctx, bool bAbstract, TDeclarator** ppTDeclarator2);

#define LANGUAGE_EXTENSIONS 
Tokens Parser_Match(Parser* parser, TScannerItemList* listOpt);
Tokens Parser_MatchToken(Parser* parser,
    Tokens tk,
    TScannerItemList* listOpt);
bool TTypeSpecifier_IsFirst(Parser* ctx, Tokens token, const char* lexeme);
//static bool Is_Type_QualifierFirst(Tokens token);
void Specifier_Qualifier_List(Parser* ctx, TSpecifierQualifierList* pSpecifierQualifierList);
static bool TTypeQualifier_IsFirst(Tokens token);

void TemplateTypeSpecifierArgumentList(Parser* ctx,
    TTemplateTypeSpecifierArgumentList* pList);

static bool IsPreprocessorTokenPhase(Tokens token)
{
    return
        token == TK_SPACES ||
        token == TK_COMMENT ||
        token == TK_LINE_COMMENT ||
        token == TK_BREAKLINE ||
        token == TK_BACKSLASHBREAKLINE ||
        //Tokens para linhas do pre processador
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




bool Declaration(Parser* ctx, TAnyDeclaration** ppDeclaration);

bool IsTypeName(Parser* ctx, Tokens token, const char * lexeme);



Result Parser_InitString(Parser* parser,
    const char* name,
    const char* text)
{
    List_Init(&parser->ClueList);

    ///////
    SymbolMap_Init(&parser->GlobalScope);
    parser->pCurrentScope = &parser->GlobalScope;
    parser->bPreprocessorEvalFlag = false;
    /////////

    //Map_Init(&parser->TypeDefNames, SYMBOL_BUCKETS_SIZE);
    parser->bError = false;

    StrBuilder_Init(&parser->ErrorMessage, 100);


    /////////
    Scanner_InitString(&parser->Scanner, name, text);

    //sair do BOF
    TScannerItemList clueList0 = TSCANNERITEMLIST_INIT;
    Parser_Match(parser, &clueList0);
    TScannerItemList_Destroy(&clueList0);

    return RESULT_OK;
}

Result Parser_InitFile(Parser* parser, const char* fileName)
{



    parser->bPreprocessorEvalFlag = false;
    List_Init(&parser->ClueList);

    /////
    SymbolMap_Init(&parser->GlobalScope);
    parser->pCurrentScope = &parser->GlobalScope;

    //////
    //Map_Init(&parser->TypeDefNames, SYMBOL_BUCKETS_SIZE);
    parser->bError = false;


    StrBuilder_Init(&parser->ErrorMessage, 100);
    Scanner_Init(&parser->Scanner);


    ////////
    Scanner_IncludeFile(&parser->Scanner, fileName, FileIncludeTypeFullPath, false);

    //Scanner_Match(&parser->Scanner);
    //sair do BOF
    TScannerItemList clueList0 = TSCANNERITEMLIST_INIT;
    Parser_Match(parser, &clueList0);
    TScannerItemList_Destroy(&clueList0);

    return RESULT_OK;
}

void Parser_PushFile(Parser* parser, const char* fileName)
{
    Scanner_IncludeFile(&parser->Scanner, fileName, FileIncludeTypeFullPath, false);


    TScannerItemList clueList0 = TSCANNERITEMLIST_INIT;
    Parser_Match(parser, &clueList0);
    ASSERT(clueList0.pHead == NULL);
    TScannerItemList_Destroy(&clueList0);

}





void Parser_Destroy(Parser* parser)
{


    List_Destroy(ScannerItem, &parser->ClueList);

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

bool Parser_HasError(Parser* pParser)
{
    return pParser->bError || pParser->Scanner.bError;
}



void SetError(Parser* parser, const char* fmt, ...)
{
    //    ASSERT(false);
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


int GetCurrentLine(Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return -1;
    }

    return Scanner_LineAt(&parser->Scanner, 0);
}

int GetFileIndex(Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return -1;
    }

    return Scanner_FileIndexAt(&parser->Scanner, 0);
}

static void GetPosition(Parser* ctx, TPosition* pPosition)
{
    pPosition->Line = GetCurrentLine(ctx);
    pPosition->FileIndex = GetFileIndex(ctx);
    //ASSERT(pPosition->FileIndex >= 0);
}

Tokens Parser_LookAheadToken(Parser* parser)
{
    Tokens token = TK_ERROR;

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


const char* Parser_LookAheadLexeme(Parser* parser)
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
            Tokens token = Scanner_TokenAt(&parser->Scanner, i);
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


Tokens Parser_CurrentToken(Parser* parser)
{
    if (Parser_HasError(parser))
    {
        return TK_ERROR;
    }

    Tokens token = Scanner_TokenAt(&parser->Scanner, 0);

    if (IsPreprocessorTokenPhase(token))
    {        
        SetError(parser, "!IsPreprocessorTokenPhase");
    }
    return token;
}


Tokens Parser_Match(Parser* parser, TScannerItemList* listOpt)
{
    //ASSERT(listOpt != NULL);
    Tokens token = TK_EOF;
    if (!Parser_HasError(parser))
    {

        if (listOpt)
        {
            List_Swap(ScannerItem, listOpt, &parser->ClueList);
        }

        Scanner_Match(&parser->Scanner);

        token = Scanner_TokenAt(&parser->Scanner, 0);
        while (token != TK_EOF &&
               token != TK_NONE &&
            (!Scanner_IsActiveAt(&parser->Scanner, 0) ||
                IsPreprocessorTokenPhase(token)))
        {
            ScannerItem* pNew = ScannerItem_Create();
            StrBuilder_Set(&pNew->lexeme, Scanner_LexemeAt(&parser->Scanner, 0));
            pNew->token = Scanner_TokenAt(&parser->Scanner, 0);
            List_Add(&parser->ClueList, pNew);

            Scanner_Match(&parser->Scanner);
            token = Scanner_TokenAt(&parser->Scanner, 0);
        }
    }

    return token;
}

Tokens Parser_MatchToken(Parser* parser,
    Tokens tk,
    TScannerItemList* listOpt)
{
    if (Parser_HasError(parser))
    {
        return TK_EOF;
    }

    Tokens currentToken = Parser_CurrentToken(parser);

    if (tk != currentToken)
    {
        SetError(parser, "Unexpected token - %s", TokenToString(tk));
        return TK_EOF;
    }

    Parser_Match(parser, listOpt);
    return Parser_CurrentToken(parser);
}

const char* GetCompletationMessage(Parser* parser)
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

const char* Lexeme(Parser* parser)
{
    if (Parser_HasError(parser))
    {
        ASSERT(false);
        return "";
    }

    return Scanner_LexemeAt(&parser->Scanner, 0);
}

bool ErrorOrEof(Parser* parser)
{
    return Parser_HasError(parser) ||
        Parser_CurrentToken(parser) == TK_EOF;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


void Expression0(Parser* ctx, TExpression**);
void CastExpression(Parser* ctx, TExpression**);
void GenericSelection(Parser* ctx);
void ArgumentExpressionList(Parser* ctx, TExpression**);
void AssignmentExpression(Parser* ctx, TExpression**);
void Initializer_List(Parser* ctx, TInitializerList* pInitializerList);



bool IsFirstOfPrimaryExpression(Tokens token)
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

        //desde que nao seja cast
    case TK__GENERIC:
        bResult = true;
        break;

    default:
        break;
    }

    return bResult;
}

void PrimaryExpression(Parser* ctx, TExpression** ppPrimaryExpression)
{
    *ppPrimaryExpression = NULL;



    /*
    (6.5.1) primary-expression:
    identifier
    constant
    string-literal
    ( expression )
    generic-selection
    */
    *ppPrimaryExpression = NULL; //out

    Tokens token = Parser_CurrentToken(ctx);
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
    case TK_STRING_LITERAL:
    {
        TPrimaryExpressionLiteral *  pPrimaryExpressionLiteral
            = TPrimaryExpressionLiteral_Create();

        *ppPrimaryExpression = (TExpression*)pPrimaryExpressionLiteral;

        while (token == TK_STRING_LITERAL)
        {
            TPrimaryExpressionLiteralItem *  pPrimaryExpressionLiteralItem
                = TPrimaryExpressionLiteralItem_Create();
            const char* lexeme2 = Lexeme(ctx);
            String_Set(&pPrimaryExpressionLiteralItem->lexeme, lexeme2);

            token = Parser_Match(ctx,
                &pPrimaryExpressionLiteralItem->ClueList0);

            List_Add(&pPrimaryExpressionLiteral->List, pPrimaryExpressionLiteralItem);
        }

    }
    break;

    case TK_IDENTIFIER:
    {
        if (SymbolMap_Find(ctx->pCurrentScope, lexeme) == NULL)
        {
            if (!ctx->bPreprocessorEvalFlag)
            {
                printf("%s ? line %d\n", lexeme, GetCurrentLine(ctx));
                //SetError2(ctx, "not found", "");
                //para o parser nao achou..mas para define X eh nromal nao achar
                
            }
        }

        TPrimaryExpressionValue *   pPrimaryExpressionValue
            = TPrimaryExpressionValue_Create();

        pPrimaryExpressionValue->token = token;
        String_Set(&pPrimaryExpressionValue->lexeme, lexeme);


        Parser_Match(ctx,
            &pPrimaryExpressionValue->ClueList0);
        *ppPrimaryExpression = (TExpression*)pPrimaryExpressionValue;
    }
    break;

    case TK_CHAR_LITERAL:
    case TK_DECIMAL_INTEGER:
    case TK_HEX_INTEGER:
    case TK_FLOAT_NUMBER:
    {
        TPrimaryExpressionValue *   pPrimaryExpressionValue
            = TPrimaryExpressionValue_Create();

        pPrimaryExpressionValue->token = token;
        String_Set(&pPrimaryExpressionValue->lexeme, Lexeme(ctx));


        Parser_Match(ctx,
            &pPrimaryExpressionValue->ClueList0);
        *ppPrimaryExpression = (TExpression*)pPrimaryExpressionValue;
    }
    break;

    case TK_LEFT_PARENTHESIS:
    {
        TPrimaryExpressionValue *   pPrimaryExpressionValue
            = TPrimaryExpressionValue_Create();

        Parser_Match(ctx,
            &pPrimaryExpressionValue->ClueList0);

        TExpression* pExpression;
        Expression0(ctx, &pExpression);

        //    //TNodeClueList_MoveToEnd(&pPrimaryExpressionValue->ClueList, &ctx->Scanner.ClueList);
        Parser_MatchToken(ctx,
            TK_RIGHT_PARENTHESIS,
            &pPrimaryExpressionValue->ClueList1);


        pPrimaryExpressionValue->token = token;
        String_Set(&pPrimaryExpressionValue->lexeme, Lexeme(ctx));
        pPrimaryExpressionValue->pExpressionOpt = pExpression;


        *ppPrimaryExpression = (TExpression*)pPrimaryExpressionValue;
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

void GenericSelection(Parser* ctx)
{
    //_Generic

    /*
    (6.5.1.1) generic-selection:
    _Generic ( assignment-expression , generic-assoc-list )
    */
}

void GenericAssocList(Parser* ctx)
{

    //type-name default

    /*
    (6.5.1.1) generic-assoc-list:
    generic-association
    generic-assoc-list , generic-association
    */
}

void GenericAssociation(Parser* ctx)
{
    //type-name default

    /*
    (6.5.1.1) generic-association:
    type-name : assignment-expression
    default : assignment-expression
    */
}

void TTypeName_Swap(TTypeName* a, TTypeName* b)
{
    TTypeName temp = *a;
    *a = *b;
    *b = temp;
}

void TTypeName_Destroy(TTypeName* p)
{
    TDeclarator_Destroy(&p->Declarator);
    TSpecifierQualifierList_Destroy(&p->SpecifierQualifierList);
}

void TypeName(Parser* ctx, TTypeName* pTypeName)
{
    /*
    type-name:
    specifier-qualifier-list abstract-declaratoropt
    */

    Specifier_Qualifier_List(ctx, &pTypeName->SpecifierQualifierList);

    TDeclarator* pDeclarator = NULL;
    Declarator(ctx, true, &pDeclarator);
    if (pDeclarator)
    {
        pTypeName->Declarator = *pDeclarator;
    }
}


static void PostfixExpressionCore(Parser* ctx, TPostfixExpressionCore* pPostfixExpressionCore)
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

    Tokens token = Parser_CurrentToken(ctx);


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

        String_Set(&pPostfixExpressionCore->Identifier, Lexeme(ctx));

        Parser_MatchToken(ctx, TK_IDENTIFIER,
            &pPostfixExpressionCore->ClueList1);
    }
    break;

    case TK_ARROW:
    {
        // postfix-expression -> identifier
        pPostfixExpressionCore->token = token;

        Parser_Match(ctx, &pPostfixExpressionCore->ClueList0);

        String_Set(&pPostfixExpressionCore->Identifier, Lexeme(ctx));

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
        // ASSERT(false);
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
        TPostfixExpressionCore *  pPostfixExpressionCoreNext =
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
        ASSERT(false);
        break;
    }


}

void PostfixExpression(Parser* ctx, TExpression** ppExpression)
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

    Tokens token = Parser_CurrentToken(ctx);



    if (token == TK_LEFT_PARENTHESIS)
    {
        const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
        Tokens lookAheadToken = Parser_LookAheadToken(ctx);

        if (IsTypeName(ctx, lookAheadToken, lookAheadlexeme))
        {
            // ( type-name ) { initializer-list }

            TPostfixExpressionCore *  pTPostfixExpressionCore =
                TPostfixExpressionCore_Create();

            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pTPostfixExpressionCore->ClueList0);

            TTypeName typeName = TTYPENAME_INIT;
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

            *ppExpression = (TExpression*)pTPostfixExpressionCore;
        }

        else
        {
            //primary-expression
            TExpression* pPrimaryExpression;
            PrimaryExpression(ctx, &pPrimaryExpression);
            *ppExpression = pPrimaryExpression;
        }
    }
    else
    {
        //tem que ser?
        ASSERT(IsFirstOfPrimaryExpression(token));

        //primary-expression
        TExpression* pPrimaryExpression;
        PrimaryExpression(ctx, &pPrimaryExpression);
        *ppExpression = pPrimaryExpression;
    }

    token = Parser_CurrentToken(ctx);
    if (IsFirstOfPrimaryExpression(token))
    {

        // ASSERT(false); //pergunta deve continuar? ta certo?

        TPostfixExpressionCore *  pPostfixExpressionCore =
            TPostfixExpressionCore_Create();
        pPostfixExpressionCore->pExpressionLeft = *ppExpression;
        PostfixExpressionCore(ctx, pPostfixExpressionCore);
        *ppExpression = (TExpression*)pPostfixExpressionCore;
    }
    else
    {
        switch (token)
        {
        case TK_LEFT_PARENTHESIS:
        {
            TPostfixExpressionCore *  pPostfixExpressionCore =
                TPostfixExpressionCore_Create();
            pPostfixExpressionCore->pExpressionLeft = *ppExpression;

            String lexemeCopy = STRING_INIT;

            TPrimaryExpressionValue* ppri =
                TExpression_As_TPrimaryExpressionValue(*ppExpression);
            if (ppri)
            {
                String_Set(&lexemeCopy, ppri->lexeme);
            }
            PostfixExpressionCore(ctx, pPostfixExpressionCore);
            *ppExpression = (TExpression*)pPostfixExpressionCore;



            String_Destroy(&lexemeCopy);
        }
        break;
        case TK_LEFT_SQUARE_BRACKET:
        case TK_FULL_STOP:
        case TK_ARROW:
        case TK_PLUSPLUS:
        case TK_MINUSMINUS:
        {
            TPostfixExpressionCore *  pPostfixExpressionCore =
                TPostfixExpressionCore_Create();
            pPostfixExpressionCore->pExpressionLeft = *ppExpression;
            PostfixExpressionCore(ctx, pPostfixExpressionCore);
            *ppExpression = (TExpression*)pPostfixExpressionCore;
        }
        break;
        default:
            ASSERT(false);
            break;
        }

    }

}

void ArgumentExpressionList(Parser* ctx, TExpression** ppExpression)
{
    /*(6.5.2) argument-expression-list:
    assignment-expression
    argument-expression-list , assignment-expression
    */
    TExpression* pAssignmentExpression;
    AssignmentExpression(ctx, &pAssignmentExpression);
    *ppExpression = pAssignmentExpression;

    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        TBinaryExpression *  pExpr =
            TBinaryExpression_Create();

        Parser_Match(ctx, &pExpr->ClueList00);// &pAssignmentExpression->ClueList);

        TExpression* pAssignmentExpressionRight;
        AssignmentExpression(ctx, &pAssignmentExpressionRight);


        GetPosition(ctx, &pExpr->Position);
        pExpr->token = TK_COMMA;
        pExpr->pExpressionLeft = pAssignmentExpression;
        pExpr->pExpressionRight = pAssignmentExpressionRight;



        *ppExpression = (TExpression*)pExpr;
    }

    token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        TBinaryExpression *  pExpr =
            TBinaryExpression_Create();

        Parser_Match(ctx, &pExpr->ClueList00);

        GetPosition(ctx, &pExpr->Position);
        pExpr->token = TK_COMMA;
        pExpr->pExpressionLeft = *ppExpression;

        TExpression *pExpressionRight;
        ArgumentExpressionList(ctx, &pExpressionRight);
        pExpr->pExpressionRight = pExpressionRight;

        *ppExpression = (TExpression*)pExpr;
    }
}



static bool IsTypeQualifierToken(Tokens token)
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

    case TK__AUTO:

    case TK_OPT_QUALIFIER:
    case TK_OWN_QUALIFIER:
    case TK_DTOR_QUALIFIER:
    case TK_MDTOR_QUALIFIER:
        bResult = true;
        break;
#endif
    default:
        ASSERT(false);
        break;
    }
    return bResult;
}

bool IsTypeName(Parser* ctx, Tokens token, const char * lexeme)
{

    bool bResult = false;

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
    case TK__AUTO:
        //type-qualifier-extensions 
    case TK_OPT_QUALIFIER:
    case TK_OWN_QUALIFIER:
    case TK_DTOR_QUALIFIER:
    case TK_MDTOR_QUALIFIER:
#endif

        //type-specifier
    case TK_VOID:
    case TK_char:
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
        ASSERT(false);
        break;
    }

    return bResult;
}

void UnaryExpression(Parser* ctx, TExpression** ppExpression)
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

    Tokens token0 = Parser_CurrentToken(ctx);
    Tokens tokenAhead = Parser_LookAheadToken(ctx);
    const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);

    if (IsTypeName(ctx, tokenAhead, lookAheadlexeme))
    {
        //first para postfix-expression
        TExpression * pPostfixExpression;
        PostfixExpression(ctx, &pPostfixExpression);
        *ppExpression = (TExpression*)(pPostfixExpression);
        return;
    }

    else if (IsFirstOfPrimaryExpression(token0))
    {
        //primary-expression é first para postfix-expression

        TExpression * pPostfixExpression;
        PostfixExpression(ctx, &pPostfixExpression);
        *ppExpression = (TExpression*)(pPostfixExpression);
        return;
    }

    switch (token0)
    {
    case TK_PLUSPLUS:
    case TK_MINUSMINUS:
    {
        TUnaryExpressionOperator* pUnaryExpressionOperator =
            TUnaryExpressionOperator_Create();

        Parser_Match(ctx, &pUnaryExpressionOperator->ClueList00);

        TExpression *pUnaryExpression;
        UnaryExpression(ctx, &pUnaryExpression);


        pUnaryExpressionOperator->token = token0;
        pUnaryExpressionOperator->pExpressionRight = pUnaryExpression;
        *ppExpression = (TExpression*)pUnaryExpressionOperator;
    }
    break;

    //unary-operator cast-expression
    case TK_AMPERSAND:
    case TK_ASTERISK:
    case TK_PLUS_SIGN:
    case TK_HYPHEN_MINUS:
    case TK_TILDE:
    case TK_EXCLAMATION_MARK:

#ifdef LANGUAGE_EXTENSIONS
        //unary-operator-extension
    case TK_MOVE:
#endif
        //
    {
        TUnaryExpressionOperator* pUnaryExpressionOperator =
            TUnaryExpressionOperator_Create();

        Parser_Match(ctx, &pUnaryExpressionOperator->ClueList00);
        TExpression* pCastExpression;
        CastExpression(ctx, &pCastExpression);



        pUnaryExpressionOperator->token = token0;
        pUnaryExpressionOperator->pExpressionRight = pCastExpression;
        *ppExpression = (TExpression*)pUnaryExpressionOperator;

    }
    break;

    //////////////

    case TK_SIZEOF:
    {
        TUnaryExpressionOperator* pUnaryExpressionOperator =
            TUnaryExpressionOperator_Create();
        *ppExpression = (TExpression*)pUnaryExpressionOperator;

        pUnaryExpressionOperator->token = token0;

        Parser_MatchToken(ctx, TK_SIZEOF, &pUnaryExpressionOperator->ClueList00);

        if (Parser_CurrentToken(ctx) == TK_LEFT_PARENTHESIS)
        {
            const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
            Tokens lookAheadToken = Parser_LookAheadToken(ctx);

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
                TExpression *pTUnaryExpression;
                UnaryExpression(ctx, &pTUnaryExpression);
                pUnaryExpressionOperator->pExpressionRight = pTUnaryExpression;
            }
        }
        else
        {
            //sizeof do tipo desta expressao
            TExpression *pTUnaryExpression;
            UnaryExpression(ctx, &pTUnaryExpression);
            pUnaryExpressionOperator->pExpressionRight = pTUnaryExpression;
        }
    }
    break;

    case TK__ALINGOF:
        //Match
        ASSERT(false);
        break;
    case TK_EOF:
        break;
        //TODO ver tudo que pode ser follow
    default:
        //ASSERT(false);
//        SetUnexpectedError(ctx, "Assert", "");
        //aqui nao eh erro necessariamente
        break;
    }

}

void CastExpression(Parser* ctx, TExpression** ppExpression)
{
    *ppExpression = NULL; //out

    /*
    cast-expression:
    unary-expression
    ( type-name ) cast-expression
    */

    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_LEFT_PARENTHESIS)
    {
        const char* lookAheadlexeme = Parser_LookAheadLexeme(ctx);
        Tokens lookAheadToken = Parser_LookAheadToken(ctx);

        if (IsTypeName(ctx, lookAheadToken, lookAheadlexeme))
        {
            TScannerItemList tempList0 = TSCANNERITEMLIST_INIT;
            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &tempList0);

            TTypeName typeName = TTYPENAME_INIT;
            TypeName(ctx, &typeName);

            TScannerItemList tempList1 = TSCANNERITEMLIST_INIT;
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

                TPostfixExpressionCore*  pTPostfixExpressionCore =
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
                *ppExpression = (TExpression*)pTPostfixExpressionCore;
                //PostfixExpressionCore(ctx, pTPostfixExpressionCore);
            }
            else
            {
                TCastExpressionType *pCastExpressionType =
                    TCastExpressionType_Create();

                TScannerItemList_Swap(&tempList0, &pCastExpressionType->ClueList0);
                TScannerItemList_Swap(&tempList1, &pCastExpressionType->ClueList1);

                TExpression* pCastExpression;
                CastExpression(ctx, &pCastExpression);

                TTypeName_Swap(&pCastExpressionType->TypeName, &typeName);
                pCastExpressionType->pExpression = pCastExpression;
                *ppExpression = (TExpression*)pCastExpressionType;

            }

            TTypeName_Destroy(&typeName);
            TScannerItemList_Destroy(&tempList0);
            TScannerItemList_Destroy(&tempList1);
        }
        else
        {
            TExpression* pUnaryExpression;
            UnaryExpression(ctx, &pUnaryExpression);
            *ppExpression = pUnaryExpression;
        }
    }
    else
    {
        TExpression* pUnaryExpression;
        UnaryExpression(ctx, &pUnaryExpression);

        *ppExpression = pUnaryExpression;
    }
}


void MultiplicativeExpression(Parser* ctx, TExpression** ppExpression)
{


    /*
    (6.5.5) multiplicative-expression:
    cast-expression                                // identifier  constant  string-literal  (  _Generic ++	  -- 	 & * + - ~ ! 		 sizeof 		 sizeof		 alignof
    multiplicative-expression * cast-expression
    multiplicative-expression / cast-expression
    multiplicative-expression % cast-expression
    */

    TExpression* pExpressionLeft;
    CastExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_PERCENT_SIGN:
    case TK_SOLIDUS:
    case TK_ASTERISK:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();

        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;

        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        CastExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_PERCENT_SIGN:
    case TK_SOLIDUS:
    case TK_ASTERISK:
    {
        TBinaryExpression *pBinaryExpression =
            TBinaryExpression_Create();

        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        MultiplicativeExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void AdditiveExpression(Parser* ctx, TExpression** ppExpression)
{

    /*
    (6.5.6) additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
    */

    TExpression* pExpressionLeft;
    MultiplicativeExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_PLUS_SIGN:
    case TK_HYPHEN_MINUS:
    {
        TBinaryExpression *pBinaryExpression =
            TBinaryExpression_Create();
        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        MultiplicativeExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_PLUS_SIGN:
    case TK_HYPHEN_MINUS:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        AdditiveExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

}

void ShiftExpression(Parser* ctx, TExpression** ppExpression)
{

    /*(6.5.7) shift-expression:
    additive-expression
    shift-expression << additive-expression
    shift-expression >> additive-expression
    */


    TExpression* pExpressionLeft;
    AdditiveExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_GREATERGREATER:
    case TK_LESSLESS:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        AdditiveExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_GREATERGREATER:
    case TK_LESSLESS:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        ShiftExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void RelationalExpression(Parser* ctx, TExpression** ppExpression)
{
    /*
    (6.5.8) relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
    */

    TExpression* pExpressionLeft;
    ShiftExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_LESS_THAN_SIGN:
    case TK_GREATER_THAN_SIGN:
    case TK_GREATEREQUAL:
    case TK_LESSEQUAL:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        ShiftExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
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
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        RelationalExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

}

void EqualityExpression(Parser* ctx, TExpression** ppExpression)
{
    /*(6.5.9) equality-expression:
    relational-expression
    equality-expression == relational-expression
    equality-expression != relational-expression
    */

    TExpression* pExpressionLeft;
    RelationalExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_EQUALEQUAL:
    case TK_NOTEQUAL:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        RelationalExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_EQUALEQUAL:
    case TK_NOTEQUAL:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        EqualityExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void AndExpression(Parser* ctx, TExpression **ppExpression)
{
    /*(6.5.10) AND-expression:
    equality-expression
    AND-expression & equality-expression
    */

    TExpression* pExpressionLeft;
    EqualityExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_AMPERSAND:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        EqualityExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_AMPERSAND:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        AndExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void ExclusiveOrExpression(Parser* ctx, TExpression** ppExpression)
{
    /*
    (6.5.11) exclusive-OR-expression:
    AND-expression
    exclusive-OR-expression ^ AND-expression
    */

    TExpression* pExpressionLeft;
    AndExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_CIRCUMFLEX_ACCENT:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        AndExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_CIRCUMFLEX_ACCENT:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);


        TExpression* pExpressionRight;
        ExclusiveOrExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

}

void InclusiveOrExpression(Parser* ctx, TExpression**ppExpression)
{

    /*
    (6.5.12) inclusive-OR-expression:
    exclusive-OR-expression
    inclusive-OR-expression | exclusive-OR-expression
    */

    TExpression* pExpressionLeft;
    ExclusiveOrExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_VERTICAL_LINE:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        ExclusiveOrExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_VERTICAL_LINE:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        InclusiveOrExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void LogicalAndExpression(Parser* ctx, TExpression** ppExpression)
{

    /*
    (6.5.13) logical-AND-expression:
    inclusive-OR-expression
    logical-AND-expression && inclusive-OR-expression
    */

    TExpression* pExpressionLeft;
    InclusiveOrExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_ANDAND:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;
        GetPosition(ctx, &pBinaryExpression->Position);


        Parser_Match(ctx, &pBinaryExpression->ClueList00);


        TExpression* pExpressionRight;
        InclusiveOrExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_ANDAND:
    {
        TBinaryExpression *pBinaryExpression =
            TBinaryExpression_Create();

        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        LogicalAndExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}

void LogicalOrExpression(Parser* ctx, TExpression** ppExpression)
{
    /*(6.5.14) logical-OR-expression:
     logical-AND-expression
     logical-OR-expression || logical-AND-expression
     */


    TExpression* pExpressionLeft;
    LogicalAndExpression(ctx, &pExpressionLeft);
    *ppExpression = pExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);


    switch (token)
    {
    case TK_OROR:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        LogicalAndExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }

    token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_OROR:
    {
        TBinaryExpression *pBinaryExpression = TBinaryExpression_Create();
        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->token = token;
        pBinaryExpression->pExpressionLeft = *ppExpression;


        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        TExpression* pExpressionRight;
        LogicalOrExpression(ctx, &pExpressionRight);
        pBinaryExpression->pExpressionRight = pExpressionRight;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;
    default:
        ASSERT(false);
        break;
    }
}


void ConditionalExpression(Parser* ctx, TExpression**ppExpression)
{
    /*(6.5.15) conditional-expression:
    logical-OR-expression
    logical-OR-expression ? expression : conditional-expression
    */
    TExpression * pLogicalOrExpressionLeft;
    LogicalOrExpression(ctx, &pLogicalOrExpressionLeft);
    *ppExpression = pLogicalOrExpressionLeft;

    if (Parser_CurrentToken(ctx) == TK_QUESTION_MARK)
    {
        TTernaryExpression* pTernaryExpression =
            TTernaryExpression_Create();

        Parser_Match(ctx, &pTernaryExpression->ClueList0);

        TExpression* pTExpression;
        Expression0(ctx, &pTExpression);

        Parser_MatchToken(ctx, TK_COLON, &pTernaryExpression->ClueList1);

        TExpression* pConditionalExpressionRight;
        ConditionalExpression(ctx, &pConditionalExpressionRight);


        pTernaryExpression->token = TK_QUESTION_MARK;
        pTernaryExpression->pExpressionLeft = pLogicalOrExpressionLeft;
        pTernaryExpression->pExpressionMiddle = pTExpression;
        pTernaryExpression->pExpressionRight = pConditionalExpressionRight;
        *ppExpression = (TExpression*)pTernaryExpression;
    }


}

void AssignmentExpression(Parser* ctx, TExpression** ppExpression)
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
    TExpression* pConditionalExpressionLeft;
    ConditionalExpression(ctx, &pConditionalExpressionLeft);
    *ppExpression = pConditionalExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);

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
        TBinaryExpression *pBinaryExpression =
            TBinaryExpression_Create();

        Parser_Match(ctx, &pBinaryExpression->ClueList00);

        //Significa que o anterior deve ser do tipo  unary-expression
        //embora tenhamos feito o parser de conditional-expression
        //se nao for é erro.
        TExpression* pAssignmentExpressionRight;
        AssignmentExpression(ctx, &pAssignmentExpressionRight);

        GetPosition(ctx, &pBinaryExpression->Position);

        pBinaryExpression->pExpressionLeft = *ppExpression;
        pBinaryExpression->pExpressionRight = pAssignmentExpressionRight;
        pBinaryExpression->token = token;
        *ppExpression = (TExpression*)pBinaryExpression;
    }
    break;

    default:
        //É apenas conditional-expression
        break;
    }
}

void Expression0(Parser* ctx, TExpression** ppExpression)
{
    *ppExpression = NULL; //out
    /*
    (6.5.17) expression:
    assignment-expression
    expression , assignment-expression
    */
    TExpression* pAssignmentExpressionLeft;
    AssignmentExpression(ctx, &pAssignmentExpressionLeft);
    *ppExpression = pAssignmentExpressionLeft;

    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COMMA)
    {
        TExpression* pAssignmentExpressionRight;
        Parser_Match(ctx, NULL);
        Expression0(ctx, &pAssignmentExpressionRight);

        TBinaryExpression* pBinaryExpression =
            TBinaryExpression_Create();
        GetPosition(ctx, &pBinaryExpression->Position);
        pBinaryExpression->pExpressionLeft = *ppExpression;
        pBinaryExpression->pExpressionRight = pAssignmentExpressionRight;
        pBinaryExpression->token = TK_COMMA;

        *ppExpression = (TExpression*)pBinaryExpression;
    }
}

void ConstantExpression(Parser* ctx, TExpression** ppExpression)
{
    *ppExpression = NULL; //out

    /*
    (6.6) constant-expression:
    conditional-expression
    */
    ConditionalExpression(ctx, ppExpression);
}


///////////////////////////////////////////////////////////////////////////////


void Designator(Parser* ctx, TDesignator* pDesignator);
void Designator_List(Parser* ctx, TDesignatorList* pDesignatorList);
void Designation(Parser* ctx, TDesignatorList* pDesignatorList);
void Initializer_List(Parser* ctx, TInitializerList* pInitializerList);
//void Assignment_Expression(Parser* ctx, TExpressionStatement* pExpressionStatement, Tokens endToken1, Tokens endToken2);
bool Statement(Parser* ctx, TStatement** ppStatement);
//void Constant_Expression(Parser* ctx, Tokens endToken1, Tokens endToken2, ScannerItemStack* outStack);
void Compound_Statement(Parser* ctx, TStatement** ppStatement);
//void Expression(Parser* ctx, TExpression** pExpression, Tokens endToken1, Tokens endToken2);
void Parameter_Declaration(Parser* ctx, TParameter* pParameterDeclaration);
bool Declaration(Parser* ctx, TAnyDeclaration** ppDeclaration);
bool Type_Qualifier_ListOpt(Parser* ctx, TTypeQualifierList* pQualifiers);
void Declaration_Specifiers(Parser* ctx, TDeclarationSpecifiers* pDeclarationSpecifiers);

//void Declarator(Parser* ctx, bool bAbstract, TDeclarator** pTDeclarator2);
void Type_Specifier(Parser* ctx, TTypeSpecifier** ppTypeSpecifier);
bool Type_Qualifier(Parser* ctx, TTypeQualifier* pQualifier);
void Initializer(Parser* ctx,
    TInitializer** ppInitializer,
    Tokens endToken1,
    Tokens endToken2);


void Expression_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    expression-statement:
    expressionopt;
    */
    TExpressionStatement* pExpression = TExpressionStatement_Create();
    *ppStatement = (TStatement*)pExpression;
    Tokens token = Parser_CurrentToken(ctx);

    if (token != TK_SEMICOLON)
    {
        Expression0(ctx, &pExpression->pExpression);
    }

    Parser_MatchToken(ctx, TK_SEMICOLON, &pExpression->ClueList0);
}

void Selection_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    selection-statement:
    if ( expression ) statement
    if ( expression ) statement else statement
    switch ( expression ) statement
    */
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_IF:
    {
        TIfStatement* pIfStatement = TIfStatement_Create();
        *ppStatement = (TStatement*)pIfStatement;

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
        TSwitchStatement* pSelectionStatement = TSwitchStatement_Create();
        *ppStatement = (TStatement*)pSelectionStatement;


        Parser_Match(ctx, &pSelectionStatement->ClueList0);


        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pSelectionStatement->ClueList1);

        Expression0(ctx, &pSelectionStatement->pConditionExpression);


        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pSelectionStatement->ClueList2);

        Statement(ctx, &pSelectionStatement->pExpression);
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

void Jump_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    jump-statement:
    goto identifier ;
    continue ;
    break ;
    return expressionopt ;
    */
    //jump-statement
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_GOTO:
    {
        TJumpStatement* pJumpStatement = TJumpStatement_Create();
        pJumpStatement->token = token;
        *ppStatement = (TStatement*)pJumpStatement;

        Parser_Match(ctx, &pJumpStatement->ClueList0);
        String_Set(&pJumpStatement->Identifier, Lexeme(ctx));
        Parser_MatchToken(ctx, TK_IDENTIFIER, &pJumpStatement->ClueList1);
        Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
    }
    break;

    case TK_CONTINUE:
    {
        TJumpStatement* pJumpStatement = TJumpStatement_Create();
        pJumpStatement->token = token;
        *ppStatement = (TStatement*)pJumpStatement;
        Parser_Match(ctx, &pJumpStatement->ClueList0);

        Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
    }
    break;

    case TK_BREAK:
    {
        TJumpStatement* pJumpStatement = TJumpStatement_Create();
        pJumpStatement->token = token;
        *ppStatement = (TStatement*)pJumpStatement;

        Parser_Match(ctx, &pJumpStatement->ClueList0);
        Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
    }
    break;

    case TK_RETURN:
    {
        TJumpStatement* pJumpStatement = TJumpStatement_Create();
        pJumpStatement->token = token;
        *ppStatement = (TStatement*)pJumpStatement;

        token = Parser_Match(ctx, &pJumpStatement->ClueList0);

        if (token != TK_SEMICOLON)
        {
            Expression0(ctx, &pJumpStatement->pExpression);
        }

        Parser_MatchToken(ctx, TK_SEMICOLON, &pJumpStatement->ClueList2);
    }
    break;

    default:
        ASSERT(false);
        break;
    }
}

void Iteration_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    iteration-statement:
    while ( expression ) statement
    do statement while ( expression ) ;
    for ( expressionopt ; expressionopt ; expressionopt ) statement
    for ( declaration expressionopt ; expressionopt ) statement
    */
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_WHILE:
    {
        TWhileStatement* pWhileStatement = TWhileStatement_Create();
        *ppStatement = (TStatement*)pWhileStatement;

        Parser_Match(ctx, &pWhileStatement->ClueList0);

        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pWhileStatement->ClueList1);

        Expression0(ctx, &pWhileStatement->pExpression);

        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pWhileStatement->ClueList2);

        Statement(ctx, &pWhileStatement->pStatement);
    }
    break;

    case TK_DO:
    {
        TDoStatement* pDoStatement = TDoStatement_Create();
        *ppStatement = (TStatement*)pDoStatement;

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
        TForStatement* pIterationStatement = TForStatement_Create();
        *ppStatement = (TStatement*)pIterationStatement;

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
        ASSERT(false);
        break;
    }
}


void Labeled_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    labeled-statement:
    identifier : statement (ver Labeled_StatementLabel)
    case constant-expression : statement
    default : statement
    */
    TLabeledStatement* pLabeledStatement = TLabeledStatement_Create();
    *ppStatement = (TStatement*)pLabeledStatement;
    Tokens token = Parser_CurrentToken(ctx);
    pLabeledStatement->token = token;

    if (token == TK_IDENTIFIER)
    {
        //aqui nao eh um tipo
        String_Set(&pLabeledStatement->Identifier, Lexeme(ctx));

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

void Asm_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    __asm assembly-instruction ;opt
    __asm { assembly-instruction-list };opt
    */
    TAsmStatement * pAsmStatement = TAsmStatement_Create();
    *ppStatement = (TStatement*)pAsmStatement;
    Parser_MatchToken(ctx, TK__ASM, NULL);
    Tokens token = Parser_CurrentToken(ctx);

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

bool Statement(Parser* ctx, TStatement** ppStatement)
{
    ASSERT(*ppStatement == NULL);

    if (Parser_HasError(ctx))
    {
        return false;
    }

    bool bResult = false;
    Tokens token = Parser_CurrentToken(ctx);
    const char * lexeme = Lexeme(ctx);

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
        //assert(false);
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

#ifdef LANGUAGE_EXTENSIONS
        //type-qualifier-extensions 
        //case TK_AUTO:

    case TK_OPT_QUALIFIER:
    case TK_OWN_QUALIFIER:
    case TK_DTOR_QUALIFIER:
    case TK_MDTOR_QUALIFIER:
        //
#endif

    case TK_TYPEDEF:
    case TK_EXTERN:
    case TK_STATIC:
    case TK__THREAD_LOCAL:
    case TK_AUTO:
    case TK_REGISTER:
    case TK_VOID:
    case TK_char:
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
    case TK__TEMPLATE:
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

void Block_Item(Parser* ctx, TBlockItem** ppBlockItem)
{
    /*
    block-item:
    declaration
    statement
    */
    *ppBlockItem = NULL;
    TStatement* pStatement = NULL;

    if (Statement(ctx, &pStatement))
    {
        *ppBlockItem = (TAnyDeclaration*)pStatement;
        ASSERT(*ppBlockItem != NULL);
    }

    else
    {


        TAnyDeclaration* pDeclaration;
        Declaration(ctx, &pDeclaration);
        *ppBlockItem = (TAnyDeclaration*)pDeclaration;
        ASSERT(*ppBlockItem != NULL);
    }
}

void Block_Item_List(Parser* ctx, TBlockItemList* pBlockItemList)
{
    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        TBlockItem* pBlockItem = NULL;
        Block_Item(ctx, &pBlockItem);
        ArrayT_Push(pBlockItemList, pBlockItem);
        Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_RIGHT_CURLY_BRACKET)
        {
            //terminou
            break;
        }

        if (ErrorOrEof(ctx))
            break;
    }
}

void Compound_Statement(Parser* ctx, TStatement** ppStatement)
{
    /*
    compound-statement:
    { block-item-listopt }
    */
    TCompoundStatement* pCompoundStatement = TCompoundStatement_Create();
    *ppStatement = (TStatement*)pCompoundStatement;

    SymbolMap BlockScope = SYMBOLMAP_INIT;

    BlockScope.pPrevious = ctx->pCurrentScope;
    ctx->pCurrentScope = &BlockScope;

    Parser_MatchToken(ctx, TK_LEFT_CURLY_BRACKET, &pCompoundStatement->ClueList0);

    Tokens token = Parser_CurrentToken(ctx);

    if (token != TK_RIGHT_CURLY_BRACKET)
    {
        Block_Item_List(ctx, &pCompoundStatement->BlockItemList);
    }

    Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET, &pCompoundStatement->ClueList1);

    //SymbolMap_Print(ctx->pCurrentScope);

    ctx->pCurrentScope = BlockScope.pPrevious;
    SymbolMap_Destroy(&BlockScope);
}

void Struct_Or_Union(Parser* ctx,
    TStructUnionSpecifier* pStructUnionSpecifier)
{
    /*
    struct-or-union:
    struct
    union
    */
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_STRUCT:
        pStructUnionSpecifier->bIsStruct = true;
        Parser_Match(ctx, &pStructUnionSpecifier->ClueList0);
        break;

    case TK_UNION:
        pStructUnionSpecifier->bIsStruct = false;
        Parser_Match(ctx, &pStructUnionSpecifier->ClueList0);
        break;

    default:
        ASSERT(false);
        break;
    }
}

void Static_Assert_Declaration(Parser* ctx, TStaticAssertDeclaration* pStaticAssertDeclaration)
{
    /*
    static_assert-declaration:
    _Static_assert ( constant-expression , string-literal ) ;
    */
    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__STATIC_ASSERT)
    {
        Parser_Match(ctx, &pStaticAssertDeclaration->ClueList0);

        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pStaticAssertDeclaration->ClueList1);

        ConstantExpression(ctx,
            &pStaticAssertDeclaration->pConstantExpression);

        Parser_MatchToken(ctx, TK_COMMA, &pStaticAssertDeclaration->ClueList2);

        String_Set(&pStaticAssertDeclaration->Text, Lexeme(ctx));
        Parser_MatchToken(ctx, TK_STRING_LITERAL, &pStaticAssertDeclaration->ClueList3);

        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pStaticAssertDeclaration->ClueList4);

        Parser_MatchToken(ctx, TK_SEMICOLON, &pStaticAssertDeclaration->ClueList5);
    }
}

void Specifier_Qualifier_List(Parser* ctx, TSpecifierQualifierList* pSpecifierQualifierList)
{
    /*specifier-qualifier-list:
    type-specifier specifier-qualifier-listopt
    type-qualifier specifier-qualifier-listopt
    */
    Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);
    if (TTypeSpecifier_IsFirst(ctx, token, lexeme))
    {
        if (TSpecifierQualifierList_CanAdd(pSpecifierQualifierList, token, lexeme))
        {
            TTypeSpecifier* pTypeSpecifier = NULL;
            Type_Specifier(ctx, &pTypeSpecifier);
            if (pTypeSpecifier != NULL)
            {
                List_Add(pSpecifierQualifierList, pTypeSpecifier);
            }
        }
        else
        {
            SetError(ctx, "invalid specifier-qualifier-list");
        }

    }
    else if (TTypeQualifier_IsFirst(token))
    {
        TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
        Type_Qualifier(ctx, pTypeQualifier);
        List_Add(pSpecifierQualifierList, TTypeQualifier_As_TSpecifierQualifier(pTypeQualifier));
    }
    else
    {
        SetError(ctx, "internal error 1");
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

void Struct_Declarator(Parser* ctx,

    TStructDeclarator** ppTDeclarator2)
{
    /**
    struct-declarator:
    declarator
    declaratoropt : constant-expression
    */
    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_COLON)
    {
        //AST TODO
        ////TNodeClueList_MoveToEnd(&ppTDeclarator2->ClueList, &ctx->Scanner.ClueList);

        Parser_Match(ctx, NULL);// &ppTDeclarator2->ClueList);
        TExpression* p = NULL;
        ConstantExpression(ctx, &p);
        TExpression_Delete(p);
    }

    else
    {
        TInitDeclarator* pInitDeclarator =
            TInitDeclarator_Create();

        *ppTDeclarator2 = pInitDeclarator;

        ASSERT(pInitDeclarator->pDeclarator == NULL);
        Declarator(ctx, false, &pInitDeclarator->pDeclarator);

        token = Parser_CurrentToken(ctx);

        if (token == TK_COLON)
        {
            Parser_Match(ctx, &pInitDeclarator->ClueList00);

            TExpression* p = NULL;
            ConstantExpression(ctx, &p);
            TExpression_Delete(p);
        }
#ifdef LANGUAGE_EXTENSIONS
        else if (token == TK_EQUALS_SIGN)
        {
            Parser_Match(ctx, &pInitDeclarator->ClueList1); //_defval ou =
            Initializer(ctx, &pInitDeclarator->pInitializer, TK_SEMICOLON, TK_SEMICOLON);
        }
        else if (token == TK__DEFVAL)
        {
            Parser_Match(ctx, &pInitDeclarator->ClueList1); //_defval
            Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, NULL); //(

            Initializer(ctx, &pInitDeclarator->pInitializer, TK_SEMICOLON, TK_SEMICOLON);

            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL); //)
        }
#endif
    }
}

void Struct_Declarator_List(Parser* ctx,

    TStructDeclaratorList* pStructDeclarationList)
{
    /*
    struct-declarator-list:
    struct-declarator
    struct-declarator-list , struct-declarator
    */
    TStructDeclarator* pTDeclarator2 = NULL;// TDeclarator_Create();

    Struct_Declarator(ctx, &pTDeclarator2);
    List_Add(pStructDeclarationList, pTDeclarator2);

    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_COMMA)
        {
			//Tem mais
            Parser_Match(ctx, &pTDeclarator2->ClueList00);

            Struct_Declarator_List(ctx, pStructDeclarationList);
        }
        else if (token == TK_SEMICOLON)
        {
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

void Struct_Declaration(Parser* ctx,
    TAnyStructDeclaration** ppStructDeclaration)
{
    /**
    struct-declaration:
    specifier-qualifier-list struct-declarator-listopt ;
    static_assert-declaration
    */
    Tokens token = Parser_CurrentToken(ctx);

    if (token != TK__STATIC_ASSERT)
    {
        TStructDeclaration* pStructDeclarationBase = TStructDeclaration_Create();
        *ppStructDeclaration = (TAnyStructDeclaration*)pStructDeclarationBase;
        Specifier_Qualifier_List(ctx,
            &pStructDeclarationBase->SpecifierQualifierList);
        token = Parser_CurrentToken(ctx);

        if (token != TK_SEMICOLON)
        {
            Struct_Declarator_List(ctx,
                &pStructDeclarationBase->DeclaratorList);

            Parser_MatchToken(ctx, TK_SEMICOLON, &pStructDeclarationBase->ClueList1);
        }
        else
        {
            Parser_MatchToken(ctx, TK_SEMICOLON, &pStructDeclarationBase->ClueList1);
        }
    }

    else
    {
        TStaticAssertDeclaration* pStaticAssertDeclaration = TStaticAssertDeclaration_Create();
        *ppStructDeclaration = (TAnyStructDeclaration*)pStaticAssertDeclaration;
        Static_Assert_Declaration(ctx, pStaticAssertDeclaration);
    }
}

void Struct_Declaration_List(Parser* ctx,
    TStructDeclarationList* pStructDeclarationList)
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

    TAnyStructDeclaration* pStructDeclaration = NULL;
    Struct_Declaration(ctx, &pStructDeclaration);
    ArrayT_Push(pStructDeclarationList, pStructDeclaration);

    Tokens token = Parser_CurrentToken(ctx);
    if (token != TK_RIGHT_CURLY_BRACKET)
    {
        //Tem mais?
        Struct_Declaration_List(ctx, pStructDeclarationList);
    }
}

void Struct_Or_Union_Specifier(Parser* ctx,
    TStructUnionSpecifier* pStructUnionSpecifier)
{
    /*
    struct-or-union-specifier:
    struct-or-union identifieropt { struct-declaration-list }
    struct-or-union identifier
    */

    /*
    struct-or-union-specifier:
    struct-or-union { struct-declaration-list }
    struct-or-union identifier

    struct-or-union identifier (argument-list)
    struct-or-union identifier { struct-declaration-list }
    struct-or-union identifier identifier (argument-list)
    */

    Struct_Or_Union(ctx, pStructUnionSpecifier);//TODO


    Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);

    if (token == TK_IDENTIFIER)
    {
        
        String_Set(&pStructUnionSpecifier->Name, lexeme);

        
        Parser_Match(ctx, &pStructUnionSpecifier->ClueList1);
        token = Parser_CurrentToken(ctx);
        if (token == TK_LEFT_PARENTHESIS)
        {
            //na verdade eh nome do template
            String_Swap(&pStructUnionSpecifier->Name, &pStructUnionSpecifier->TemplateName);

            Parser_Match(ctx, NULL);
            TemplateTypeSpecifierArgumentList(ctx,
                (TTemplateTypeSpecifierArgumentList *)&pStructUnionSpecifier->Args);
            Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL);
        }
        else if (token == TK_IDENTIFIER)
        {

            Tokens tokenAhead = Parser_LookAheadToken(ctx);

            //token = Parser_CurrentToken(ctx);
            if (tokenAhead == TK_LEFT_PARENTHESIS)
            {
                String_Set(&pStructUnionSpecifier->TemplateName, Lexeme(ctx));
                Parser_Match(ctx, NULL);

                Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, NULL);

                TemplateTypeSpecifierArgumentList(ctx,
                    (TTemplateTypeSpecifierArgumentList *)&pStructUnionSpecifier->Args);

                Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL);
            }
            else  if (tokenAhead == TK_SEMICOLON)
            {
                //typedef struct X X;
                //                  ^
            }
            else  if (tokenAhead == TK_LEFT_SQUARE_BRACKET)
            {
                //typedef struct X {;
                //                 ^
            }
            else
            {
                // SetError2(ctx, "unexpected struct ", "");
            }
        }
    }


    if (pStructUnionSpecifier->Name != NULL)
    {
        SymbolMap_SetAt(ctx->pCurrentScope, pStructUnionSpecifier->Name, (TTypePointer*)pStructUnionSpecifier);
    }

    token = Parser_CurrentToken(ctx);
    if (token == TK_LEFT_CURLY_BRACKET)
    {
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

void Enumeration_Constant(Parser* ctx,
    TEnumerator* pEnumerator2)
{
    const char* lexeme = Lexeme(ctx);
    String_Set(&pEnumerator2->Name, lexeme);
    Parser_MatchToken(ctx, TK_IDENTIFIER, &pEnumerator2->ClueList0);
}

bool EnumeratorC(Parser* ctx, TEnumerator* pEnumerator2)
{
    /*
    enumerator:
    enumeration-constant
    enumeration-constant = constant-expression
    */
    bool bValueAssigned = false;
    Enumeration_Constant(ctx, pEnumerator2);


    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_EQUALS_SIGN)
    {
        Parser_Match(ctx, &pEnumerator2->ClueList1);
        ConstantExpression(ctx, &pEnumerator2->pExpression);
        bValueAssigned = true;
    }

    return bValueAssigned;
}

void Enumerator_List(Parser* ctx,
    TEnumeratorList* pEnumeratorList2)
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

    TEnumerator* pEnumerator2 = TEnumerator_Create();
    List_Add(pEnumeratorList2, pEnumerator2);

    EnumeratorC(ctx, pEnumerator2);
    SymbolMap_SetAt(ctx->pCurrentScope, pEnumerator2->Name, (TTypePointer*)pEnumerator2);

    Tokens token = Parser_CurrentToken(ctx);

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

void Enum_Specifier(Parser* ctx, TEnumSpecifier* pEnumSpecifier2)
{
    /*
    enum-specifier:
    enum identifieropt { enumerator-list }
    enum identifieropt { enumerator-list, }
    enum identifier
    */

    Parser_MatchToken(ctx, TK_ENUM, &pEnumSpecifier2->ClueList0);

    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_IDENTIFIER)
    {
        const char* lexeme = Lexeme(ctx);
        String_Set(&pEnumSpecifier2->Name, lexeme);
        Parser_Match(ctx, &pEnumSpecifier2->ClueList1);
    }

    else
    {
        const char* name = GetName();
        String_Set(&pEnumSpecifier2->Name, name);
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

bool TFunctionSpecifier_IsFirst(Tokens token)
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

bool Function_Specifier(Parser* ctx,
    TFunctionSpecifier* pFunctionSpecifier)
{
    /*
    function-specifier:
    inline
    _Noreturn
    */
    bool bResult = false;
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK__INLINE://microsoft
    case TK__FORCEINLINE://microsoft
    case TK_INLINE:
        pFunctionSpecifier->bIsInline = true;
        Parser_Match(ctx, &pFunctionSpecifier->ClueList0);
        bResult = true;
        break;

    case TK__NORETURN:
        pFunctionSpecifier->bIsNoReturn = true;
        Parser_Match(ctx, &pFunctionSpecifier->ClueList0);
        bResult = true;
        break;

    default:
        break;
    }

    return bResult;
}

bool TStorageSpecifier_IsFirst(Tokens token)
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

bool Storage_Class_Specifier(Parser* ctx,

    TStorageSpecifier* pStorageSpecifier)
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
    bool bResult = false;
    Tokens token = Parser_CurrentToken(ctx);

    //const char* lexeme = Lexeme(ctx);
    switch (token)
    {
    case TK_TYPEDEF:
        pStorageSpecifier->bIsTypedef = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    case TK_EXTERN:
        pStorageSpecifier->bIsExtern = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    case TK_STATIC:
        pStorageSpecifier->bIsStatic = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    case TK__THREAD_LOCAL:
        pStorageSpecifier->bIsThread_local = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    case TK_AUTO:
        pStorageSpecifier->bIsAuto = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    case TK_REGISTER:
        pStorageSpecifier->bIsRegister = true;
        Parser_Match(ctx, &pStorageSpecifier->ClueList0);
        bResult = true;
        break;

    default:
        break;
    }

    return bResult;
}

void Parameter_List(Parser* ctx,
    TParameterList* pParameterList)
{
    /*
    parameter-list:
    parameter-declaration
    parameter-list, parameter-declaration
    */
    Tokens token = Parser_CurrentToken(ctx);

    TParameter*  pParameter = TParameter_Create();
    List_Add(pParameterList, pParameter);
    Parameter_Declaration(ctx, pParameter);

    //Tem mais?
    token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        //a virgula fica no anterior
        pParameter->bHasComma = true;
        Parser_Match(ctx, &pParameter->ClueList00);

        token = Parser_CurrentToken(ctx);
        if (token != TK_DOTDOTDOT)
        {
            Parameter_List(ctx, pParameterList);
        }
    }
}

void Parameter_Declaration(Parser* ctx,
    TParameter* pParameterDeclaration)
{

    //ctx->pCurrentParameterScope

    /*
    parameter-declaration:
    declaration-specifiers declarator
    declaration-specifiers abstract-declaratoropt
    */
    Declaration_Specifiers(ctx,
        &pParameterDeclaration->Specifiers);

    TDeclarator *pDeclarator = NULL;
    Declarator(ctx, true, &pDeclarator);
    if (pDeclarator)
    {
        pParameterDeclaration->Declarator = *pDeclarator;
    }
}

void Parameter_Type_List(Parser* ctx,
    TParameterTypeList* pParameterList)
{
    /*
    parameter-type-list:
    parameter-list
    parameter-list , ...
    */

    Parameter_List(ctx, &pParameterList->ParameterList);

    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK_DOTDOTDOT)
    {
        pParameterList->bVariadicArgs = true;
        //TODO ADD this parameter
        Parser_Match(ctx, &pParameterList->ClueList1);
    }
}


void Direct_Declarator(Parser* ctx, bool bAbstract, TDirectDeclarator** ppDeclarator2)
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
    TDirectDeclarator *pDirectDeclarator = NULL;

    if (ErrorOrEof(ctx))
        return;


    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
    case TK_LEFT_PARENTHESIS:
    {
        ASSERT(pDirectDeclarator == NULL);
        pDirectDeclarator = TDirectDeclarator_Create();

        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pDirectDeclarator->ClueList0);

        Declarator(ctx, bAbstract, &pDirectDeclarator->pDeclarator);
        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pDirectDeclarator->ClueList1);

        //Para indicar que eh uma ( declarator )
        pDirectDeclarator->Type = TDirectDeclaratorTypeDeclarator;
        // ) para nao confundir com funcao (
    }
    break;

    case TK_IDENTIFIER:
    {
        //identifier
        pDirectDeclarator = TDirectDeclarator_Create();

        //Para indicar que eh uma identificador

        pDirectDeclarator->Type = TDirectDeclaratorTypeIdentifier;

        const char* lexeme = Lexeme(ctx);
        String_Set(&pDirectDeclarator->Identifier, lexeme);
        pDirectDeclarator->Position.Line = GetCurrentLine(ctx);
        pDirectDeclarator->Position.FileIndex = GetFileIndex(ctx);
        Parser_Match(ctx, &pDirectDeclarator->ClueList0);
    }
    break;

    default:
        //assert(false);
        break;
    }



    if (pDirectDeclarator == NULL)
    {
        //Por enquanto esta funcao esta sendo usada para
        //abstract declarator que nao tem nome.
        //vou criar aqui por enquanto um cara vazio
        pDirectDeclarator = TDirectDeclarator_Create();
        String_Set(&pDirectDeclarator->Identifier, "");
        pDirectDeclarator->Position.Line = GetCurrentLine(ctx);
        pDirectDeclarator->Position.FileIndex = GetFileIndex(ctx);

        //Para indicar que eh uma identificador

        pDirectDeclarator->Type = TDirectDeclaratorTypeIdentifier;

        //Quando tiver abstract declarator vai ser 
        //bug cair aqui
    }

    *ppDeclarator2 = pDirectDeclarator;

    for (;;)
    {
        ASSERT(pDirectDeclarator != NULL);

        token = Parser_CurrentToken(ctx);
        switch (token)
        {
        case TK_LEFT_PARENTHESIS:

            /*
            direct-declarator ( parameter-type-list )
            direct-declarator ( identifier-listopt )
            */
            //      pDirectDeclarator->token = token;
      //      ASSERT(pDirectDeclarator->pParametersOpt == NULL);
      //      pDirectDeclarator->pParametersOpt = TParameterList_Create();
            token = Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pDirectDeclarator->ClueList2);

            //Para indicar que eh uma funcao

            pDirectDeclarator->Type = TDirectDeclaratorTypeFunction;

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

            //ASSERT(pDirectDeclarator->pParametersOpt == NULL);
            //pDirectDeclarator->pParametersOpt = TParameterList_Create();

            //Para indicar que eh um array

            pDirectDeclarator->Type = TDirectDeclaratorTypeArray;

            token = Parser_MatchToken(ctx, TK_LEFT_SQUARE_BRACKET, &pDirectDeclarator->ClueList2);
            if (token == TK_STATIC)
            {
            }
            else
            {
                if (token != TK_RIGHT_SQUARE_BRACKET)
                {
                    ASSERT(pDirectDeclarator->pExpression == NULL);
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
            ASSERT(false);
            break;
        }

        token = Parser_CurrentToken(ctx);
        if (token != TK_LEFT_PARENTHESIS &&   token != TK_LEFT_SQUARE_BRACKET)
        {
            break;
        }
        else
        {
            TDirectDeclarator *pDirectDeclaratorNext = TDirectDeclarator_Create();
            pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
            pDirectDeclarator = pDirectDeclaratorNext;
        }
    }

    token = Parser_CurrentToken(ctx);
    if (token == TK_LEFT_PARENTHESIS)
    {
        //tem mais
        TDirectDeclarator *pDirectDeclaratorNext = NULL;
        Direct_Declarator(ctx, bAbstract, &pDirectDeclaratorNext);
        pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
    }
    else if (!bAbstract && token == TK_IDENTIFIER)
    {
        //tem mais
        TDirectDeclarator *pDirectDeclaratorNext = NULL;
        Direct_Declarator(ctx, bAbstract, &pDirectDeclaratorNext);
        pDirectDeclarator->pDirectDeclarator = pDirectDeclaratorNext;
    }


}

static bool TTypeQualifier_IsFirst(Tokens token)
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
    case TK__AUTO:

    case TK_OPT_QUALIFIER:
    case TK_OWN_QUALIFIER:
    case TK_DTOR_QUALIFIER:
    case TK_MDTOR_QUALIFIER:
        bResult = true;
        break;
#endif
    default:
        break;
    }

    return bResult;
}



bool Type_Qualifier(Parser* ctx, TTypeQualifier* pQualifier)
{
    /*
    type-qualifier:
    const
    restrict
    volatile
    _Atomic
    */
    bool bResult = false;
    Tokens token = Parser_CurrentToken(ctx);

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

    case TK__AUTO:
        //type-qualifier-extensions 
    case TK_OPT_QUALIFIER:
    case TK_OWN_QUALIFIER:
    case TK_DTOR_QUALIFIER:
    case TK_MDTOR_QUALIFIER:
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

bool Type_Qualifier_ListOpt(Parser* ctx,
    TTypeQualifierList* pQualifiers)
{
    /*
    type-qualifier-list:
    type-qualifier
    type-qualifier-list type-qualifier
    */
    bool bResult = IsTypeQualifierToken(Parser_CurrentToken(ctx));
    if (bResult)
    {
        TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
        Type_Qualifier(ctx, pTypeQualifier);
        List_Add(pQualifiers, pTypeQualifier);

        if (IsTypeQualifierToken(Parser_CurrentToken(ctx)))
        {
            Type_Qualifier_ListOpt(ctx, pQualifiers);
        }
    }

    return bResult;
}

int PointerOpt(Parser* ctx, TPointerList* pPointerList)
{
    /*
    exemplo
    int _wexecv(int const* const* _ArgList);
    */
    /*
    pointer:
    * type-qualifier-listopt
    * type-qualifier-listopt pointer
    */
    int ns = 0;
    //Empilha pointer to
    Tokens token = Parser_CurrentToken(ctx);

    while (IsTypeQualifierToken(token) ||
        token == TK_ASTERISK)   //pointer
    {
        TPointer* pPointer = TPointer_Create();
        List_Add(pPointerList, pPointer);

        if (IsTypeQualifierToken(token))
        {
            pPointer->bPointer = false;
            Type_Qualifier_ListOpt(ctx, &pPointer->Qualifier);
        }
        else if (token == TK_ASTERISK)
        {
            Parser_Match(ctx, &pPointer->ClueList0);
            pPointer->bPointer = true;
        }


        token = Parser_CurrentToken(ctx);
        ns++;
    }

    //Retorna numero de items empilhados
    return ns;
}

//pag 123 C
void Declarator(Parser* ctx, bool bAbstract, TDeclarator** ppTDeclarator2)
{
    *ppTDeclarator2 = NULL; //out
    TDeclarator* pDeclarator = TDeclarator_Create();
    /*
    declarator:
    pointeropt direct-declarator
    */
    PointerOpt(ctx, &pDeclarator->PointerList);

    ASSERT(pDeclarator->pDirectDeclarator == NULL);
    Direct_Declarator(ctx, bAbstract, &pDeclarator->pDirectDeclarator);

    *ppTDeclarator2 = pDeclarator;
}


bool TAlignmentSpecifier_IsFirst(Tokens token)
{
    /*
    alignment - specifier:
    _Alignas(type - name)
    _Alignas(constant - expression)
    */
    return (token == TK__ALIGNAS);
}

bool Alignment_Specifier(Parser* ctx,
    TAlignmentSpecifier* pAlignmentSpecifier)
{
    bool bResult = false;
    /*
    alignment - specifier:
    _Alignas(type - name)
    _Alignas(constant - expression)
    */
    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__ALIGNAS)
    {
        Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, NULL);
        ASSERT(false);//TODO
        Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL);
        bResult = true;
    }

    return bResult;
}


bool TTypeSpecifier_IsFirst(Parser* ctx, Tokens token, const char* lexeme)
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
    case TK_char:
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
    case TK__TEMPLATE:
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

void TemplateTypeSpecifierArgument(Parser* ctx,
    TTemplateTypeSpecifierArgument* p)
{
    /*
    Thiago
    template-type-specifier-argument:
    typename
    */

    TypeName(ctx, &p->TypeName);
}

void TemplateTypeSpecifierArgumentList(Parser* ctx,
    TTemplateTypeSpecifierArgumentList* pList)
{
    /*
    template-type-specifier-argument-list:
    template-type-specifier-argument
    template-type-specifier-argument , template-type-specifier-argument-list
    */

    TTemplateTypeSpecifierArgument* pTemplateTypeSpecifierArgument
        = TTemplateTypeSpecifierArgument_Create();

    TemplateTypeSpecifierArgument(ctx, pTemplateTypeSpecifierArgument);

    List_Add(pList, pTemplateTypeSpecifierArgument);

    //Tem mais?
    Tokens token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        Parser_Match(ctx, NULL);
        TemplateTypeSpecifierArgumentList(ctx, pList);
    }
}

void TemplateTypeSpecifier(Parser* ctx,
    TTypeSpecifier** ppTypeSpecifier)
{
    /*
    template-type-specifier:
    _Template ( identifier)
    _Template ( identifier, template-type-specifier-argument-list )
    */
    TTemplateTypeSpecifier* pTemplateTypeSpecifier =
        TTemplateTypeSpecifier_Create();

    *ppTypeSpecifier = TTemplateTypeSpecifier_As_TTypeSpecifier(pTemplateTypeSpecifier);

    Parser_MatchToken(ctx, TK__TEMPLATE, &pTemplateTypeSpecifier->ClueList0);

    Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, NULL);

    String_Set(&pTemplateTypeSpecifier->Identifier, Lexeme(ctx));
    Parser_MatchToken(ctx, TK_IDENTIFIER, NULL);

    Tokens token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        Parser_MatchToken(ctx, TK_COMMA, NULL);
        TemplateTypeSpecifierArgumentList(ctx,
            &pTemplateTypeSpecifier->Args);
    }

    Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, NULL);

}


void AtomicTypeSpecifier(Parser* ctx,
    TTypeSpecifier** ppTypeSpecifier)
{
    assert(false); //tODO criar TAtomicTypeSpecifier
    /*
    atomic-type-specifier:
      _Atomic ( type-name )
    */
    TAtomicTypeSpecifier* pAtomicTypeSpecifier =
        TAtomicTypeSpecifier_Create();

    *ppTypeSpecifier = TAtomicTypeSpecifier_As_TTypeSpecifier(pAtomicTypeSpecifier);

    Parser_MatchToken(ctx, TK__ATOMIC, &pAtomicTypeSpecifier->ClueList0);

    Parser_MatchToken(ctx, TK_LEFT_PARENTHESIS, &pAtomicTypeSpecifier->ClueList1);

    TypeName(ctx, &pAtomicTypeSpecifier->TypeName);

    Parser_MatchToken(ctx, TK_RIGHT_PARENTHESIS, &pAtomicTypeSpecifier->ClueList2);
}

void Type_Specifier(Parser* ctx, TTypeSpecifier** ppTypeSpecifier)
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

    const char* lexeme = Lexeme(ctx);
    Tokens token = Parser_CurrentToken(ctx);

    switch (token)
    {
        //type - specifier
    case TK_VOID:
    case TK_char:
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
        TSingleTypeSpecifier*  pSingleTypeSpecifier =
            TSingleTypeSpecifier_Create();

        pSingleTypeSpecifier->Token = token;
        bResult = true;

        Parser_Match(ctx, &pSingleTypeSpecifier->ClueList0);

        *ppTypeSpecifier = (TTypeSpecifier*)pSingleTypeSpecifier;
    }
    break;

    //atomic-type-specifier
    case TK__ATOMIC:
        bResult = true;
        AtomicTypeSpecifier(ctx, ppTypeSpecifier);
        break;

        //extensao thiago
        //template-type-specifier
    case TK__TEMPLATE:
        bResult = true;
        TemplateTypeSpecifier(ctx, ppTypeSpecifier);
        break;

    case TK_STRUCT:
    case TK_UNION:
    {
        ASSERT(*ppTypeSpecifier == NULL);
        bResult = true;
        TStructUnionSpecifier* pStructUnionSpecifier = TStructUnionSpecifier_Create();

        *ppTypeSpecifier = (TTypeSpecifier*)pStructUnionSpecifier;
        Struct_Or_Union_Specifier(ctx, pStructUnionSpecifier);
    }
    break;

    case TK_ENUM:
    {
        ASSERT(*ppTypeSpecifier == NULL);
        bResult = true;
        TEnumSpecifier* pEnumSpecifier2 = TEnumSpecifier_Create();
        *ppTypeSpecifier = (TTypeSpecifier*)pEnumSpecifier2;
        Enum_Specifier(ctx, pEnumSpecifier2);
    }
    break;

    case TK_IDENTIFIER:
    {
        int bIsTypedef = IsTypeName(ctx, TK_IDENTIFIER, lexeme);
        if (bIsTypedef)
        {
            TSingleTypeSpecifier* pSingleTypeSpecifier = TSingleTypeSpecifier_Create();
            pSingleTypeSpecifier->Token = token;
            String_Set(&pSingleTypeSpecifier->TypedefName, lexeme);
            bResult = true;

            Parser_Match(ctx, &pSingleTypeSpecifier->ClueList0);
            *ppTypeSpecifier = (TTypeSpecifier*)pSingleTypeSpecifier;

        }
        else
        {
            ASSERT(false); //temque chegar aqui limpo ja
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

bool Declaration_Specifiers_IsFirst(Parser* ctx, Tokens token, const char* lexeme)
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

void Declaration_Specifiers(Parser* ctx,
    TDeclarationSpecifiers* pDeclarationSpecifiers)
{
    /*
    declaration-specifiers:
    storage-class-specifier declaration-specifiersopt
    type-specifier          declaration-specifiersopt
    type-qualifier          declaration-specifiersopt
    function-specifier      declaration-specifiersopt
    alignment-specifier     declaration-specifiersopt
    */
    Tokens token = Parser_CurrentToken(ctx);
    const char* lexeme = Lexeme(ctx);

    if (TStorageSpecifier_IsFirst(token))
    {
        TStorageSpecifier* pStorageSpecifier = TStorageSpecifier_Create();

        Storage_Class_Specifier(ctx, pStorageSpecifier);

        List_Add(pDeclarationSpecifiers, TStorageSpecifier_As_TSpecifier(pStorageSpecifier));
    }
    else if (TTypeSpecifier_IsFirst(ctx, token, lexeme))
    {        
        if (TDeclarationSpecifiers_CanAddSpeficier(pDeclarationSpecifiers,
            token,
            lexeme))
        {
            TTypeSpecifier* pTypeSpecifier = NULL;
            Type_Specifier(ctx, &pTypeSpecifier);
            List_Add(pDeclarationSpecifiers, pTypeSpecifier);
        }
        else
        {
            SetError(ctx, "double typedef");
        }

    }
    else if (TTypeQualifier_IsFirst(token))
    {
        TTypeQualifier* pTypeQualifier = TTypeQualifier_Create();
        Type_Qualifier(ctx, pTypeQualifier);
        List_Add(pDeclarationSpecifiers, TTypeQualifier_As_TSpecifierQualifier(pTypeQualifier));
    }
    else if (TFunctionSpecifier_IsFirst(token))
    {
        TFunctionSpecifier* pFunctionSpecifier = TFunctionSpecifier_Create();
        Function_Specifier(ctx, pFunctionSpecifier);
        List_Add(pDeclarationSpecifiers, TFunctionSpecifier_As_TSpecifier(pFunctionSpecifier));
    }
    else if (TAlignmentSpecifier_IsFirst(token))
    {
        ASSERT(false);
        //TAlignmentSpecifier* pAlignmentSpecifier = TAlignmentSpecifier_Create();
        //List_Add(pDeclarationSpecifiers, TAlignmentSpecifier_As_TSpecifier(pAlignmentSpecifier));
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



void Initializer(Parser* ctx,
    TInitializer** ppInitializer,
    Tokens endToken1,
    Tokens endToken2)
{
    ASSERT(*ppInitializer == NULL);
    /*
    initializer:
    assignment-expression
    { initializer-list }
    { initializer-list , }
    */

    /*
    initializer:
    _default
    assignment-expression
    _defaultopt { initializer-list }
    _defaultopt { initializer-list , }
    */

    Tokens token = Parser_CurrentToken(ctx);
    if (token == TK__DEFAULT ||
        token == TK_DEFAULT)
    {
        TInitializerListType* pTInitializerList =
            TInitializerListType_Create();

        pTInitializerList->bDefault = true;

        *ppInitializer = (TInitializer*)pTInitializerList;

        Parser_Match(ctx, &pTInitializerList->ClueList00);
        token = Parser_CurrentToken(ctx);


        if (token == TK_LEFT_CURLY_BRACKET)
        {
            Parser_Match(ctx, &pTInitializerList->ClueList0);

            Initializer_List(ctx, &pTInitializerList->InitializerList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                &pTInitializerList->ClueList1);
        }
    }
    else
    {
        if (token == TK_LEFT_CURLY_BRACKET)
        {
            TInitializerListType* pTInitializerList =
                TInitializerListType_Create();


            *ppInitializer = (TInitializer*)pTInitializerList;

            Parser_Match(ctx, &pTInitializerList->ClueList0);

            Initializer_List(ctx, &pTInitializerList->InitializerList);

            Parser_MatchToken(ctx, TK_RIGHT_CURLY_BRACKET,
                &pTInitializerList->ClueList1);
        }

        else
        {
            TExpression* pExpression = NULL;
            AssignmentExpression(ctx, &pExpression);
            *ppInitializer = (TInitializer*)pExpression;
        }
    }
}

void Initializer_List(Parser* ctx, TInitializerList* pInitializerList)
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

        TInitializerListItem* pTInitializerListItem = TInitializerListItem_Create();
        List_Add(pInitializerList, pTInitializerListItem);
        Tokens token = Parser_CurrentToken(ctx);
#ifdef LANGUAGE_EXTENSIONS
        if (token == TK_RIGHT_CURLY_BRACKET)
        {
            //Empty initializer
            break;
        }
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

void Designation(Parser* ctx, TDesignatorList* pDesignatorList)
{
    /*
    designation:
    designator-list =
    */
    Designator_List(ctx, pDesignatorList);
    Parser_MatchToken(ctx, TK_EQUALS_SIGN, NULL);//tODO
}

void Designator_List(Parser* ctx, TDesignatorList* pDesignatorList)
{
    // http://www.drdobbs.com/the-new-c-declarations-initializations/184401377
    /*
    designator-list:
    designator
    designator-list designator
    */
    TDesignator* pDesignator = TDesignator_Create();
    Designator(ctx, pDesignator);
    List_Add(pDesignatorList, pDesignator);

    for (; ;)
    {
        if (ErrorOrEof(ctx))
            break;

        Tokens token = Parser_CurrentToken(ctx);

        if (token == TK_LEFT_SQUARE_BRACKET ||
            token == TK_FULL_STOP)
        {
            TDesignator* pDesignatorNew = TDesignator_Create();
            Designator(ctx, pDesignatorNew);
            List_Add(pDesignatorList, pDesignatorNew);
        }

        else
        {
            break;
        }
    }
}

void Designator(Parser* ctx, TDesignator* p)
{
    /*
    designator:
    [ constant-expression ]
    . identifier
    */
    Tokens token = Parser_CurrentToken(ctx);

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

        String_Set(&p->Name, Lexeme(ctx));
        Parser_MatchToken(ctx, TK_IDENTIFIER, NULL);
    }
}

void Init_Declarator(Parser* ctx,
    TInitDeclarator** ppDeclarator2)
{
    /*
    init-declarator:
    declarator
    declarator = initializer
    */
    TInitDeclarator* pInitDeclarator =
        TInitDeclarator_Create();

    ASSERT(pInitDeclarator->pDeclarator == NULL);
    Declarator(ctx, false, &pInitDeclarator->pDeclarator);
    Tokens token = Parser_CurrentToken(ctx);

    const char* declaratorName = TInitDeclarator_FindName(pInitDeclarator);
    if (declaratorName)
    {
        //Fica em um contexto que vive so durante a declaracao
        //depois eh substituido
        
        SymbolMap_SetAt(ctx->pCurrentScope, declaratorName, (TTypePointer*) pInitDeclarator);
    }

    //Antes do =
    *ppDeclarator2 = pInitDeclarator;

    if (token == TK_EQUALS_SIGN)
    {
        ASSERT(*ppDeclarator2 != NULL);

        Parser_Match(ctx, &pInitDeclarator->ClueList00);

        Initializer(ctx, &pInitDeclarator->pInitializer, TK_SEMICOLON, TK_SEMICOLON);
        ////TNodeClueList_MoveToEnd(&pInitDeclarator->ClueList, &ctx->Scanner.ClueList);
    }
}

void Init_Declarator_List(Parser* ctx,
    TInitDeclaratorList* pInitDeclaratorList)
{
    /*
    init-declarator-list:
    init-declarator
    init-declarator-list , init-declarator
    */

    TInitDeclarator* pInitDeclarator = NULL;
    Init_Declarator(ctx, &pInitDeclarator);
    List_Add(pInitDeclaratorList, pInitDeclarator);

    //Tem mais?
    Tokens token = Parser_CurrentToken(ctx);
    if (token == TK_COMMA)
    {
        Parser_Match(ctx, &pInitDeclarator->ClueList00);

        Init_Declarator_List(ctx, pInitDeclaratorList);
    }

}


bool  Declaration(Parser* ctx,
    TAnyDeclaration** ppDeclaration)
{
    /*
    declaration:
    declaration-specifiers;
    declaration-specifiers init-declarator-list ;
    static_assert-declaration
    */



    bool bHasDeclaration = false;
    Tokens token = Parser_CurrentToken(ctx);

    if (token == TK__STATIC_ASSERT)
    {
        TStaticAssertDeclaration* pStaticAssertDeclaration = TStaticAssertDeclaration_Create();
        *ppDeclaration = (TAnyDeclaration*)pStaticAssertDeclaration;
        Static_Assert_Declaration(ctx, pStaticAssertDeclaration);
        bHasDeclaration = true;
    }

    else
    {
        TDeclaration* pFuncVarDeclaration = TDeclaration_Create();

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
            *ppDeclaration = (TAnyDeclaration*)pFuncVarDeclaration;
            pFuncVarDeclaration->FileIndex = GetFileIndex(ctx);
            pFuncVarDeclaration->Line = GetCurrentLine(ctx);
            //ASSERT(pFuncVarDeclaration->FileIndex >= 0);

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

                SymbolMap BlockScope = SYMBOLMAP_INIT;

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
                ForEachListItem(TInitDeclarator, pInitDeclarator, &pFuncVarDeclaration->InitDeclaratorList)
                {
                    const char* declaratorName = TInitDeclarator_FindName(pInitDeclarator);
                   
               
                                       
                    if (declaratorName != NULL)
                    {
                        SymbolMap_SetAt(ctx->pCurrentScope, declaratorName, (TTypePointer*)pFuncVarDeclaration);
                    }

                    //ctx->
                }
                //
                if (token == TK__DEFAULT ||
                    token == TK_DEFAULT)
                {
                    /*
                    6.9.1) function-definition:
                    declaration-specifiers declarator declaration-listopt defaultopt compound-statement
                    */

                    pFuncVarDeclaration->bDefault = true;
                    Parser_Match(ctx, &pFuncVarDeclaration->ClueList00);
                    token = Parser_CurrentToken(ctx);
                }

                if (token == TK_LEFT_CURLY_BRACKET)
                {
                    //Ativa o escopo dos parametros
                    //Adiconar os parametros em um escopo um pouco a cima.
                    SymbolMap BlockScope = SYMBOLMAP_INIT;

                    TInitDeclarator* pDeclarator3 =
                        pFuncVarDeclaration->InitDeclaratorList.pHead;

                    ForEachListItem(TParameter, pParameter, &pDeclarator3->pDeclarator->pDirectDeclarator->Parameters.ParameterList)
                    {
                        const char* parameterName = TDeclarator_GetName(&pParameter->Declarator);
                        if (parameterName != NULL)
                        {
                            SymbolMap_SetAt(&BlockScope, parameterName, (TTypePointer*)pParameter);
                        }
                        else
                        {
                            //parametro sem nome
                        }
                    }

                    BlockScope.pPrevious = ctx->pCurrentScope;
                    ctx->pCurrentScope = &BlockScope;


                    //SymbolMap_Print(ctx->pCurrentScope);
                    /*
                    6.9.1) function-definition:
                    declaration-specifiers declarator declaration-listopt compound-statement
                    */
                    TStatement* pStatement;
                    Compound_Statement(ctx, &pStatement);
                    //TODO cast

                    ctx->pCurrentScope = BlockScope.pPrevious;
                    SymbolMap_Destroy(&BlockScope);


                    pFuncVarDeclaration->pCompoundStatementOpt = (TCompoundStatement*)pStatement;
                }

                else
                {
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


void Parse_Declarations(Parser* ctx, TDeclarations* declarations)
{
    int declarationIndex = 0;

    while (!ErrorOrEof(ctx))
    {
        TAnyDeclaration* pDeclarationOut = NULL;
        bool bHasDecl = Declaration(ctx, &pDeclarationOut);
        if (bHasDecl)
        {


            //TDeclarations_Destroy(&ctx->Templates);
            //TDeclarations_Init(&ctx->Templates);

            //printf("%s", ctx->Scanner.PreprocessorAndCommentsString);
           // StrBuilder_Clear(&ctx->Scanner.PreprocessorAndCommentsString);

            //Cada Declaration poderia ter out uma lista TDeclarations
            //publica que vai ser inserida aqui.
            //
            ArrayT_Push(declarations, pDeclarationOut);
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
                SetError(ctx, "declaration expected");
            }
            break;
        }

        if (Parser_CurrentToken(ctx) == TK_EOF)
        {
            TEofDeclaration* pEofDeclaration =
                TEofDeclaration_Create();
            //ok
            Parser_Match(ctx, &pEofDeclaration->ClueList0);
            ArrayT_Push(declarations, pEofDeclaration);
        }

        if (Parser_HasError(ctx))
            break;

    }



}

void Parser_Main(Parser* ctx, TDeclarations* declarations)
{
    Parse_Declarations(ctx, declarations);
}

static void TFileMapToStrArray(TFileMap* map, TFileArray* arr)
{
    ArrayT_Reserve(arr, map->Size);
    arr->size = map->Size;

    for (int i = 0; i < map->buckets.size; i++)
    {
        Bucket*  data = map->buckets.data[i];

        if (data != NULL)
        {
            for (int k = 0; k < data->size; k++)
            {
                BucketItem* node = data->data[k];
                TFile* pFile = (TFile*)node->data;

                if (pFile->FileIndex >= 0 &&
                    pFile->FileIndex < (int)arr->size)
                {
                    arr->pItems[pFile->FileIndex] = pFile;
                    node->data = NULL; //movido para array
                }
            }
        }
    }
}

bool GetAST(const char*  filename,
    const char* configFileName /*optional*/,

    TProgram* pProgram)
{
    bool bResult = false;


    Parser parser;

    if (configFileName != NULL)
    {
        //opcional   
        String fullConfigFilePath = STRING_INIT;
        GetFullPath(configFileName, &fullConfigFilePath);

        Parser_InitFile(&parser, fullConfigFilePath);
        Parser_Main(&parser, &pProgram->Declarations);

        //Some com o arquivo de config
        List_Clear(ScannerItem, &parser.ClueList);
        BasicScannerStack_Pop(&parser.Scanner.stack);
        //Some com o arquivo de config
        String_Destroy(&fullConfigFilePath);
    }

    String fullFileNamePath = STRING_INIT;
    GetFullPath(filename, &fullFileNamePath);


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
        Parser_Main(&parser, &pProgram->Declarations);
    }

    TFileMapToStrArray(&parser.Scanner.FilesIncluded, &pProgram->Files2);
    printf("%s\n", GetCompletationMessage(&parser));
    SymbolMap_Swap(&parser.GlobalScope, &pProgram->GlobalScope);

    if (Parser_HasError(&parser))
    {
      Scanner_PrintDebug(&parser.Scanner);
    }
    
    MacroMap_Swap(&parser.Scanner.Defines2, &pProgram->Defines);

    bResult = !Parser_HasError(&parser);

    Parser_Destroy(&parser);
    String_Destroy(&fullFileNamePath);

    return bResult;
}




#if 0


void PrintString(const char* psz)
{
    printf("\"");

    while (*psz)
    {
        switch (*psz)
        {
        case '"':
            printf("\\\"");
            break;

        case '/':
            printf("\\/");
            break;

        case '\b':
            printf("\\b");
            break;

        case '\f':
            printf("\\f");
            break;

        case '\n':
            printf("\\n");
            break;

        case L'\r':
            printf("\\r");
            break;

        case L'\t':
            printf("\\t");
            break;

        case L'\\':
            printf("\\\\");
            break;

        default:
            printf("%c", *psz);
            break;
        }

        psz++;
    }

    printf("\"");
}

void PrintTokens(Scanner* scanner)
{
    while (Scanner_TokenAt(scanner) != TK_EOF)
    {
        printf("%s : ", Scanner_TokenString(scanner));
        PrintString(Scanner_Lexeme(scanner));
        printf("\n");
        Scanner_Next(scanner);
    }
}

void PrintTokensOfFile(const char* fileName)
{
    Scanner scanner;
    Scanner_Init(&scanner);
    Scanner_IncludeFile(&scanner, fileName, FileIncludeTypeFullPath);
    PrintTokens(&scanner);
    Scanner_Destroy(&scanner);
}


void PrintPreprocessedToConsole(const char* fileIn)
{
    Scanner scanner;
    Scanner_Init(&scanner);
    Scanner_IncludeFile(&scanner, fileIn, FileIncludeTypeFullPath);
    scanner.bIncludeSpaces = true;

    while (Scanner_TokenAt(&scanner) != TK_EOF)
    {
        Tokens token = Scanner_Top(&scanner)->currentItem.token;
        const char* lexeme = Scanner_Top(&scanner)->currentItem.lexeme.c_str;

        if (token == TK_BREAKLINE)
        {
            printf("%s", lexeme);
        }

        if (token == TK_LINE_COMMENT ||
            token == TK_COMMENT)
        {
            //fprintf(fp, "%s", lexeme);
        }
        else
        {
            printf("%s", lexeme);
        }

        Scanner_Next(&scanner);
    }

    Scanner_Destroy(&scanner);
}


void GetUnprocessedTokens(const char* fileIn/*, JObj* pOutArray*/)
{
    BasicScanner scanner;
    BasicScanner_InitFile(&scanner, fileIn);

    while (scanner.currentItem.token != TK_EOF)
    {
        Tokens token = scanner.currentItem.token;
        const char* lexeme = scanner.currentItem.lexeme.c_str;
        //JObj *pNew = JObj_PushNewArray(pOutArray);
        //JObj_PushString(pNew, TokenToString(token));
        //JObj_PushString(pNew, lexeme);
        BasicScanner_Next(&scanner);
    }

    BasicScanner_Destroy(&scanner);
}

void GetProcessedTokens2(const char* fileIn/*, JObj* pOutArray*/)
{
    Scanner scanner;
    Scanner_Init(&scanner);
    Scanner_IncludeFile(&scanner, fileIn, FileIncludeTypeFullPath);

    while (Scanner_TokenAt(&scanner) != TK_EOF)
    {
        Tokens token = Scanner_TokenAt(&scanner);
        const char* lexeme = Scanner_Lexeme(&scanner);
        //JObj *pNew = JObj_PushNewArray(pOutArray);
        //JObj_PushString(pNew, TokenToString(token));
        //JObj_PushString(pNew, lexeme);
        Scanner_Next(&scanner);
    }

    Scanner_Destroy(&scanner);
}

void GetProcessedTokens(const char* fileIn/*, JObj* pOutArray*/)
{
    Scanner scanner;
    Scanner_Init(&scanner);
    scanner.bIncludeSpaces = true;
    Scanner_IncludeFile(&scanner, fileIn, FileIncludeTypeFullPath);

    while (Scanner_TokenAt(&scanner) != TK_EOF)
    {
        Tokens token = Scanner_Top(&scanner)->currentItem.token;
        const char* lexeme = Scanner_Top(&scanner)->currentItem.lexeme.c_str;
        //JObj *pNew = JObj_PushNewObject(pOutArray);
        //JObj_SetString(pNew, "token", TokenToString(token));
        //JObj_SetString(pNew, "lexeme", lexeme);
        Scanner_Next(&scanner);
    }

    Scanner_Destroy(&scanner);
}



#endif

