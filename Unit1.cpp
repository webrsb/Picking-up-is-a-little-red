// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
int TForm1::gameStart() {
	int i, j;

	gameEnd();
	// 初始化輪到的玩家
	current = -1;
	selHand = -1;
	selOcean = -1;
	selDrawone = -1;
	// 初始化花色
	initCardTypes(&cardTypes);
	// 初始化牌組
	initCard(&cards);
	// 初始化玩家
	if (players == NULL) {
		players = (Player * *) malloc(playerNum*sizeof(Player*));
	}

	for (i = 0; i < playerNum; i++) {
		Player *p = initPlayer(cards);
		players[i] = p;
		players[i]->pid = i;
		scoreLabelPlayer[i]->Caption = IntToStr(players[i]->score);
	}
	// 初始化牌庫
	bank = initCardList(cards, total_card);
	ocean = initCardList(cards, total_card);
	drawone = initCardList(cards, 1);
	// 填充牌
	fillCard(bank);
	// 洗牌
	randomizeCard(bank);

	// 發海底牌
	for (i = 0; i < oceanCardNum; i++) {
		addCard(ocean, drawCard(bank));
	}

	// 發手牌
	for (i = 0; i < playerNum; i++) {
		for (j = 0; j < handCardNum; j++) {
			Card *tc = drawCard(bank);
			addCard(players[i]->hand, tc);
			// 加入按鈕事件
			btnsPlayer[i][j]->OnClick = BtnPlayerClick;
			// 加入識別碼
			btnsPlayer[i][j]->Tag = j;
		}
	}

	for (i = 0; i < 12; i++) {
		btnsOc[i]->OnClick = BtnOceanClick;
		// 加入識別碼
		btnsOc[i]->Tag = i;
	}

	nextPlayer();

	return 0;
}

void TForm1::nextPlayer() {
	int i;
	wchar_t *str;

	if (current + 1 >= 4) {
		current = 0;
	}
	else {
		current++;
	}

	for (i = 0; i < playerNum; i++) {
		if (current == i) {
			panelsPlayer[i]->Enabled = true;
			panelsPlayer[i]->Color = clSkyBlue;

			continue;
		}

		panelsPlayer[i]->Enabled = false;
		panelsPlayer[i]->Color = clBtnFace;
	}

	Label4->Caption = L"玩家" + IntToStr(current + 1);

	viewRefresh();
	gameEndCheck();
}

void TForm1::gameEndCheck() {
	int i, j, hand_num = 0, f_score, s_score;
	Player **p, *pt;

	for (i = 0; i < playerNum; i++) {
		hand_num += players[i]->hand->length;
	}

	if (hand_num > 0) {
		return;
	}

	p = (Player * *) malloc(playerNum*sizeof(Player*));
	memcpy(p, players, playerNum*sizeof(Player*));

	// 泡沫排序
	for (i = 0; i < playerNum - 1; i++) {
		for (j = 0; j < playerNum - i - 1; j++) {
			f_score = p[j]->score;
			s_score = p[j + 1]->score;

			if (s_score > f_score) {
				pt = p[j];
				p[j] = p[j + 1];
				p[j + 1] = pt;
			}
		}
	}

	ShowMessage(L"遊戲結束");
	Memo1->Lines->Add(L"分數排名：");

	for (i = 0; i < playerNum; i++) {
		Memo1->Lines->Add(L"玩家" + IntToStr(p[i]->pid + 1));
		Memo1->Lines->Add(IntToStr(p[i]->score));
	}

	free(p);
	gameEnd();
}

void TForm1::viewRefresh() {
	int i, j;
	Card *card;

	for (i = 0; i < playerNum; i++) {
		for (j = 0; j < handCardNum; j++) {
			card = getCardByIndex(players[i]->hand, j);

			if (card == NULL) {
				btnsPlayer[i][j]->Caption = L"";
				btnsPlayer[i][j]->Enabled = false;
			}
			else {
				btnsPlayer[i][j]->Caption = card->caption;
				btnsPlayer[i][j]->Enabled = true;
			}
		}
	}

	for (i = 0; i < 12; i++) {
		card = getCardByIndex(ocean, i);

		if (card == NULL) {
			btnsOc[i]->Caption = L"";
		}
		else {
			btnsOc[i]->Caption = card->caption;
		}
	}

	card = getCardByIndex(drawone, 0);

	if (card == NULL) {
		SpeedButton1->Enabled = false;
		SpeedButton1->Caption = L"";
	}
	else {
		SpeedButton1->Enabled = true;
		SpeedButton1->Caption = card->caption;
	}

	Label2->Caption = bank->length;
}

