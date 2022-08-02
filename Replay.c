#define _CRT_SECURE_NO_WARNINGS
#include "Replay.h"
#include "Public_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 리플레이 데이터는 게임에 사용되었던 시드값과 뱀의 방향이 전환된 위치를 저장합니다
// 리플레이 재생 시 현재 위치 - 다음 위치를 비교해 방향을 지정하여 이동합니다
// 다음 위치 도달 시 방향 지정을 다시 수행하여 파일의 끝까지 반복합니다

replaydata* createreplay(int x, int y) {
	replaydata* temp = (replaydata*)malloc(sizeof(replaydata));
	temp->location.x = x;
	temp->location.y = y;

	return temp;
}

void append(player* p, replaydata* newnode) {
	if (p->data == NULL) {
		newnode->link = newnode;
		p->data = newnode;
		return;
	}

	newnode->link = p->data->link;
	p->data->link = newnode;
	p->data = newnode;
}
// 파일 이름 자동 생성
char* getname() {
	static char fname[100];

	int count = 1;
	char charcount[3];

	while (1) { //replay(n).dat 생성
		char buffer[100] = "replay(";
		sprintf(charcount, "%d", count);
		strcat(buffer, charcount);
		strcat(buffer, ").dat");

		if (fopen(buffer, "r") == NULL) {	//파일이 존재하지 않다면 해당 이름으로 저장
			strcpy(fname, buffer);
			break;
		}

		count++;
	}

	return fname;
}

FILE* made_replay() {
	return fopen(getname(), "wb");
}
// 데이터를 파일로 저장
void savefile(player* p, int seed) {
	FILE* fp = made_replay();

	fwrite(&seed, sizeof(int), 1, fp); //시드값 저장
	replaydata* movement = p->data->link; //리플레이 시작부분

	do {
		replaydata* nextmovement = movement->link;

		if (movement->location.x != nextmovement->location.x && movement->location.y != nextmovement->location.y) {
			if (movement->link != p->data->link) {
				//오류 수정코드
				replaydata* next2movement = nextmovement->link;

				if (nextmovement->location.y == next2movement->location.y) {
					replaydata* correctionnode = (replaydata*)malloc(sizeof(replaydata));

					correctionnode->location.x = nextmovement->location.x;
					correctionnode->location.y = movement->location.y;

					correctionnode->link = nextmovement;
					movement->link = correctionnode;
				}

				else if (nextmovement->location.x == next2movement->location.x) {
					replaydata* correctionnode = (replaydata*)malloc(sizeof(replaydata));

					correctionnode->location.x = movement->location.x;
					correctionnode->location.y = nextmovement->location.y;

					correctionnode->link = nextmovement;
					movement->link = correctionnode;
				}
			}
		}

		fwrite(movement, sizeof(replaydata), 1, fp);

		movement = movement->link;
	} while (movement != p->data->link);

	printf("저장이 완료되었습니다.\n");
	fclose(fp);
}
