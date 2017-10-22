#include "Ast.h"
#include "Array.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

void TBlockItemList_Destroy(TBlockItemList* p)
{
    ArrayT_Destroy(TBlockItem, p);
}

void TCompoundStatement_Destroy(TCompoundStatement *p) _default
{
    TBlockItemList_Destroy(&p->BlockItemList);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TLabeledStatement_Destroy(TLabeledStatement *p) _default
{
    TStatement_Delete(p->pStatementOpt);
    TExpression_Delete(p->pExpression);
    String_Destroy(&p->Identifier);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TForStatement_Destroy(TForStatement *p) _default
{
    TAnyDeclaration_Delete(p->pInitDeclarationOpt);
    TExpression_Delete(p->pExpression1);
    TExpression_Delete(p->pExpression2);
    TExpression_Delete(p->pExpression3);
    TStatement_Delete(p->pStatement);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
    TScannerItemList_Destroy(&p->ClueList4);
}

void TWhileStatement_Destroy(TWhileStatement *p) _default
{
    TExpression_Delete(p->pExpression);
    TStatement_Delete(p->pStatement);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TDoStatement_Destroy(TDoStatement *p) _default
{
    TExpression_Delete(p->pExpression);
    TStatement_Delete(p->pStatement);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
    TScannerItemList_Destroy(&p->ClueList4);
}

void TExpressionStatement_Destroy(TExpressionStatement *p) _default
{
    TExpression_Delete(p->pExpression);
    TScannerItemList_Destroy(&p->ClueList0);
}

void TJumpStatement_Destroy(TJumpStatement *p) _default
{
    String_Destroy(&p->Identifier);
    TExpression_Delete(p->pExpression);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TAsmStatement_Destroy(TAsmStatement *p) _default
{
    TScannerItemList_Destroy(&p->ClueList);
}

void TSwitchStatement_Destroy(TSwitchStatement *p) _default
{
    TExpression_Delete(p->pConditionExpression);
    TStatement_Delete(p->pExpression);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TIfStatement_Destroy(TIfStatement *p) _default
{
    TExpression_Delete(p->pConditionExpression);
    TStatement_Delete(p->pStatement);
    TStatement_Delete(p->pElseStatement);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
}

void TStatement_Destroy(TStatement* p)
{
    switch (TYPEOF(p))
    {
    case TExpressionStatement_ID:
        TExpressionStatement_Destroy((TExpressionStatement*)p);
        break;
    case TSwitchStatement_ID:
        TSwitchStatement_Destroy((TSwitchStatement*)p);
        break;
    case TLabeledStatement_ID:
        TLabeledStatement_Destroy((TLabeledStatement*)p);
        break;
    case TForStatement_ID:
        TForStatement_Destroy((TForStatement*)p);
        break;
    case TJumpStatement_ID:
        TJumpStatement_Destroy((TJumpStatement*)p);
        break;
    case TAsmStatement_ID:
        TAsmStatement_Destroy((TAsmStatement*)p);
        break;
    case TCompoundStatement_ID:
        TCompoundStatement_Destroy((TCompoundStatement*)p);
        break;
    case TIfStatement_ID:
        TIfStatement_Destroy((TIfStatement*)p);
        break;
    case TDoStatement_ID:
        TDoStatement_Destroy((TDoStatement*)p);
        break;

    case TWhileStatement_ID:
        TWhileStatement_Destroy((TWhileStatement*)p);
        break;
    default:
        ASSERT(false);
        break;
    }
}

void TBlockItem_Destroy(TBlockItem* p)
{

    switch (TYPEOF(p))
    {
    case TDeclaration_ID:
        TDeclaration_Destroy((TDeclaration*)p);
        break;
    case TJumpStatement_ID:
        TJumpStatement_Destroy((TJumpStatement*)p);
        break;
    case TAsmStatement_ID:
        TAsmStatement_Destroy((TAsmStatement*)p);
        break;
    case TSwitchStatement_ID:
        TSwitchStatement_Destroy((TSwitchStatement*)p);
        break;
    case TExpressionStatement_ID:
        TExpressionStatement_Destroy((TExpressionStatement*)p);
        break;
    case TCompoundStatement_ID:
        TCompoundStatement_Destroy((TCompoundStatement*)p);
        break;
    case TLabeledStatement_ID:
        TLabeledStatement_Destroy((TLabeledStatement*)p);
        break;
    case TForStatement_ID:
        TForStatement_Destroy((TForStatement*)p);
        break;
    case TWhileStatement_ID:
        TWhileStatement_Destroy((TWhileStatement*)p);
        break;
    case TIfStatement_ID:
        TIfStatement_Destroy((TIfStatement*)p);
        break;
    case TDoStatement_ID:
        TDoStatement_Destroy((TDoStatement*)p);
        break;
        //Statement
    default:
        ASSERT(false);
        break;
    }

}

void TPrimaryExpressionValue_Destroy(TPrimaryExpressionValue* p) _default
{
    String_Destroy(&p->lexeme);
    TExpression_Delete(p->pExpressionOpt);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TPostfixExpressionCore_Destroy(TPostfixExpressionCore* p) _default
{
    String_Destroy(&p->lexeme);
    TExpression_Delete(p->pExpressionLeft);
    TExpression_Delete(p->pExpressionRight);
    TInitializerList_Destroy(&p->InitializerList);
    String_Destroy(&p->Identifier);
    TTypeName_Delete(p->pTypeName);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
    TScannerItemList_Destroy(&p->ClueList4);
}

void TBinaryExpression_Destroy(TBinaryExpression* p) _default
{
    TExpression_Delete(p->pExpressionLeft);
    TExpression_Delete(p->pExpressionRight);
    TScannerItemList_Destroy(&p->ClueList00);
}

void TUnaryExpressionOperator_Destroy(TUnaryExpressionOperator* p) _default
{
    TExpression_Delete(p->pExpressionRight);
    TTypeName_Destroy(&p->TypeName);
    TScannerItemList_Destroy(&p->ClueList00);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TCastExpressionType_Destroy(TCastExpressionType* p) _default
{
    TExpression_Delete(p->pExpression);
    TTypeName_Destroy(&p->TypeName);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TTernaryExpression_Destroy(TTernaryExpression* p) _default
{
    TExpression_Delete(p->pExpressionLeft);
    TExpression_Delete(p->pExpressionMiddle);
    TExpression_Delete(p->pExpressionRight);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TPrimaryExpressionLiteralItem_Destroy(TPrimaryExpressionLiteralItem *p) _default
{
    String_Destroy(&p->lexeme);
    TScannerItemList_Destroy(&p->ClueList0);
}

void TPrimaryExpressionLiteral_Destroy(TPrimaryExpressionLiteral* p) _default
{
    TPrimaryExpressionLiteralItemList_Destroy(&p->List);
}

void TExpression_Destroy(TExpression* p)
{
    switch (TYPEOF(p))
    {
        CASE(TPrimaryExpressionLiteral) :
            TPrimaryExpressionLiteral_Destroy((TPrimaryExpressionLiteral*)p);
        break;
        CASE(TPrimaryExpressionValue) :
            TPrimaryExpressionValue_Destroy((TPrimaryExpressionValue*)p);
        break;
        CASE(TPostfixExpressionCore) :
            TPostfixExpressionCore_Destroy((TPostfixExpressionCore*)p);
        break;
        CASE(TBinaryExpression) :
            TBinaryExpression_Destroy((TBinaryExpression*)p);
        break;
        CASE(TUnaryExpressionOperator) :
            TUnaryExpressionOperator_Destroy((TUnaryExpressionOperator*)p);
        break;
        CASE(TCastExpressionType) :
            TCastExpressionType_Destroy((TCastExpressionType*)p);
        break;

        CASE(TTernaryExpression) :
            TTernaryExpression_Destroy((TTernaryExpression*)p);
        break;

    default:
        ASSERT(false);
        break;
    }

}

void TEofDeclaration_Destroy(TEofDeclaration* p) _default
{
    TScannerItemList_Destroy(&p->ClueList0);
}

void TStaticAssertDeclaration_Destroy(TStaticAssertDeclaration* p) _default
{
    TExpression_Delete(p->pConstantExpression);
    String_Destroy(&p->Text);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
    TScannerItemList_Destroy(&p->ClueList4);
    TScannerItemList_Destroy(&p->ClueList5);
}

void TEnumerator_Destroy(TEnumerator* p) _default
{
    String_Destroy(&p->Name);
    TExpression_Delete(p->pExpression);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TEnumeratorList_Destroy(TEnumeratorList* p)
{
    List_Destroy(TEnumerator, p);
}

void TEnumSpecifier_Destroy(TEnumSpecifier* p) _default
{
    String_Destroy(&p->Name);
    TEnumeratorList_Destroy(&p->EnumeratorList);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
}


void TStructUnionSpecifier_Destroy(TStructUnionSpecifier* p)
{
    String_Destroy(&p->Name);
    String_Destroy(&p->StereotypeStr);
    ArrayT_Destroy(TAnyStructDeclaration, &p->StructDeclarationList);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
}

void TSingleTypeSpecifier_Destroy(TSingleTypeSpecifier* p) _default
{
    String_Destroy(&p->TypedefName);
    TScannerItemList_Destroy(&p->ClueList0);
}

const char* TSingleTypeSpecifier_GetTypedefName(TSingleTypeSpecifier* p)
{
    const char* result = NULL;
    if (p->Token == TK_IDENTIFIER)
    {
        result = p->TypedefName;
    }
    return result;
}

void TTypeSpecifier_Destroy(TTypeSpecifier* p)
{
    switch (TYPEOF(p))
    {
    case TSingleTypeSpecifier_ID:
        TSingleTypeSpecifier_Destroy((TSingleTypeSpecifier*)p);
        break;
    case TEnumSpecifier_ID:
        TEnumSpecifier_Destroy((TEnumSpecifier*)p);
        break;
    case TStructUnionSpecifier_ID:
        TStructUnionSpecifier_Destroy((TStructUnionSpecifier*)p);
        break;
    default:
        ASSERT(false);
        break;
    }
}


void TDeclarator_Destroy(TDeclarator* p) _default
{
    TPointerList_Destroy(&p->PointerList);
    TDirectDeclarator_Delete(p->pDirectDeclarator);
    TScannerItemList_Destroy(&p->ClueList);
}

void TInitDeclarator_Destroy(TInitDeclarator* p) _default
{
    TDeclarator_Delete(p->pDeclarator);
    TInitializer_Delete(p->pInitializer);
    TScannerItemList_Destroy(&p->ClueList00);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TParameterTypeList_Destroy(TParameterTypeList* p);

void TDirectDeclarator_Destroy(TDirectDeclarator* p) _default
{
    String_Destroy(&p->Identifier);
    TDeclarator_Delete(p->pDeclarator);
    TDirectDeclarator_Delete(p->pDirectDeclarator);
    TParameterTypeList_Destroy(&p->Parameters);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
    TScannerItemList_Destroy(&p->ClueList3);
}

//bool TDeclarator_IsPointer(TDeclarator* p)
//{
  //  return TPointerList_IsPointer(&p->PointerList);
//}

//bool TDeclarator_IsAutoPointer(TDeclarator* p)
//{
  //  return TPointerList_IsAutoPointer(&p->PointerList);
//}

TSpecifier* TSpecifierQualifierList_GetMainSpecifier(TSpecifierQualifierList* p)
{
    TSpecifier* pSpecifier = NULL;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];
        if (IS_TYPE(pSpecifierQualifier , TSingleTypeSpecifier_ID) ||
            IS_TYPE(pSpecifierQualifier,TStructUnionSpecifier_ID) ||
            IS_TYPE(pSpecifierQualifier, TEnumSpecifier_ID))
        {
            pSpecifier = pSpecifierQualifier;
            break;
        }
    }
    return pSpecifier;
}

const char* TSpecifierQualifierList_GetTypedefName(TSpecifierQualifierList* p)
{
    const char* typedefName = NULL;

    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];


        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifierQualifier_As_TSingleTypeSpecifier(pSpecifierQualifier);
        if (pSingleTypeSpecifier &&
            pSingleTypeSpecifier->Token == TK_IDENTIFIER)
        {
            typedefName = pSingleTypeSpecifier->TypedefName;
            break;
        }
    }
    return typedefName;
}

bool TSpecifierQualifierList_IsTypedefQualifier(TSpecifierQualifierList* p)
{
    bool bResult = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];
        TStorageSpecifier* pStorageSpecifier =
            TSpecifierQualifier_As_TStorageSpecifier(pSpecifierQualifier);
        if (pStorageSpecifier &&
            pStorageSpecifier->bIsTypedef)
        {
            bResult = true;
            break;
        }
    }
    return bResult;
}

bool TSpecifierQualifierList_IsChar(TSpecifierQualifierList* p)
{
    bool bResult = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];

        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifierQualifier_As_TSingleTypeSpecifier(pSpecifierQualifier);
        if (pSingleTypeSpecifier &&
            pSingleTypeSpecifier->Token == TK_char)
        {
            bResult = true;
            break;
        }
    }
    return bResult;
}


bool TSpecifierQualifierList_IsAnyInteger(TSpecifierQualifierList* p)
{
    bool bResult = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];
        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifierQualifier_As_TSingleTypeSpecifier(pSpecifierQualifier);
        if (pSingleTypeSpecifier &&
            (pSingleTypeSpecifier->Token == TK_INT ||
                pSingleTypeSpecifier->Token == TK_SHORT ||
                pSingleTypeSpecifier->Token == TK_SIGNED ||
                pSingleTypeSpecifier->Token == TK_UNSIGNED ||
                pSingleTypeSpecifier->Token == TK__INT8 ||
                pSingleTypeSpecifier->Token == TK__INT16 ||
                pSingleTypeSpecifier->Token == TK__INT32 ||
                pSingleTypeSpecifier->Token == TK__INT64 ||
                pSingleTypeSpecifier->Token == TK__WCHAR_T)
            )
        {
            bResult = true;
            break;
        }
    }
    return bResult;
}


bool TSpecifierQualifierList_IsAnyFloat(TSpecifierQualifierList* p)
{
    bool bResult = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];
        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifierQualifier_As_TSingleTypeSpecifier(pSpecifierQualifier);
        if (pSingleTypeSpecifier &&
            (pSingleTypeSpecifier->Token == TK_DOUBLE ||
                pSingleTypeSpecifier->Token == TK_FLOAT))
        {
            bResult = true;
            break;
        }
    }
    return bResult;
}

