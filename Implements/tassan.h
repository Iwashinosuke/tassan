#ifndef TASSAN_H
#define TASSAN_H

#include <stddef.h>

typedef struct Tassan Tassan;

/*
SUTの入力システムに組み込まれるTassanのインターフェースを起動します
handle: Tassanのハンドルへのポインタ(NULL初期化したもの)
buf_size: AFLから渡されるバッファのサイズ
buf: AFLから渡されるバッファへのポインタ
*/
void TS_new_test(Tassan** handle, size_t buf_size, char* buf);

/*
SUT内のロジックループでループごとに実行
delta_time: 前回の呼び出しからの経過時間（ミリ秒単位）
*/
void TS_update(Tassan* handle, unsigned short int delta_time);

/*
ncursesのgetchなどのためのラッパー関数
TS_pollkeyinput_onebyoneとの併用は非推奨
*/
char TS_getch(Tassan* handle);

/*
SDL_PollEventなどで使用できる関数
Tassanが有する入力キーのリストから1つずつキー入力を取得します
TS_getchとの併用は非推奨

Arguments:
 handle: Tassanのハンドル
 processing: 取得したキー入力を格納するポインタ
 is_pressed: 取得したキーが押下されているかどうかを格納するポインタ(1: 押下されている, 0: 押下されていない)
Returns:
 1: まだ読み込まれてないキーがある
 0: すべてのキーを読み込んだ
 (この関数では、リストに記録されているキーを順番に出力します。0が返されたあとは、再度リストの最初から読み込みます)
*/
int  TS_pollkeyinput_onebyone(Tassan* handle, char* processing, int* is_pressed);

/*
テストケース内の入力の実行がすべて完了したかどうかを取得します
Returns:
 1: テストケースの実行中
 0: テストケースの実行が完了した
*/
int  TS_is_running(Tassan* handle);

/*
Tassanのハンドルを解放します
*/
void TS_free(Tassan* handle);

#endif // TASSAN_H