void TForm1::gameContinue() {
	Card *temp, *src, *dst;
	CardList *src_cl, *dst_cl;
	int *src_sel, *dst_sel;
	TSpeedButton *src_btn, *dst_btn;

	// 手牌與海底牌的配對
	if (selHand != -1 && selOcean != -1) {
		src = getCardByIndex(players[current]->hand, selHand);
		src_cl = players[current]->hand;
		src_sel = &selHand;
		src_btn = btnsPlayer[current][*src_sel];
		// 翻開的牌與海底牌配對
	}
	else if (selDrawone != -1 && selOcean != -1) {
		src = getCardByIndex(drawone, selDrawone);
		src_cl = drawone;
		src_sel = &selDrawone;
		src_btn = SpeedButton1;
	}
	else {
		return;
	}

	dst = getCardByIndex(ocean, selOcean);
	dst_cl = ocean;
	dst_sel = &selOcean;
	dst_btn = btnsOc[*dst_sel];

	// 配對情況
	if (src != NULL && dst != NULL) {

		if (pairCard(src, dst)) {
			drawCardByIndex(src_cl, *src_sel);
			drawCardByIndex(dst_cl, *dst_sel);
			addCard(players[current]->paired, src);
			addCard(players[current]->paired, dst);
			players[current]->score += src->score + dst->score;

			scoreLabelPlayer[current]->Caption =
				IntToStr(players[current]->score);

			// 清除按下顏色
			src_btn->Font->Color = clBlack;
			dst_btn->Font->Color = clBlack;
			*dst_sel = -1;
			*src_sel = -1;

			combo();
		}
		else {
			*dst_sel = -1;
		}

	}
	// 將手牌丟到海底
	else if (src != NULL && dst == NULL) {
		if (pairableCheck(src_cl, dst_cl)) {
			ShowMessage(L"您還有可配對的牌！");
			return;
		}
		temp = drawCardByIndex(src_cl, *src_sel);

		if (temp != NULL) {
			addCard(dst_cl, temp);
		}

		// 清除按下顏色
		dst_btn->Font->Color = clBlack;
		src_btn->Font->Color = clBlack;

		*dst_sel = -1;
		selHand = -1;

		// 無牌可出，翻一張牌
		if (selDrawone == -1) {
			combo();
		}                      
		// 已翻過牌，結束回合
		else {
			selDrawone = -1;
			nextPlayer();
		}
	}
}

bool TForm1::pairableCheck(CardList* src, CardList* dst) {
	int i, j;
	bool r;

	for (i = 0; i < src->length; i++) {
		for (j = 0; j < dst->length; j++) {
			r = pairCard(getCardByIndex(src, i), getCardByIndex(dst, j));

			if (r) {
				return true;
			}
		}
	}

	return false;
}

void TForm1::combo() {
	Card *card;
	bool r;

	card = drawCard(bank);

	// 牌庫無牌
	if (card == NULL) {
		nextPlayer();

		return;
	}

	panelsPlayer[current]->Enabled = false;
	selDrawone = 0;
	SpeedButton1->Font->Color = clRed;
	addCard(drawone, card);
	viewRefresh();
}

void __fastcall TForm1::BtnPlayerClick(TObject *Sender) {
	static int last = -1;
	int tag = ((TSpeedButton*)Sender)->Tag;

	if (last != -1) {
		btnsPlayer[current][last]->Font->Color = clBlack;
	}

	if (tag == selHand) {
		((TSpeedButton*)Sender)->Font->Color = clBlack;
		selHand = -1;
	}
	else {
		selHand = tag;
		((TSpeedButton*)Sender)->Font->Color = clRed;
	}

	last = selHand;
}

void __fastcall TForm1::BtnOceanClick(TObject *Sender) {
	int tag = ((TSpeedButton*)Sender)->Tag;

	selOcean = tag;
	gameContinue();
}

CardList* TForm1::initCardList(Card* card, int size) {
	int i, j, no, mem_size, *arr;
	CardList* cardList;

	arr = (int*) malloc(sizeof(int) * size);
	cardList = (CardList*) malloc(sizeof(CardList));
	cardList->_card = card;
	cardList->_cards = arr;
	cardList->_size = size;
	cardList->length = 0;

	return cardList;
}

bool TForm1::delCardList(CardList** ptr) {
	if (*ptr != NULL) {
		free((*ptr)->_cards);
		free(*ptr);
		*ptr = NULL;
	}

	return true;
}