bool TSpecifierQualifierList_IsBool(TSpecifierQualifierList* p)
{
    bool bResult = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifierQualifier = p->pData[i];
        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifierQualifier_As_TSingleTypeSpecifier(pSpecifierQualifier);

        if (pSingleTypeSpecifier &&
            pSingleTypeSpecifier->Token == TK__BOOL)
        {
            bResult = true;
            break;
        }
    }
    return bResult;
}

const char* TDeclarator_GetName(TDeclarator*   p)
{
    if (p == NULL)
    {
        return NULL;
    }
    TDirectDeclarator* pDirectDeclarator = p->pDirectDeclarator;
    while (pDirectDeclarator != NULL)
    {
        if (pDirectDeclarator->Identifier != NULL &&
            pDirectDeclarator->Identifier[0] != 0)
        {
            return pDirectDeclarator->Identifier;
        }

        if (pDirectDeclarator->pDeclarator)
        {
            const char* name =
                TDeclarator_GetName(pDirectDeclarator->pDeclarator);
            if (name != NULL)
            {
                return name;
            }
        }
        pDirectDeclarator =
            pDirectDeclarator->pDirectDeclarator;
    }
    return NULL;
}

const char* TInitDeclarator_FindName(TInitDeclarator* p)
{
    ASSERT(p->pDeclarator != NULL);
    return TDeclarator_GetName(p->pDeclarator);
}



