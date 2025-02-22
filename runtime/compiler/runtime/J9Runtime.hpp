/*******************************************************************************
 * Copyright IBM Corp. and others 2000
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] https://openjdk.org/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0-only WITH Classpath-exception-2.0 OR GPL-2.0-only WITH OpenJDK-assembly-exception-1.0
 *******************************************************************************/

#ifndef J9RUNTIME_INCL
#define J9RUNTIME_INCL

#include "runtime/Runtime.hpp"
#include "codegen/PreprologueConst.hpp"
#include "codegen/PrivateLinkage.hpp"


#if defined(TR_HOST_S390)
void restoreJitEntryPoint(uint8_t* intEP, uint8_t* jitEP);
void saveJitEntryPoint(uint8_t* intEP, uint8_t* jitEP);
#endif



#if defined(TR_HOST_X86)

inline uint16_t jitEntryOffset(void *startPC)
   {
#if defined(TR_HOST_64BIT)
   return J9::PrivateLinkage::LinkageInfo::get(startPC)->getReservedWord();
#else
   return 0;
#endif
   }

inline uint16_t jitEntryJmpInstruction(void *startPC, int32_t startPCToTarget)
   {
   const uint8_t TWO_BYTE_JUMP_INSTRUCTION = 0xEB;
   int32_t displacement = startPCToTarget - (jitEntryOffset(startPC) + 2);
   return (displacement << 8) | TWO_BYTE_JUMP_INSTRUCTION;
   }

void saveFirstTwoBytes(void *startPC, int32_t startPCToSaveArea);
void replaceFirstTwoBytesWithShortJump(void *startPC, int32_t startPCToTarget);
void replaceFirstTwoBytesWithData(void *startPC, int32_t startPCToData);
#endif



#if defined(TR_HOST_POWER)
#define  OFFSET_REVERT_INTP_PRESERVED_FSD                (-4)
#define  OFFSET_REVERT_INTP_FIXED_PORTION                (-(12 + 2 * (int32_t)sizeof(intptr_t)))

#define  OFFSET_SAMPLING_PREPROLOGUE_FROM_STARTPC        (-(16 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_BRANCH_FROM_STARTPC             (-(12 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_METHODINFO_FROM_STARTPC         (-(8 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_PRESERVED_FROM_STARTPC          (-8)
#endif

#if defined(TR_HOST_ARM)
#define  OFFSET_REVERT_INTP_FIXED_PORTION                (-(12 + 2 * (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_PREPROLOGUE_FROM_STARTPC        (-(16 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_BRANCH_FROM_STARTPC             (-(12 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_METHODINFO_FROM_STARTPC                  (-(8 + (int32_t)sizeof(intptr_t)))
#define  OFFSET_SAMPLING_PRESERVED_FROM_STARTPC          (-8)
#define  START_PC_TO_METHOD_INFO_ADDRESS                  -8 // offset from startpc to jitted body info
#define  OFFSET_COUNTING_BRANCH_FROM_JITENTRY             36
#endif

#if defined(TR_HOST_ARM64)
/**
 * Prior to refactoring, the type of these expressions was an intptr_t, and
 * some of the contexts in which these macros are currently used rely on them
 * being that type.  Until those contexts are changed to handle int32_t
 * types, explicitly cast these expressions to intptr_t for type correctness.
 */
#define  OFFSET_REVERT_INTP_FIXED_PORTION                ( (intptr_t)(-(12 + 2 * (int32_t)sizeof(intptr_t))) ) // See generateSwitchToInterpreterPrePrologue()
#define  OFFSET_SAMPLING_PREPROLOGUE_FROM_STARTPC        ( (intptr_t)(-(16 + (int32_t)sizeof(intptr_t))) )
#define  OFFSET_SAMPLING_BRANCH_FROM_STARTPC             ( (intptr_t)(-(12 + (int32_t)sizeof(intptr_t))) )
#define  OFFSET_SAMPLING_METHODINFO_FROM_STARTPC         ( (intptr_t)(-(8 + (int32_t)sizeof(intptr_t))) )
#define  OFFSET_SAMPLING_PRESERVED_FROM_STARTPC          (-8)
#define  OFFSET_COUNTING_BRANCH_FROM_JITENTRY            (9*ARM64_INSTRUCTION_LENGTH)
#endif