void TForm1::fillCard(CardList* cardList) {
	int i;

	for (i = 0; i < cardList->_size; i++) {
		cardList->_cards[i] = i;
	}

	cardList->length = cardList->_size;
}

bool TForm1::addCard(CardList *cardList, Card *card) {
	if (cardList->length + 1 > cardList->_size) {
		return false;
	}

	cardList->_cards[cardList->length] = card->id;
	cardList->length++;

	return true;
}

void TForm1::randomizeCard(CardList* cardList) {
	int r, i, j, *remain_card, *arr, size;

	size = cardList->length;
	arr = cardList->_cards;

	remain_card = (int*) malloc(sizeof(int) * size);
	memcpy(remain_card, arr, size * sizeof(int));

	for (i = 0; i < size; i++) {
		remain_card[i] = i;
	}

	for (i = size - 1, j = 0; i >= 0; i--, j++) {
		r = rand() % (i + 1);

		arr[j] = remain_card[r];

		if (r < i) {
			memcpy(&remain_card[r], &remain_card[r + 1], (i - r) * sizeof(int));
		}
	}

	free(remain_card);
}

Card* TForm1::drawCard(CardList* cardList) {
	Card *card = cardList->_card;

	if (cardList->length - 1 >= 0) {
		cardList->length--;

		return &(card[cardList->_cards[cardList->length]]);
	}

	return NULL;
}

Card* TForm1::drawCardByIndex(CardList* cardList, int index) {
	Card *card = cardList->_card;
	Card *target;

	if (index >= 0 && index < cardList->length) {
		cardList->length--;
		target = &(card[cardList->_cards[index]]);

		if (index < cardList->length) {
			memcpy(&cardList->_cards[index], &cardList->_cards[index + 1],
				sizeof(int) * (cardList->length - index));
		}

		return target;
	}

	return NULL;
}

int TForm1::getCardLength(CardList* cardList) {
	return cardList->length;
}

Card* TForm1::getCardByIndex(CardList* cardList, int index) {
	if (index >= cardList->length) {
		return NULL;
	}

	return &cardList->_card[cardList->_cards[index]];
}

bool TForm1::pairCard(Card* dst, Card* src) {
	int dst_no = dst->no;
	int src_no = src->no;

	if (dst_no <= 9) {
		if (dst_no + src_no == 10) {
			return true;
		}
	}
	else if (dst_no == src_no) {
		return true;
	}

	return false;
}

int TForm1::getCard(int *ptr) {
	static int curr = 0;
	int v, r;

	if (ptr == NULL) {
		curr = 0;
		return -1;
	}

	v = curr;
	curr++;

	if (v >= total_card) {
		return -1;
	}

	r = ptr[v];

	return ptr[v];
}

void TForm1::initCardTypes(CardType** ptr) {
	int i, mem_size;

	const wchar_t* typeList[] = {L"♣ ", L"♦ ", L"♥ ", L"♠ "};

	if (*ptr == NULL) {
		*ptr = (CardType*) malloc(typeNum*sizeof(CardType));
	}

	for (i = 0; i < typeNum; i++) {
		mem_size = (wcslen(typeList[i]) + 1)*sizeof(wchar_t);
		(*ptr + i)->caption = (wchar_t*)malloc(mem_size);
		(*ptr + i)->id = i;
		wcscpy((*ptr + i)->caption, typeList[i]);
	}
}

bool TForm1::delCardypes(CardType** ptr) {
	int i;

	if (*ptr != NULL) {
		for (i = 0; i < typeNum; i++) {
			free((*ptr + i)->caption);
		}

		free(*ptr);
		*ptr = NULL;

	}

	return true;
}