void TAlignmentSpecifier_Destroy(TAlignmentSpecifier* p) _default
{
    String_Destroy(&p->TypeName);
}



void TStructDeclaration_Destroy(TStructDeclaration* p)
{
    List_Destroy(TInitDeclarator, &p->DeclaratorList);
    TSpecifierQualifierList_Destroy(&p->SpecifierQualifierList);
    //TTypeQualifier_Destroy(&p->Qualifier);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TAnyStructDeclaration_Destroy(TAnyStructDeclaration* p)
{
    switch (TYPEOF(p))
    {
    case TStructDeclaration_ID:
        TStructDeclaration_Destroy((TStructDeclaration*)p);
        break;
    case TEofDeclaration_ID:
        TEofDeclaration_Destroy((TEofDeclaration*)p);
        break;
    case TStaticAssertDeclaration_ID:
        TStaticAssertDeclaration_Destroy((TStaticAssertDeclaration*)p);
        break;
    default:
        ASSERT(false);
        break;
    }
}

bool TPointerList_IsAutoPointer(TPointerList* pPointerlist)
{
    bool bIsPointer = false;
    bool bIsAuto = false;
    if (pPointerlist)
    {
        //ForEachListItem(TPointer, pItem, pPointerlist)
        TPointer* pItem = pPointerlist->pHead;
        //for (T * var = (list)->pHead; var != NULL; var = var->pNext)
        while (pItem)
        {
            if (pItem->Token == TK_ASTERISK)
            {
                bIsPointer = true;
            }

            for (int i = 0; i < pItem->Qualifier.Size; i++)
            {
                TTypeQualifier* pQualifier = pItem->Qualifier.pData[i];
                if (pQualifier->Token == TK__AUTO ||
                    pQualifier->Token == TK_OWN_QUALIFIER)
                {
                    bIsAuto = true;
                    break;
                }
            }
            if (bIsAuto && bIsPointer)
                break;

            pItem = pItem->pNext;
        }
    }
    return bIsAuto;
}

void TPointerList_Destroy(TPointerList* p)
{
    List_Destroy(TPointer, p);
}

bool TPointerList_IsPointer(TPointerList* pPointerlist)
{
    bool bIsPointer = false;
    if (pPointerlist)
    {
        ForEachListItem(TPointer, pItem, pPointerlist)
        {
            if (pItem->Token == TK_ASTERISK)
            {
                bIsPointer = true;
                break;
            }
        }
    }
    return bIsPointer;
}


const char * TPointerList_GetSize(TPointerList* pPointerlist)
{
    const char* pszResult = NULL;

    if (pPointerlist)
    {
        ForEachListItem(TPointer, pItem, pPointerlist)
        {
            if (pItem->Token != TK_ASTERISK)
            {

                for (int i = 0; i < pItem->Qualifier.Size; i++)
                {
                    TTypeQualifier* pQualifier = pItem->Qualifier.pData[i];

                    if (pQualifier->Token == TK__SIZE)
                    {
                        pszResult = pQualifier->SizeIdentifier;
                        break;
                    }
                }

            }
        }
    }
    return pszResult;
}

bool TPointerList_IsPointerN(TPointerList* pPointerlist, int n)
{
    int k = 0;
    if (pPointerlist)
    {
        ForEachListItem(TPointer, pItem, pPointerlist)
        {
            if (pItem->Token == TK_ASTERISK)
            {
                k++;
            }
        }
    }
    return k == n;
}

bool TPointerList_IsPointerToObject(TPointerList* pPointerlist)
{
    bool bResult = false;
    TPointer *pPointer = NULL;

    pPointer = pPointerlist->pHead;
    if (pPointer && pPointer->Token == TK_ASTERISK)
    {
        pPointer = pPointer->pNext;
        if (pPointer == NULL)
        {
            bResult = true;
        }
    }

    return bResult;
}


bool TPointerList_IsAutoPointerToObject(TPointerList* pPointerlist)
{
    bool bResult = false;
    TPointer *pPointer = NULL;

    pPointer = pPointerlist->pHead;
    if (pPointer && pPointer->Token == TK_ASTERISK)
    {
        pPointer = pPointer->pNext;
        if (pPointer && pPointer->Token == TK__AUTO)
        {
            pPointer = pPointer->pNext;
            if (pPointer == NULL)
            {
                bResult = true;
            }
        }
    }

    return bResult;
}



bool TPointerList_IsAutoPointerToPointer(TPointerList* pPointerlist)
{
    bool bResult = false;
    TPointer *pPointer = NULL;

    pPointer = pPointerlist->pHead;
    if (pPointer && pPointer->Token == TK_ASTERISK)
    {
        pPointer = pPointer->pNext;
        if (pPointer && pPointer->Token == TK_ASTERISK)
        {
            pPointer = pPointer->pNext;
            if (pPointer && pPointer->Token == TK__AUTO)
            {

                pPointer = pPointer->pNext;
                if (pPointer == NULL)
                {
                    bResult = true;
                }

            }
        }
    }

    return bResult;
}


bool TPointerList_IsAutoPointerToAutoPointer(TPointerList* pPointerlist)
{
    bool bResult = false;
    TPointer *pPointer = NULL;

    pPointer = pPointerlist->pHead;
    if (pPointer && pPointer->Token == TK_ASTERISK)
    {
        pPointer = pPointer->pNext;
        if (pPointer && pPointer->Token == TK__AUTO)
        {
            pPointer = pPointer->pNext;
            if (pPointer && pPointer->Token == TK_ASTERISK)
            {
                pPointer = pPointer->pNext;
                if (pPointer && pPointer->Token == TK__AUTO)
                {
                    pPointer = pPointer->pNext;
                    if (pPointer == NULL)
                    {
                        bResult = true;
                    }
                }
            }
        }
    }

    return bResult;
}



void TPointer_Destroy(TPointer* p) _default
{
    TTypeQualifierList_Destroy(&p->Qualifier);
    TScannerItemList_Destroy(&p->ClueList0);
}


void TTypeQualifierList_Destroy(TTypeQualifierList* p) _default
{
    for (int i = 0; i < p->Size; i++)
    {
        TTypeQualifier_Delete(p->pData[i]);
    }
    free((void*)p->pData);
}

void TTypeQualifierList_Reserve(TTypeQualifierList* p, int n) _default
{
    if (n > p->Capacity)
    {
        TTypeQualifier** pnew = p->pData;
        pnew = (TTypeQualifier**)realloc(pnew, n * sizeof(TTypeQualifier*));
        if (pnew)
        {
            p->pData = pnew;
            p->Capacity = n;
        }
    }
}

void TTypeQualifierList_PushBack(TTypeQualifierList* p, TTypeQualifier* pItem) _default
{
    if (p->Size + 1 > p->Capacity)
    {
        int n = p->Capacity * 2;
        if (n == 0)
        {
          n = 1;
        }
        TTypeQualifierList_Reserve(p, n);
    }
    p->pData[p->Size] = pItem;
    p->Size++;
}

void TTypeQualifier_Destroy(TTypeQualifier* p) _default
{
    String_Destroy(&p->SizeIdentifier);
    TScannerItemList_Destroy(&p->ClueList0);
}

void TStorageSpecifier_Destroy(TStorageSpecifier* p) _default
{
    TScannerItemList_Destroy(&p->ClueList0);
}

void TAtomicTypeSpecifier_Destroy(TAtomicTypeSpecifier* p) _default
{
    TTypeName_Destroy(&p->TypeName);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
    TScannerItemList_Destroy(&p->ClueList2);
}

void TSpecifierQualifierList_Destroy(TSpecifierQualifierList* pDeclarationSpecifiers) _default
{
    for (int i = 0; i < pDeclarationSpecifiers->Size; i++)
    {
        TSpecifierQualifier_Delete(pDeclarationSpecifiers->pData[i]);
    }
    free((void*)pDeclarationSpecifiers->pData);
}


void TSpecifierQualifierList_Reserve(TSpecifierQualifierList* p, int n) _default
{
    if (n > p->Capacity)
    {
        TSpecifierQualifier** pnew = p->pData;
        pnew = (TSpecifierQualifier**)realloc(pnew, n * sizeof(TSpecifierQualifier*));
        if (pnew)
        {
            p->pData = pnew;
            p->Capacity = n;
        }
    }
}

void TSpecifierQualifierList_PushBack(TSpecifierQualifierList* p, TSpecifierQualifier* pItem) _default
{
    if (p->Size + 1 > p->Capacity)
    {
        int n = p->Capacity * 2;
        if (n == 0)
        {
          n = 1;
        }
        TSpecifierQualifierList_Reserve(p, n);
    }
    p->pData[p->Size] = pItem;
    p->Size++;
}




bool TSpecifierQualifierList_CanAdd(TSpecifierQualifierList* p, Tokens token, const char* lexeme)
{
    bool bResult = false;

    bool bStruct = false;
    bool bEnum = false;

    bool bTypeDef = false;
    bool bInt = false;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifierQualifier* pSpecifier = p->pData[i];
        switch (TYPEOF(pSpecifier))
        {
            CASE(TSingleTypeSpecifier) :
            {     TSingleTypeSpecifier* pTSingleTypeSpecifier =
                (TSingleTypeSpecifier*)pSpecifier;
            switch (pTSingleTypeSpecifier->Token)
            {
            case TK_INT:
                bInt = true;
                break;
            case TK_DOUBLE:
                break;

            case TK_IDENTIFIER:
                bTypeDef = true;
                break;
            default:
                ASSERT(false);
                break;
            }
            }
            break;

            CASE(TStructUnionSpecifier) :
                bStruct = true;
            break;

            CASE(TEnumSpecifier) :
                bEnum = true;
            break;

            CASE(TStorageSpecifier) :

                break;
            CASE(TTypeQualifier) :

                break;
            CASE(TFunctionSpecifier) :

                break;
            CASE(TAlignmentSpecifier) :

                break;
        default:
            ASSERT(false);
            break;
        }
    }


    if (token == TK_IDENTIFIER)
    {
        if (!bTypeDef && !bInt)
        {
            //Exemplo que se quer evitar
            //typedef int X;
            //void F(int X)
            //nao pode ter nada antes
            bResult = true;
        }
    }
    else
    {
        //verificar combinacoes unsigned float etc.
        bResult = true;
    }
    return bResult;

}

