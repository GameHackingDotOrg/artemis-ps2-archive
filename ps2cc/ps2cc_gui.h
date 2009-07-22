#include <windows.h>

#define PROGRAM_NAME "PS2 Code Creator - Rev 13"

#define PS2CC_DLG 1000
#define PS2CC_TABCTRL 1001
#define NTPB_STATUS_BAR 1002
#define DUMPSTATE_PRB 1003
//custom commands
#define LSV_CS_BEGINEDIT 400
#define LSV_CS_ENDEDIT 401
//Menus
#define PS2CC_MNU 500
#define MNU_LOAD_SEARCH 501
#define MNU_EXIT 502
#define MNU_DUMP_DIR 503
#define MNU_SHOW_CONFIG 504
#define MNU_RES_SHOW_HEX 505
#define MNU_RES_SHOW_DECU 506
#define MNU_RES_SHOW_DECS 507
#define MNU_RES_SHOW_FLOAT 508
#define MNU_RES_EXPORT_HEX 509
#define MNU_RES_EXPORT_DECU 510
#define MNU_RES_EXPORT_DECS 511
#define MNU_RES_EXPORT_FLOAT 512
#define MNU_CS_INPUT_HEX 513
#define MNU_CS_INPUT_DEC 514
#define MNU_CS_INPUT_FLOAT 515
#define MNU_CS_UNDO 516
//search tab
#define SEARCH_DLG 1100
#define SEARCH_GRP1 1101
#define QUICK_SEARCH_LBL 1102
#define QS_INIT_CMD 1103
#define QS_EQUAL_CMD 1104
#define QS_NEQUAL_CMD 1105
#define QS_GREATER_CMD 1106
#define QS_LESS_CMD 1107
#define QS_KNOWN_CMD 1108
#define SEARCH_TYPE_CMB 1109
#define SEARCH_VALUE1_TXT 1110
#define SEARCH_VALUE2_TXT 1111
#define SEARCH_DASH_LBL 1112
#define SEARCH_VALUES_LBL 1113
#define SEARCH_SIZE_GRP 1114
#define SEARCH_SIZE_CMB 1115
#define COMPARE_TO_GRP 1116
#define SEARCH_AREA_GRP 1118
#define COMPARE_TO_CMB 1117
#define SEARCH_AREA_CMB 1119
#define SEARCH_AREA_LOW_TXT 1120
#define SEARCH_AREA_HIGH_TXT 1121
#define SEARCH_AREA_LBL 1122
#define DO_SEARCH_CMD 1123
#define TAKE_DUMP_CMD 1124
#define EX_SEARCH_GRP 1125
#define SEARCH_SIGNED_CHK 1126
#define EX_SEARCH_LSV 1127
#define EX_FILTER_CMD 1128
#define SEARCH_HISTORY_GRP 1129
#define SEARCH_HISTORY_TXT 1130
#define EX_VALUE_TXT 1131
//IP config dialog
#define SETTINGS_DLG 1200
#define SEARCH_ACCESS_GRP 1201
#define IP_ADDR_TXT 1202
#define SET_OPTIONS_CMD 1203
#define TEST_IP_CMD 1204
#define IP_CONFIG_GRP 1205
#define DUMP_DIR_GRP 1206
#define DUMP_DIR_TXT 1207
#define SET_DUMP_DIR_CMD 1208
#define SEARCH_ACCESS_ARRAY_OPT 1209
#define SEARCH_ACCESS_FILE_OPT 1210
#define RESULTS_PAGE_SIZE_GRP 1211
#define RESULTS_PAGE_SIZE_TXT 1212
#define RESULTS_PAGE_SIZE_LBL 1213
#define RESULTS_PAGE_MAX_GRP 1214
#define RESULTS_PAGE_MAX_TXT 1215
#define RESULTS_PAGE_MAX_LBL 1216

//results tab
#define RESULTS_DLG 1300
#define RESULTS_LIST_GRP 1301
#define ACTIVE_CODES_GRP 1307
#define RESULTS_LSV 1302
#define RESULTS_PAGE_LBL 1303
#define RESULTS_PAGE_CMB 1304
#define ACTIVE_CODES_LSV 1308
#define ACTIVE_RES_ADDR_TXT 1309
#define ACTIVE_RES_VALUE_TXT 1310
#define ACTIVE_RES_SIZE_CMB 1311
#define ACTIVE_RES_ADDR_LBL 1312
#define ACTIVE_RES_VALUE_LBL 1313
#define ACTIVE_RES_SIZE_LBL 1314
#define RES_WRITE_ONCE_CMD 1315
#define RES_ACTIVATE_CMD 1316
#define RES_DEL_ACTIVE_CMD 1317
#define RES_ALL_ON_CMD 1318
#define RES_ALL_OFF_CMD 1319
#define RES_CLEAR_ALL_CMD 1320
#define RESULTS_PGDOWN_CMD 1321
