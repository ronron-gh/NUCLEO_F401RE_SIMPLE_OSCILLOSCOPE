#ifndef __OSCILLO_H__
#define __OSCILLO_H__

#define ADC_BUF_SIZE	(300)		// サンプリングのバッファサイズ（サンプル数）
#define SHOW_SIZE		(280)		// 画面表示するサンプル数

#define SAMPLING_RATE	(200)		// kHz
#define DELTA_TIME		(1.0 / SAMPLING_RATE)		// ms
#define ADC_RESOLUTION	(4096)		// 12bit ADC
#define ADC_REF_V		(3.3)
#define VOLT_DIVIDE_RATIO	(110/10)	// 100kΩと10kΩで分圧

#define LABEL_STRLEN_MAX	(10)
#define LABEL_LIST_MAX	(15)

struct Label{
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	u16 color;
	u8  str[LABEL_STRLEN_MAX];
	u16 str_size;
	u16 str_color;
	u8	enable;

	void (*onClick)(void);
};


struct View{
	void (*drawView)(void);
	struct Label *labelList[LABEL_LIST_MAX];
};

extern struct View *current_view;

void oscillo_init(void);
void oscillo_draw(struct View *view);


#endif //__OSCILLO_H__