bool TDeclarationSpecifiers_CanAddSpeficier(TDeclarationSpecifiers* pDeclarationSpecifiers,
    Tokens token,
    const char* lexeme)
{
    bool bResult = false;
    bool bStruct = false;
    bool bEnum = false;


    bool bTypeDef = false;
    bool bInt = false;

    for (int i = 0; i < pDeclarationSpecifiers->Size; i++)
    {
        TSpecifier* pSpecifier = pDeclarationSpecifiers->pData[i];

        switch (TYPEOF(pSpecifier))
        {
            CASE(TSingleTypeSpecifier) :
            {     TSingleTypeSpecifier* pTSingleTypeSpecifier =
                (TSingleTypeSpecifier*)pSpecifier;
            switch (pTSingleTypeSpecifier->Token)
            {
            case TK_INT:
                bInt = true;
                break;
            case TK_DOUBLE:
            case TK_IDENTIFIER:
                bTypeDef = true;
                break;
            default:
                ASSERT(false);
                break;
            }
            }
            break;

            CASE(TStructUnionSpecifier) :
                bStruct = true;
            break;

            CASE(TEnumSpecifier) :
                bEnum = true;
            break;

            CASE(TStorageSpecifier) :

                break;
            CASE(TTypeQualifier) :

                break;
            CASE(TFunctionSpecifier) :

                break;
            CASE(TAlignmentSpecifier) :

                break;

        default:
            ASSERT(false);
            break;
        }
    }


    if (token == TK_IDENTIFIER)
    {
        if (!bTypeDef && !bInt)
        {
            //Exemplo que se quer evitar
            //typedef int X;
            //void F(int X)
            //nao pode ter nada antes
            bResult = true;
        }
    }
    else
    {
        //verificar combinacoes unsigned float etc.
        bResult = true;
    }
    return bResult;
}