#if defined(TR_HOST_POWER) || defined(TR_HOST_ARM64)
inline uint32_t getJitEntryOffset(J9::PrivateLinkage::LinkageInfo *linkageInfo)
   {
   return linkageInfo->getReservedWord() & 0x0ffff;
   }
#endif

/* Functions used by AOT runtime to fixup recompilation info for AOT */
#if defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM) || defined(TR_HOST_ARM64)
uint32_t *getLinkageInfo(void * startPC);
uint32_t isRecompMethBody(void *li);
void fixPersistentMethodInfo(void *table, bool isJITClientAOTLoad = false);
void fixupMethodInfoAddressInCodeCache(void *startPC, void *bodyInfo);
#endif


typedef struct TR_InlinedSiteLinkedListEntry
   {
   TR_ExternalRelocationTargetKind reloType;
   uint8_t *location;
   uint8_t *destination;
   uint8_t *guard;
   TR_InlinedSiteLinkedListEntry *next;
   } TR_InlinedSiteLinkedListEntry;


typedef struct TR_InlinedSiteHashTableEntry
   {
   TR_InlinedSiteLinkedListEntry *first;
   TR_InlinedSiteLinkedListEntry *last;
   } TR_InlinedSiteHashTableEntry;

typedef enum
   {
   noPerfAssumptions = 0,
   tooManyFailedValidations = 1,
   tooManyFailedInlinedMethodRelos = 2,
   tooManyFailedInlinedAllocRelos = 3
   } TR_FailedPerfAssumptionCode;

typedef enum
   {
   inlinedMethodIsStatic            = 0x01,
   inlinedMethodIsSpecial           = 0x02,
   inlinedMethodIsVirtual           = 0x04,
   staticSpecialMethodFromCpIsSplit = 0x08,
   needsFullSizeRuntimeAssumption   = 0x10,
   methodTracingEnabled             = 0x20,

   // Relo Flags cannot be more than 12 bits
   // unless the _flags field of the
   // TR_RelocationRecordBinaryTemplate
   // is appropriately updated. This is because
   // the lowest 4 bits are used for the (poorly
   // named) Cross Platform Flags.
   highestBit                       = 0x800,
   } TR_RelocationFlags;

/* TR_AOTMethodHeader Versions:
*     1.0    Java6 GA
*     1.1    Java6 SR1
*     2.0    Java7
*     2.1    Java7
*     3.0    Java 8
*     4.0    Java 8+ with OSR
*/

#define TR_AOTMethodHeader_MajorVersion   4
#define TR_AOTMethodHeader_MinorVersion   0

typedef struct TR_AOTMethodHeader {
   uint16_t  minorVersion;
   uint16_t  majorVersion;
   uint32_t  offsetToRelocationDataItems;
   uint32_t  offsetToExceptionTable;
   uint32_t  offsetToPersistentInfo;
   uint32_t  flags;
   uintptr_t compileMethodCodeStartPC;
   uintptr_t compileMethodCodeSize;
   uintptr_t compileMethodDataStartPC;
   uintptr_t compileMethodDataSize;
   uintptr_t unused;
   struct
      {
      uintptr_t _frameSizeInBytes;
      uintptr_t _scratchBufferSizeInBytes;
      uintptr_t _stackFrameSizeInBytes;
      } _osrBufferInfo;
   } TR_AOTMethodHeader;


/* AOT Method flags */
#define TR_AOTMethodHeader_NeedsRecursiveMethodTrampolineReservation 0x00000001
#define TR_AOTMethodHeader_MethodEnterEventCanBeHooked               0x00000002
#define TR_AOTMethodHeader_MethodExitEventCanBeHooked                0x00000004
#define TR_AOTMethodHeader_UsesEnableStringCompressionFolding        0x00000008
#define TR_AOTMethodHeader_StringCompressionEnabled                  0x00000010
#define TR_AOTMethodHeader_UsesSymbolValidationManager               0x00000020
#define TR_AOTMethodHeader_TMDisabled                                0x00000040
#define TR_AOTMethodHeader_CompressedMethodInCache                   0x00000080
#define TR_AOTMethodHeader_IsNotCapableOfExceptionHook               0x00000100
#define TR_AOTMethodHeader_UsesOSR                                   0x00000200
#define TR_AOTMethodHeader_MethodTracingEnabled                      0x00000400
#define TR_AOTMethodHeader_UsesFSD                                   0x00000800
#define TR_AOTMethodHeader_TracksDependencies                        0x00001000


