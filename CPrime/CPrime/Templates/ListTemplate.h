#pragma once

#include "..\CodePrint.h"

//implementa funcoes especiais
bool ListPlugin_InstanciateSpecialFunctions(TProgram* program,
                             TStructUnionSpecifier* pStructUnionSpecifier,
                             const char* pVariableName,
                             bool bVariableNameIsPointer,
                             BuildType buildType,
    StrBuilder* fp);

//instancia o tipo
bool ListPlugin_InstanciateType(TProgram* program,
                               Options * options,
                               TStructUnionSpecifier* p,
                               bool b, StrBuilder* fp);

//Implementa 'default'
bool ListPlugin_CodePrint(TProgram* program,
                          Options * options,
                          TDeclaration* p,
                          bool b,
                          StrBuilder* fp);