const char* TDeclarationSpecifiers_GetTypedefName(TDeclarationSpecifiers* pDeclarationSpecifiers)
{
    if (pDeclarationSpecifiers == NULL)
    {
        return NULL;
    }
    const char* typeName = NULL;


    for (int i = 0; i < pDeclarationSpecifiers->Size; i++)
    {
        TSpecifier* pItem = pDeclarationSpecifiers->pData[i];

        TSingleTypeSpecifier* pSingleTypeSpecifier =
            TSpecifier_As_TSingleTypeSpecifier(pItem);
        if (pSingleTypeSpecifier != NULL)
        {
            if (pSingleTypeSpecifier->Token == TK_IDENTIFIER)
            {
                typeName = pSingleTypeSpecifier->TypedefName;
                break;
            }
        }
    }
    return typeName;
}

void TSpecifierQualifier_Destroy(TSpecifierQualifier* pItem)
{
    switch (TYPEOF(pItem))
    {
        CASE(TSingleTypeSpecifier) :
            TSingleTypeSpecifier_Destroy((TSingleTypeSpecifier*)pItem);
        break;

        CASE(TStructUnionSpecifier) :
            TStructUnionSpecifier_Destroy((TStructUnionSpecifier*)pItem);
        break;

        CASE(TEnumSpecifier) :
            TEnumSpecifier_Destroy((TEnumSpecifier*)pItem);
        break;

        CASE(TStorageSpecifier) :
            TStorageSpecifier_Destroy((TStorageSpecifier*)pItem);
        break;
        CASE(TTypeQualifier) :
            TTypeQualifier_Destroy((TTypeQualifier*)pItem);
        break;
        CASE(TFunctionSpecifier) :
            TFunctionSpecifier_Destroy((TFunctionSpecifier*)pItem);
        break;

        CASE(TAlignmentSpecifier) :
            TAlignmentSpecifier_Destroy((TAlignmentSpecifier*)pItem);
        break;


    default:
        ASSERT(false);
        break;
    }
}


void TSpecifier_Destroy(TSpecifier* pItem)
{

  
        switch (TYPEOF(pItem))
        {
            CASE(TSingleTypeSpecifier) :
                TSingleTypeSpecifier_Destroy((TSingleTypeSpecifier*)pItem);
            break;

            CASE(TStructUnionSpecifier) :
                TStructUnionSpecifier_Destroy((TStructUnionSpecifier*)pItem);
            break;

            CASE(TEnumSpecifier) :
                TEnumSpecifier_Destroy((TEnumSpecifier*)pItem);
            break;

            CASE(TStorageSpecifier) :
                TStorageSpecifier_Destroy((TStorageSpecifier*)pItem);
            break;
            CASE(TTypeQualifier) :
                TTypeQualifier_Destroy((TTypeQualifier*)pItem);
            break;
            CASE(TFunctionSpecifier) :
                TFunctionSpecifier_Destroy((TFunctionSpecifier*)pItem);
            break;

            CASE(TAlignmentSpecifier) :
                TAlignmentSpecifier_Destroy((TAlignmentSpecifier*)pItem);
            break;


        default:
            ASSERT(false);
            break;
        }
    

}

void TDeclarationSpecifiers_Destroy(TDeclarationSpecifiers* pDeclarationSpecifiers) _default
{
    for (int i = 0; i < pDeclarationSpecifiers->Size; i++)
    {
        TSpecifier_Delete(pDeclarationSpecifiers->pData[i]);
    }
    free((void*)pDeclarationSpecifiers->pData);
}


void TDeclarationSpecifiers_Reserve(TDeclarationSpecifiers* p, int n) _default
{
    if (n > p->Capacity)
    {
        TSpecifier** pnew = p->pData;
        pnew = (TSpecifier**)realloc(pnew, n * sizeof(TSpecifier*));
        if (pnew)
        {
            p->pData = pnew;
            p->Capacity = n;
        }
    }
}

void TDeclarationSpecifiers_PushBack(TDeclarationSpecifiers* p, TSpecifier* pItem) _default
{
    if (p->Size + 1 > p->Capacity)
    {
        int n = p->Capacity * 2;
        if (n == 0)
        {
          n = 1;
        }
        TDeclarationSpecifiers_Reserve(p, n);
    }
    p->pData[p->Size] = pItem;
    p->Size++;
}


TDeclarator* TDeclaration_FindDeclarator(TDeclaration*  p, const char* name)
{
    if (p == NULL)
    {
        return NULL;
    }
    TDeclarator*  pResult = NULL;

    ForEachListItem(TInitDeclarator, pInitDeclarator, &p->InitDeclaratorList)
    {
        if (pInitDeclarator->pDeclarator &&
            pInitDeclarator->pDeclarator->pDirectDeclarator &&
            pInitDeclarator->pDeclarator->pDirectDeclarator->Identifier)
        {
            if (strcmp(pInitDeclarator->pDeclarator->pDirectDeclarator->Identifier, name) == 0)
            {
                pResult = pInitDeclarator->pDeclarator;
                break;
            }
        }
    }
    return pResult;
}

void TFunctionSpecifier_Destroy(TFunctionSpecifier* p) _default
{
    TScannerItemList_Destroy(&p->ClueList0);
}


bool TDeclaration_Is_StructOrUnionDeclaration(TDeclaration* p)
{
    bool bIsStructOrUnion = false;
    for (int i = 0; i < p->Specifiers.Size; i++)
    {
        TSpecifier* pItem = p->Specifiers.pData[i];
        if (TSpecifier_As_TStructUnionSpecifier(pItem))
        {
            bIsStructOrUnion = true;
            break;
        }

    }
    return bIsStructOrUnion;
}


