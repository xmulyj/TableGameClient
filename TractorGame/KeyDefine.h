/*
 * KeyDefine.h
 *
 *  Created on: Oct 1, 2013
 *      Author: tim
 */

#ifndef KEYDEFINE_H_
#define KEYDEFINE_H_

//Protocol Type
//////////////////////////////
#define PRO_ReportRoomInfo      0
#define PRO_GetRoomList         1
#define PRO_GetRoomListRsp      2
#define PRO_GetRoomAddr         3
#define PRO_GetRoomAddrRsp      4

#define PRO_IntoRoom            5
#define PRO_LeaveRoom           6
#define PRO_RoomInfoBroadCast   7

#define PRO_AddGame             8
#define PRO_QuitGame            9
#define PRO_StartGame           10
#define PRO_DealPoker           11
#define PRO_AddGameRsp          12
#define PRO_AddGameBroadCast    13
#define PRO_QuitGameBroadCast   14
#define PRO_StartGameBroadCast   15

//KeyName Define
//////////////////////////////
#define KEY_Protocol        0    //value:见Protocol Type
#define KEY_RoomID          1
#define KEY_RoomIP          2
#define KEY_RoomPort        3
#define KEY_ClientNum       4
#define KEY_TableNum        5
#define KEY_TableID         6
#define KEY_Array           7
#define KEY_ClientID        8
#define KEY_ClientName      9
#define KEY_RoomNum         10
#define KEY_Message         11
#define KEY_Status          12
#define KEY_PlayerNum       13
#define KEY_AudienceNum     14
#define KEY_NeedNum         15
#define KEY_CardFlag        16
#define KEY_CurLevel        17
#define KEY_CurColor        18
#define KEY_CurDealer       19
#define KEY_PokerNum        20
#define KEY_ScoreNum        21
#define KEY_ScoreArray      22
#define KEY_PosIndex        23

#endif /* KEYDEFINE_H_ */
