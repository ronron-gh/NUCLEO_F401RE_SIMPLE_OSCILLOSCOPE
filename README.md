# NUCLEO_F401RE_SIMPLE_OSCILLOSCOPE

以下のブログで紹介している簡易オシロスコープのファームウェアを公開します。

[STM32マイコンで簡易オシロスコープを自作](https://mzmlab.hatenablog.com/entry/stm32-oscilloscope)


## 1. 簡易オシロスコープの仕様

簡易なオシロスコープということで、仕様は次の通りです。

* チャンネル数 : 1ch　(立ち上がりエッジでトリガ可)
* 周波数帯域 : 40kHz
* A/D分解能 : 12bit
* サンプリング周波数 : 200kS/s (※1)
* レコード長 : 300サンプル
* 最高入力電圧 : 3.3V (※2)

STM32マイコンの評価ボード **Nucleo Board STM32F401**の機能だけで上記仕様を実現しています。
[ブログ](https://mzmlab.hatenablog.com/entry/stm32-oscilloscope)で写真と動画も載せていますが、ブレッドボードにNucleoボードとTFT液晶(MSP2807)をセットするだけで動作を確認できます。  

※1 自分の用途では200kS/sで十分なのでそのように実装しましたが、ADCの性能的には1MS/s以上にすることも可能だと思います。

※2 マイコンのADCポートに直接信号を入力しているため、<u>マイコンの電源電圧3.3Vを超える信号は入力できません</u>。本来は入力回路も適切に設計する必要がありますが、そこは追々勉強していきたいと思います...


## 2. 開発環境
* 開発用PC ・・・ OS: Windows10 64bit
* 統合開発環境 ・・・ STM32CubeIDE Version:1.8.0

## 3. 注意事項
### 3.1. 自動生成コードの不具合について
STM32CubeMXの自動生成コードのうち、main.cの以下の部分は、MX_DMA_Init()とMX_SPI1_Init()の順番を入れ替えないとSPI用のDMAがうまく動作しません。

■修正前  
  /* Initialize all configured peripherals */  
  MX_GPIO_Init();  
  MX_USART2_UART_Init();  
  **MX_SPI1_Init();**  
  **MX_DMA_Init();**  
  MX_ADC1_Init();  
  MX_TIM2_Init();  
  MX_TIM3_Init();  
  MX_TIM5_Init();  

■修正後  
  /* Initialize all configured peripherals */  
  MX_GPIO_Init();  
  MX_USART2_UART_Init();  
  **MX_DMA_Init();**  
  **MX_SPI1_Init();**  
  MX_ADC1_Init();  
  MX_TIM2_Init();  
  MX_TIM3_Init();  
  MX_TIM5_Init();  

## バージョン履歴
**Ver.1.0.0**  
ブログ 「[STM32マイコンで簡易オシロスコープを自作](https://mzmlab.hatenablog.com/entry/stm32-oscilloscope)」で紹介した初期バージョン。

**Ver.1.1.0 (最新コミット)**  
液晶のタッチ操作により設定変更する機能を追加。  
オシロスコープのアプリケーション部分のコードをOscillo.cに集約。