void TDeclaration_Destroy(TDeclaration* p)
{
    //
    List_Destroy(TInitDeclarator, &p->InitDeclaratorList);
    //
    TDeclarationSpecifiers_Destroy(&p->Specifiers);
    TCompoundStatement_Delete(p->pCompoundStatementOpt);
    TScannerItemList_Destroy(&p->ClueList00);
    TScannerItemList_Destroy(&p->ClueList1);
}

void TParameter_Swap(TParameter* a, TParameter* b)
{
    TParameter temp = *a;
    *a = *b;
    *b = temp;
}

const char* TSpecifier_GetTypedefName(TDeclarationSpecifiers* p)
{
    const char* typedefName = NULL;
    for (int i = 0; i < p->Size; i++)
    {
        TSpecifier* pSpecifier = p->pData[i];
        TSingleTypeSpecifier *pSingleTypeSpecifier =
            TSpecifier_As_TSingleTypeSpecifier(pSpecifier);
        if (pSingleTypeSpecifier &&
            pSingleTypeSpecifier->Token == TK_IDENTIFIER)
        {
            typedefName = pSingleTypeSpecifier->TypedefName;
        }
    }
    return typedefName;
}

const char* TParameter_GetTypedefName(TParameter* p)
{
    return TSpecifier_GetTypedefName(&p->Specifiers);
}

bool TDeclarator_IsDirectPointer(TDeclarator* p)
{
    int n = 0;
    ForEachListItem(TPointer, pPointer, &p->PointerList)
    {
        if (pPointer->Token == TK_ASTERISK)
        {
            n++;
            if (n > 1)
            {
                break;
            }
        }
    }
    return n == 1;
}

bool TParameter_IsDirectPointer(TParameter* p)
{
    return TDeclarator_IsDirectPointer(&p->Declarator);
}

const char* TParameter_GetName(TParameter* p)
{
    return TDeclarator_GetName(&p->Declarator);
}

void TParameter_Destroy(TParameter* p) _default
{
    TDeclarationSpecifiers_Destroy(&p->Specifiers);
    TDeclarator_Destroy(&p->Declarator);
    TScannerItemList_Destroy(&p->ClueList00);
}

void TParameterTypeList_Destroy(TParameterTypeList* p)
{
    TParameterList_Destroy(&p->ParameterList);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}

bool TAnyDeclaration_Is_StructOrUnionDeclaration(TAnyDeclaration* pAnyDeclaration)
{
    TDeclaration *  pDeclaration = TAnyDeclaration_As_TDeclaration(pAnyDeclaration);
    if (pDeclaration != NULL)
    {
        return TDeclaration_Is_StructOrUnionDeclaration(pDeclaration);
    }

    return false;
}


bool TDeclarationSpecifiers_IsTypedef(TDeclarationSpecifiers* pDeclarationSpecifiers)
{
    bool bResult = false;
    for (int i = 0; i < pDeclarationSpecifiers->Size; i++)
    {
        TSpecifier* pItem = pDeclarationSpecifiers->pData[i];
        switch (TYPEOF(pItem))
        {
            CASE(TStorageSpecifier) :
            {
                TStorageSpecifier* pStorageSpecifier =
                    (TStorageSpecifier*)pItem;

                if (pStorageSpecifier->bIsTypedef)
                {
                    bResult = true;
                }
            }

            break;
        default:
            ASSERT(false);
            break;
        }

        if (bResult)
        {
            break;
        }
    }
    return bResult;
}

bool TAnyDeclaration_IsTypedef(TAnyDeclaration* pDeclaration)
{
    bool bResult = false;
    switch (TYPEOF(pDeclaration))
    {
    case TDeclaration_ID:
    {
        TDeclaration* p = (TDeclaration*)pDeclaration;
        bResult = TDeclarationSpecifiers_IsTypedef(&p->Specifiers);
    }
    break;
    default:
        ASSERT(false);
        break;
    }
    return bResult;
}

int TAnyDeclaration_GetFileIndex(TAnyDeclaration* pDeclaration)
{
    int result = -1;
    switch (TYPEOF(pDeclaration))
    {
    case TDeclaration_ID:
        result = ((TDeclaration*)pDeclaration)->FileIndex;
        break;
    case TStaticAssertDeclaration_ID:
        break;
    default:
        ASSERT(false);
        break;
    }
    return result;
}
void TAnyDeclaration_Destroy(TAnyDeclaration* pDeclaration)
{
    switch (TYPEOF(pDeclaration))
    {
    case TEofDeclaration_ID:
        TEofDeclaration_Destroy((TEofDeclaration*)pDeclaration);
        break;

    case TDeclaration_ID:
        TDeclaration_Destroy((TDeclaration*)pDeclaration);
        break;
    default:
        ASSERT(false);
        break;
    }
}

void TDesignation_Destroy(TDesignation* pDesignation)
{

    List_Destroy(TDesignator, &pDesignation->DesignatorList);
    TScannerItemList_Destroy(&pDesignation->ClueList0);

}

void TDesignator_Destroy(TDesignator* p) _default
{
    String_Destroy(&p->Name);
    TExpression_Delete(p->pExpression);
    TScannerItemList_Destroy(&p->ClueList0);
    TScannerItemList_Destroy(&p->ClueList1);
}


void  TInitializerListType_Destroy(TInitializerListType* pTInitializerListType) _default
{
    TInitializerList_Destroy(&pTInitializerListType->InitializerList);
    TScannerItemList_Destroy(&pTInitializerListType->ClueList00);
    TScannerItemList_Destroy(&pTInitializerListType->ClueList0);
    TScannerItemList_Destroy(&pTInitializerListType->ClueList1);
}

void TInitializerList_Destroy(TInitializerList* p)
{
    List_Destroy(TInitializerListItem, p)
}

void TInitializer_Destroy(TInitializer* p)
{
    if (IS_TYPE(p, TInitializerListType_ID))
    {
        TInitializerListType_Destroy((TInitializerListType*)p);

    }
    else
    {
        TExpression_Destroy((TExpression*)p);
    }

}

void TInitializerListItem_Destroy(TInitializerListItem* p)
{
    List_Destroy(TDesignator, &p->DesignatorList);
    TInitializer_Delete(p->pInitializer);
    TScannerItemList_Destroy(&p->ClueList);
}


TDeclaration* TProgram_FindDeclaration(TProgram* p, const char* name)
{
    TTypePointer* pt = SymbolMap_Find(&p->GlobalScope, name);
    if (pt != NULL &&
        IS_TYPE(pt, TDeclaration_ID))
    {
        return (TDeclaration*)pt;
    }
    return NULL;
    //return DeclarationsMap_FindDeclaration(&p->Symbols, name);
}

TDeclaration* TProgram_FindFunctionDeclaration(TProgram* p, const char* name)
{
    TTypePointer* pt = SymbolMap_Find(&p->GlobalScope, name);
    if (pt != NULL &&
        IS_TYPE(pt,TDeclaration_ID))
    {
        return (TDeclaration*)pt;
    }
    return NULL;
}