typedef struct TR_AOTInliningStats
   {
   int32_t numFailedValidations;
   int32_t numSucceededValidations;
   int32_t numMethodFromDiffClassLoader;
   int32_t numMethodInSameClass;
   int32_t numMethodNotInSameClass;
   int32_t numMethodResolvedAtCompile;
   int32_t numMethodNotResolvedAtCompile;
   int32_t numMethodROMMethodNotInSC;
   } TR_AOTInliningStats;


typedef struct TR_AOTStats
   {
   int32_t numCHEntriesAlreadyStoredInLocalList;
   int32_t numStaticEntriesAlreadyStoredInLocalList;
   int32_t numNewCHEntriesInLocalList;
   int32_t numNewStaticEntriesInLocalList;
   int32_t numNewCHEntriesInSharedClass;
   int32_t numEntriesFoundInLocalChain;
   int32_t numEntriesFoundAndValidatedInSharedClass;
   int32_t numClassChainNotInSharedClass;
   int32_t numCHInSharedCacheButFailValiation;
   int32_t numInstanceFieldInfoNotUsed;
   int32_t numStaticFieldInfoNotUsed;
   int32_t numDefiningClassNotFound;
   int32_t numInstanceFieldInfoUsed;
   int32_t numStaticFieldInfoUsed;
   int32_t numCannotGenerateHashForStore; // Shouldn't happen
   int32_t numRuntimeChainNotFound;
   int32_t numRuntimeStaticFieldUnresolvedCP;
   int32_t numRuntimeInstanceFieldUnresolvedCP;
   int32_t numRuntimeUnresolvedStaticFieldFromCP;
   int32_t numRuntimeUnresolvedInstanceFieldFromCP;
   int32_t numRuntimeResolvedStaticFieldButFailValidation;
   int32_t numRuntimeResolvedInstanceFieldButFailValidation;
   int32_t numRuntimeStaticFieldReloOK;
   int32_t numRuntimeInstanceFieldReloOK;

   int32_t numInlinedMethodOverridden;
   int32_t numInlinedMethodNotResolved;
   int32_t numInlinedMethodClassNotMatch;
   int32_t numInlinedMethodCPNotResolved;
   int32_t numInlinedMethodRelocated;
   int32_t numInlinedMethodValidationFailed;

   TR_AOTInliningStats staticMethods;
   TR_AOTInliningStats specialMethods;
   TR_AOTInliningStats virtualMethods;
   TR_AOTInliningStats interfaceMethods;
   TR_AOTInliningStats abstractMethods;

   TR_AOTInliningStats profiledInlinedMethods;
   TR_AOTInliningStats profiledClassGuards;
   TR_AOTInliningStats profiledMethodGuards;

   int32_t numDataAddressRelosSucceed;
   int32_t numDataAddressRelosFailed;

   int32_t numCheckcastNodesIlgenTime;
   int32_t numInstanceofNodesIlgenTime;
   int32_t numCheckcastNodesCodegenTime;
   int32_t numInstanceofNodesCodegenTime;

   int32_t numRuntimeClassAddressUnresolvedCP;
   int32_t numRuntimeClassAddressFromCP;
   int32_t numRuntimeClassAddressButFailValidation;
   int32_t numRuntimeClassAddressReloOK;

   int32_t numRuntimeClassAddressRelocationCount;
   int32_t numRuntimeClassAddressReloUnresolvedCP;
   int32_t numRuntimeClassAddressReloUnresolvedClass;

   int32_t numVMCheckCastEvaluator;
   int32_t numVMInstanceOfEvaluator;
   int32_t numVMIfInstanceOfEvaluator;
   int32_t numCheckCastVMHelperInstructions;
   int32_t numInstanceOfVMHelperInstructions;
   int32_t numIfInstanceOfVMHelperInstructions;

   int32_t numClassValidations;
   int32_t numClassValidationsFailed;
   int32_t numWellKnownClassesValidationsFailed;

   TR_FailedPerfAssumptionCode failedPerfAssumptionCode;

   uint32_t numRelocationsFailedByType[TR_NumExternalRelocationKinds];

   } TR_AOTStats;


#endif
