// ---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>

typedef struct {
    int id;
	wchar_t* caption;
}

CardType;

typedef struct {
	int id;
	int no;
	CardType cardType;
	int score;
	wchar_t* caption;
}

Card;

typedef struct {
	Card* _card;
	int* _cards;
	int _size;
	int length;
}

CardList;

typedef struct {
	int pid;
	CardList* hand;
	CardList* paired;
	int score;
}

Player;

// ---------------------------------------------------------------------------
class TForm1 : public TForm {
__published: // IDE-managed Components
	TButton *Button1;
	TPanel *P1Hand;
	TPanel *P2Hand;
	TPanel *P3Hand;
	TPanel *P4Hand;
	TPanel *Panel1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TSpeedButton *SpeedButton6;
	TSpeedButton *SpeedButton7;
	TSpeedButton *SpeedButton8;
	TSpeedButton *SpeedButton9;
	TSpeedButton *SpeedButton10;
	TSpeedButton *SpeedButton11;
	TSpeedButton *SpeedButton12;
	TSpeedButton *SpeedButton13;
	TSpeedButton *SpeedButton14;
	TSpeedButton *SpeedButton15;
	TSpeedButton *SpeedButton16;
	TSpeedButton *SpeedButton17;
	TSpeedButton *SpeedButton18;
	TSpeedButton *SpeedButton19;
	TSpeedButton *SpeedButton20;
	TSpeedButton *SpeedButton21;
	TSpeedButton *SpeedButton22;
	TSpeedButton *SpeedButton23;
	TSpeedButton *SpeedButton24;
	TSpeedButton *SpeedButton25;
	TSpeedButton *SpeedButton26;
	TSpeedButton *SpeedButton27;
	TSpeedButton *SpeedButton28;
	TSpeedButton *SpeedButton29;
	TSpeedButton *SpeedButton30;
	TSpeedButton *SpeedButton31;
	TSpeedButton *SpeedButton32;
	TSpeedButton *SpeedButton33;
	TSpeedButton *SpeedButton34;
	TSpeedButton *SpeedButton35;
	TSpeedButton *SpeedButton36;
	TSpeedButton *SpeedButton37;
	TLabel *Label6;
	TLabel *Label5;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TLabel *Label17;
	TMemo *Memo1;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);

private: // User declarations
	const int cardNum = 13;
	const int typeNum = 4;
	const int playerNum = 4;
	const int handCardNum = 6;
	const int oceanCardNum = 4;
	const int total_card = cardNum * typeNum;

	bool pairableCheck(CardList*, CardList*);
	CardType* cardTypes;
	Card* cards;
	Player** players;
	CardList* bank;
	CardList* ocean;
	CardList* drawone;
	TSpeedButton* btnsPlayer[4][6];
	TSpeedButton* btnsOc[12];
	TPanel* panelsPlayer[4];
    TLabel* scoreLabelPlayer[4];
	int current;
	int selHand;
	int selOcean;
	int selDrawone;

	void __fastcall BtnPlayerClick(TObject *Sender);
	void __fastcall BtnOceanClick(TObject *Sender);
	void gameContinue();
	void nextPlayer();
	void initCard(Card**);
	bool delCard(Card**);
	void initCardTypes(CardType**);
	bool delCardypes(CardType**);
    void gameEndCheck();
	CardList* initCardList(Card*, int);
	bool delCardList(CardList**);
	Player* initPlayer(Card*);
	bool delPlayer(Player**);
	void fillCard(CardList *);
	void combo();
	bool addCard(CardList *, Card *);
	void randomizeCard(CardList *);
    void viewRefresh();
	Card* drawCard(CardList *);
	Card* drawCardByIndex(CardList *, int);
	int getCardLength(CardList*) ;
	Card* getCardByIndex(CardList*, int);
	bool pairCard(Card*, Card*);
	int gameEnd();
	int gameStart();
	int getCard(int *);

public: // User declarations
	__fastcall TForm1(TComponent* Owner);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
// ---------------------------------------------------------------------------
#endif