//Retorna a declaracao final do tipo entrando em cada typedef. 
TDeclaration* TProgram_GetFinalTypeDeclaration(TProgram* p, const char* typeName)
{
    return SymbolMap_FindTypedefDeclarationTarget(&p->GlobalScope, typeName);
}

#define TPROGRAM_INIT {ARRAYT_INIT, STRARRAY_INIT, SYMBOLMAP_INIT, MACROMAP_INIT}
void TProgram_Init(TProgram* p)
{
    TProgram d = TPROGRAM_INIT;
    *p = d;
}


void TProgram_Destroy(TProgram * p)
{
    ArrayT_Destroy(TAnyDeclaration, &p->Declarations);
    ArrayT_Destroy(TFile, &p->Files2);
    SymbolMap_Destroy(&p->GlobalScope);
    MacroMap_Destroy(&p->Defines);
}



///////////////////////////////////////////

static bool TPostfixExpressionCore_CodePrint2(TPostfixExpressionCore * p,
    int *pResult)
{

    int result = *pResult;

    if (p->pExpressionLeft)
    {
        int left;
        EvaluateConstantExpression(p->pExpressionLeft, &left);
    }

    //if (p->pInitializerList)
    {
        //falta imprimeir typename
        //TTypeName_Print*
        //b = TInitializerList_CodePrint(p->pInitializerList, b, fp);
    }

    switch (p->token)
    {
    case TK_FULL_STOP:
        //fprintf(fp, ".%s", p->Identifier);
        ASSERT(false);
        break;
    case TK_ARROW:
        //fprintf(fp, "->%s", p->Identifier);
        //b = true;
        ASSERT(false);
        break;

    case TK_LEFT_SQUARE_BRACKET:
    {
        int index;
        //fprintf(fp, "[");
        EvaluateConstantExpression(p->pExpressionRight, &index);
        //fprintf(fp, "]");
        ASSERT(false);
    }
    break;

    case TK_LEFT_PARENTHESIS:
    {
        EvaluateConstantExpression(p->pExpressionRight, &result);
    }
    break;

    case TK_PLUSPLUS:
        ASSERT(false);
        break;
    case TK_MINUSMINUS:
        ASSERT(false);
        break;
    default:
        ASSERT(false);
        break;
    }



    if (p->pNext)
    {
        int result2 = result;
        TPostfixExpressionCore_CodePrint2(p->pNext, &result2);
        result = result2;
    }

    return true;
}

//Criado para avaliacao do #if
//Tem que arrumar para fazer os casts do enum
bool EvaluateConstantExpression(TExpression *  p, int *pResult)
{
    int result = -987654321;

    if (p == NULL)
    {
        return false;
    }
    bool b = false;

    switch (TYPEOF(p))
    {
        CASE(TBinaryExpression) :
        {
            TBinaryExpression* pBinaryExpression =
                (TBinaryExpression*)p;

            int left;
            b = EvaluateConstantExpression(pBinaryExpression->pExpressionLeft, &left);

            int right;
            b = EvaluateConstantExpression(pBinaryExpression->pExpressionRight, &right);

            switch (pBinaryExpression->token)
            {
            case TK_ASTERISK:
                result = (left * right);
                b = true;
                break;
            case TK_PLUS_SIGN:
                result = (left + right);
                b = true;
                break;
            case TK_HYPHEN_MINUS:
                result = (left - right);
                b = true;
                break;
            case TK_ANDAND:
                result = (left && right);
                b = true;
                break;
            case TK_OROR:
                result = (left || right);
                b = true;
                break;
            case TK_NOTEQUAL:
                result = (left != right);
                b = true;
                break;
            case TK_EQUALEQUAL:
                result = (left == right);
                b = true;
                break;
            case TK_GREATEREQUAL:
                result = (left >= right);
                b = true;
                break;
            case TK_LESSEQUAL:
                result = (left <= right);
                b = true;
                break;
            case TK_GREATER_THAN_SIGN:
                result = (left > right);
                b = true;
                break;
            case TK_LESS_THAN_SIGN:
                result = (left < right);
                b = true;
                break;
            case TK_AMPERSAND:
                result = (left & right);
                b = true;
                break;
            case TK_GREATERGREATER:
                result = (left >> right);
                b = true;
                break;
            case TK_LESSLESS:
                result = (left << right);
                b = true;
                break;
            case TK_VERTICAL_LINE:
                result = (left | right);
                b = true;
                break;

            case TK_SOLIDUS:
                if (right != 0)
                {
                    result = (left / right);
                    b = true;
                }
                else
                {
                    b = false;
                    //SetError
                }
                break;


            default:
                //TODO ADD THE OPERADOR?
                ASSERT(false);
                b = false;
            }

            //if (pBinaryExpression->)
        }
        break;

        CASE(TTernaryExpression) :
        {
            int e1, e2, e3;
            b = EvaluateConstantExpression(((TTernaryExpression*)p)->pExpressionLeft, &e1);

            b = EvaluateConstantExpression(((TTernaryExpression*)p)->pExpressionMiddle, &e2);

            b = EvaluateConstantExpression(((TTernaryExpression*)p)->pExpressionRight, &e3);
            ASSERT(false);
        }
        break;

        CASE(TPrimaryExpressionValue) :
        {
            TPrimaryExpressionValue* pPrimaryExpressionValue =
                (TPrimaryExpressionValue*)p;

            if (pPrimaryExpressionValue->pExpressionOpt != NULL)
            {
                b = EvaluateConstantExpression(pPrimaryExpressionValue->pExpressionOpt, &result);
            }
            else
            {
                switch (pPrimaryExpressionValue->token)
                {
                case TK_IDENTIFIER:
                    result = 0; //para macro
                    b = true;
                    break;

                case TK_DECIMAL_INTEGER:
                    result = atoi(pPrimaryExpressionValue->lexeme);
                    b = true;
                    break;

                case TK_HEX_INTEGER:
                    result = strtol(pPrimaryExpressionValue->lexeme, NULL, 16);
                    b = true;
                    break;

                case TK_CHAR_LITERAL:
                    if (pPrimaryExpressionValue->lexeme != NULL)
                    {
                        //vem com 'A'
                        result = pPrimaryExpressionValue->lexeme[1];
                        b = true;
                    }
                    else
                    {
                        result = 0;
                    }
                    break;
                default:
                    b = false;
                    ASSERT(0);
                    break;
                }

            }
        }
        break;

        CASE(TPostfixExpressionCore) :
        {
            TPostfixExpressionCore* pPostfixExpressionCore =
                (TPostfixExpressionCore*)p;
            b = TPostfixExpressionCore_CodePrint2(pPostfixExpressionCore, &result);
            ASSERT(false);
        }
        break;

        CASE(TUnaryExpressionOperator) :
        {

            TUnaryExpressionOperator* pTUnaryExpressionOperator =
                (TUnaryExpressionOperator*)p;

            if (pTUnaryExpressionOperator->token == TK_SIZEOF)
            {

                //if (TDeclarationSpecifiers_IsTypedef(pTUnaryExpressionOperator->TypeName.SpecifierQualifierList))
                {

                    //b = TTypeQualifier_CodePrint2(&pTUnaryExpressionOperator->TypeName.qualifiers, fp);
                    //b = TTypeSpecifier_CodePrint2(pTUnaryExpressionOperator->TypeName.pTypeSpecifier, b, fp);
                    // b = TDeclarator_CodePrint(&pTUnaryExpressionOperator->TypeName.declarator, b, fp);


                }
                //else
                {
                    b = EvaluateConstantExpression(pTUnaryExpressionOperator->pExpressionRight, &result);
                }
            }
            else
            {
                int localResult;
                b = EvaluateConstantExpression(pTUnaryExpressionOperator->pExpressionRight, &localResult);
                switch (pTUnaryExpressionOperator->token)
                {
                case TK_EXCLAMATION_MARK:
                    result = !localResult;
                    b = true;
                    break;
                case TK_HYPHEN_MINUS:
                    result = -localResult;
                    b = true;
                    break;
                default:
                    ASSERT(false);
                }
            }


        }
        break;

        CASE(TCastExpressionType) :
        {
            TCastExpressionType * pCastExpressionType =
                (TCastExpressionType*)p;


            //b = TTypeQualifier_CodePrint2(&pCastExpressionType->TypeName.qualifiers, fp);
            //b = TTypeSpecifier_CodePrint2(pCastExpressionType->TypeName.pTypeSpecifier, b, fp);
            //b = TDeclarator_CodePrint(&pCastExpressionType->TypeName.declarator, b, fp);

            b = EvaluateConstantExpression(pCastExpressionType->pExpression, &result);
            ASSERT(false);

        }
        break;

    default:
        ASSERT(false);
    }

    ASSERT(result != -987654321);
    *pResult = result;
    return b;
}