void TForm1::initCard(Card** ptr) {
	const int buffer_size = 10;
	wchar_t numChar[buffer_size];
	int i, j, no, mem_size, tid;

	if (*ptr == NULL) {
		*ptr = (Card*) malloc(total_card*sizeof(Card));
	}

	for (i = 0; i < typeNum; i++) {
		for (j = 0; j < cardNum; j++) {
			no = i * cardNum + j;
			(*ptr + no)->id = no;
			(*ptr + no)->no = j + 1;
			(*ptr + no)->cardType = cardTypes[i];
			tid = (*ptr + no)->cardType.id;
			swprintf(numChar, buffer_size, L"%s%d", cardTypes[i].caption,
				j + 1);
			mem_size = (wcslen(numChar) + 1)*sizeof(wchar_t);
			(*ptr + no)->caption = (wchar_t*)malloc(mem_size);
			wcscpy((*ptr + no)->caption, numChar);
			/**
			 分數計算
			 黑桃A +30
			 A +20
			 紅色 9, 10, J, Q, K +10
			 紅色 2-8 +本身數字
			 * */

			if (tid == 3 && (*ptr + no)->no == 1) {
				(*ptr + no)->score = 30;
			}
			else if (tid == 1 || tid == 2) {
				if ((*ptr + no)->no == 1) {
					(*ptr + no)->score = 20;
				}
				else if ((*ptr + no)->no >= 9) {
					(*ptr + no)->score = 10;
				}
				else {
					(*ptr + no)->score = (*ptr + no)->no;
				}
			}
			else {
				(*ptr + no)->score = 0;
			}
		}
	}
}

bool TForm1::delCard(Card** ptr) {
	int i;

	if (*ptr != NULL) {
		for (i = 0; i < total_card; i++) {
			free((*ptr + i)->caption);
		}

		free(*ptr);
		*ptr = NULL;

	}

	return true;
}

Player* TForm1::initPlayer(Card *card) {
	Player *player = (Player*) malloc(sizeof(Player));

	player->hand = initCardList(card, handCardNum);
	player->paired = initCardList(card, total_card);
	player->score = 0;

	return player;
}

bool TForm1::delPlayer(Player** ptr) {
	if (*ptr != NULL) {
		delCardList(&(*ptr)->hand);
		delCardList(&(*ptr)->paired);
		free(*ptr);
		*ptr = NULL;
	}

	return true;
}

int TForm1::gameEnd() {
	int i;

	delCard(&cards);
	delCardypes(&cardTypes);

	delCardList(&drawone);
	delCardList(&bank);
	delCardList(&ocean);

	if (players != NULL) {
		for (i = 0; i < playerNum; i++) {
			delPlayer(&players[i]);
		}

		free(players);
		players = NULL;
	}

	return 0;
}

__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
	TForm1::gameStart();
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender) {

	srand(time(NULL));
	panelsPlayer[0] = P1Hand;
	panelsPlayer[1] = P2Hand;
	panelsPlayer[2] = P3Hand;
	panelsPlayer[3] = P4Hand;

	btnsPlayer[0][0] = SpeedButton2;
	btnsPlayer[0][1] = SpeedButton3;
	btnsPlayer[0][2] = SpeedButton4;
	btnsPlayer[0][3] = SpeedButton5;
	btnsPlayer[0][4] = SpeedButton6;
	btnsPlayer[0][5] = SpeedButton7;

	btnsPlayer[1][0] = SpeedButton8;
	btnsPlayer[1][1] = SpeedButton9;
	btnsPlayer[1][2] = SpeedButton10;
	btnsPlayer[1][3] = SpeedButton11;
	btnsPlayer[1][4] = SpeedButton12;
	btnsPlayer[1][5] = SpeedButton13;

	btnsPlayer[2][0] = SpeedButton14;
	btnsPlayer[2][1] = SpeedButton15;
	btnsPlayer[2][2] = SpeedButton16;
	btnsPlayer[2][3] = SpeedButton17;
	btnsPlayer[2][4] = SpeedButton18;
	btnsPlayer[2][5] = SpeedButton19;

	btnsPlayer[3][0] = SpeedButton20;
	btnsPlayer[3][1] = SpeedButton21;
	btnsPlayer[3][2] = SpeedButton22;
	btnsPlayer[3][3] = SpeedButton23;
	btnsPlayer[3][4] = SpeedButton24;
	btnsPlayer[3][5] = SpeedButton25;

	btnsOc[0] = SpeedButton26;
	btnsOc[1] = SpeedButton27;
	btnsOc[2] = SpeedButton28;
	btnsOc[3] = SpeedButton29;
	btnsOc[4] = SpeedButton30;
	btnsOc[5] = SpeedButton31;
	btnsOc[6] = SpeedButton32;
	btnsOc[7] = SpeedButton33;
	btnsOc[8] = SpeedButton34;
	btnsOc[9] = SpeedButton35;
	btnsOc[10] = SpeedButton36;
	btnsOc[11] = SpeedButton37;

	scoreLabelPlayer[0] = Label13;
	scoreLabelPlayer[1] = Label14;
	scoreLabelPlayer[2] = Label15;
	scoreLabelPlayer[3] = Label16;
}
// ---------------------------------------------------------------------------
