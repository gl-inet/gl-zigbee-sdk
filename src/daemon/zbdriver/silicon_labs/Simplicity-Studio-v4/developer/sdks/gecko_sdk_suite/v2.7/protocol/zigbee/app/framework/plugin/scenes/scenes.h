/***************************************************************************//**
 * @file
 * @brief Definitions for the Scenes plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

EmberAfStatus emberAfScenesSetSceneCountAttribute(uint8_t endpoint,
                                                  uint8_t newCount);
EmberAfStatus emberAfScenesMakeValid(uint8_t endpoint,
                                     uint8_t sceneId,
                                     uint16_t groupId);

// DEPRECATED.
#define emberAfScenesMakeInvalid emberAfScenesClusterMakeInvalidCallback

void emAfPluginScenesServerPrintInfo(void);

extern uint8_t emberAfPluginScenesServerEntriesInUse;
#if defined(EMBER_AF_PLUGIN_SCENES_USE_TOKENS) && !defined(EZSP_HOST)
// In this case, we use token storage
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  halCommonGetIndexedToken(&entry, TOKEN_SCENES_TABLE, i)
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  halCommonSetIndexedToken(TOKEN_SCENES_TABLE, i, &entry)
  #define emberAfPluginScenesServerNumSceneEntriesInUse()                               \
  (halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
   emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x),               \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    ++emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse()                            \
  ((halCommonGetToken(&emberAfPluginScenesServerEntriesInUse, TOKEN_SCENES_NUM_ENTRIES), \
    --emberAfPluginScenesServerEntriesInUse),                                            \
   halCommonSetToken(TOKEN_SCENES_NUM_ENTRIES, &emberAfPluginScenesServerEntriesInUse))
#else
// Use normal RAM storage
extern EmberAfSceneTableEntry emberAfPluginScenesServerSceneTable[];
  #define emberAfPluginScenesServerRetrieveSceneEntry(entry, i) \
  (entry = emberAfPluginScenesServerSceneTable[i])
  #define emberAfPluginScenesServerSaveSceneEntry(entry, i) \
  (emberAfPluginScenesServerSceneTable[i] = entry)
  #define emberAfPluginScenesServerNumSceneEntriesInUse() \
  (emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerSetNumSceneEntriesInUse(x) \
  (emberAfPluginScenesServerEntriesInUse = (x))
  #define emberAfPluginScenesServerIncrNumSceneEntriesInUse() \
  (++emberAfPluginScenesServerEntriesInUse)
  #define emberAfPluginScenesServerDecrNumSceneEntriesInUse() \
  (--emberAfPluginScenesServerEntriesInUse)
#endif // Use tokens

bool emberAfPluginScenesServerParseAddScene(const EmberAfClusterCommand *cmd,
                                            uint16_t groupId,
                                            uint8_t sceneId,
                                            uint16_t transitionTime,
                                            uint8_t *sceneName,
                                            uint8_t *extensionFieldSets);
bool emberAfPluginScenesServerParseViewScene(const EmberAfClusterCommand *cmd,
                                             uint16_t groupId,
                                             uint8_t sceneId);