TDeclarationSpecifiers* TDeclaration_GetArgTypeSpecifier(TDeclaration* p, int index)
{
    TDeclarationSpecifiers* pResult = NULL;
    TParameterTypeList *pArguments = TDeclaration_GetFunctionArguments(p);
    int n = 0;
    ForEachListItem(TParameter, pItem, &pArguments->ParameterList)
    {
        if (n == index)
        {
            pResult = &pItem->Specifiers;
            break;
        }
        n++;
    }
    return pResult;
}

TParameterTypeList * TDeclaration_GetFunctionArguments(TDeclaration* p)
{
    TParameterTypeList* pParameterTypeList = NULL;

    if (p->InitDeclaratorList.pHead != NULL)
    {
        if (p->InitDeclaratorList.pHead->pNext == NULL)
        {
            if (p->InitDeclaratorList.pHead->pDeclarator != NULL)
            {
                if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator)
                {
                    if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator->DeclaratorType == TDirectDeclaratorTypeFunction)
                    {
                        pParameterTypeList =
                            &p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator->Parameters;

                    }
                }
            }
        }
    }
    return pParameterTypeList;
}

const char* TDeclaration_GetFunctionName(TDeclaration* p)
{
    const char* functionName = NULL;

    if (p->InitDeclaratorList.pHead != NULL)
    {
        if (p->InitDeclaratorList.pHead->pNext == NULL)
        {
            if (p->InitDeclaratorList.pHead->pDeclarator != NULL)
            {
                if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator)
                {
                    if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator->DeclaratorType == TDirectDeclaratorTypeFunction)
                    {
                        functionName =
                            p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator->Identifier;
                    }
                }
            }
        }
    }
    return functionName;
}

TCompoundStatement* TDeclaration_Is_FunctionDefinition(TDeclaration* p)
{
    TCompoundStatement* pCompoundStatement = NULL;

    if (p->InitDeclaratorList.pHead != NULL)
    {
        if (p->InitDeclaratorList.pHead->pNext == NULL)
        {
            if (p->InitDeclaratorList.pHead->pDeclarator != NULL)
            {
                if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator)
                {
                    if (p->InitDeclaratorList.pHead->pDeclarator->pDirectDeclarator->DeclaratorType== TDirectDeclaratorTypeFunction)
                    {
                        pCompoundStatement = p->pCompoundStatementOpt;
                    }
                }
            }
        }
    }
    return pCompoundStatement;
}

TStructUnionSpecifier* TDeclarationSpecifiers_Find_StructUnionSpecifier(TDeclarationSpecifiers* p)
{
    TStructUnionSpecifier* pStructUnionSpecifier = NULL;

    for (int i = 0; i < p->Size; i++)
    {
        TSpecifier* pDeclarationSpecifier = p->pData[i];

        pStructUnionSpecifier =
            TSpecifier_As_TStructUnionSpecifier(pDeclarationSpecifier);
        if (pStructUnionSpecifier)
        {
            break;
        }
    }
    return pStructUnionSpecifier;
}

TStructUnionSpecifier* TParameter_Is_DirectPointerToStruct(TProgram* program, TParameter* pParameter)
{
    TStructUnionSpecifier* pStructUnionSpecifier = NULL;
    if (TParameter_IsDirectPointer(pParameter))
    {
        const char* typedefName = TParameter_GetTypedefName(pParameter);
        if (typedefName != NULL)
        {
            TDeclaration* pArgType = TProgram_FindDeclaration(program, TParameter_GetTypedefName(pParameter));
            if (pArgType)
            {
                pStructUnionSpecifier =
                    TDeclarationSpecifiers_Find_StructUnionSpecifier(&pArgType->Specifiers);
            }
        }
    }
    return pStructUnionSpecifier;
}

void TPrimaryExpressionLiteralItemList_Destroy(TPrimaryExpressionLiteralItemList* p)
{
    List_Destroy(TPrimaryExpressionLiteralItem, p);
}

const char* TDeclaration_GetArgName(TDeclaration* p, int index)
{
    const char* argName = NULL;
    TParameterTypeList *pArguments = TDeclaration_GetFunctionArguments(p);
    int n = 0;
    ForEachListItem(TParameter, pItem, &pArguments->ParameterList)
    {
        if (n == index)
        {
            argName = TParameter_GetName(pItem);
            break;
        }
        n++;
    }
    return argName;
}


int TDeclaration_GetNumberFuncArgs(TDeclaration* p)
{
    TParameterTypeList *pArguments = TDeclaration_GetFunctionArguments(p);
    int n = 0;
    ForEachListItem(TParameter, pItem, &pArguments->ParameterList)
    {
        n++;
    }
    return n;
}

