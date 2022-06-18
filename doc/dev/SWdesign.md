# STM32 簡易オシロスコープ ソフトウェア設計

* チャンネル数 : 1ch　(立ち上がりエッジでトリガ可)
* 周波数帯域 : 40kHz
* A/D分解能 : 12bit
* サンプリング周波数 : 200kS/s (※1)
* レコード長 : 300サンプル
* 最高入力電圧 : 3.3V (※2)

## 1. ハードウェアブロック図

## 2. 開発環境
* 開発用PC ・・・ OS: Windows10 64bit
* 統合開発環境 ・・・ STM32CubeIDE Version:1.8.0

## 3. マイコンのピンアサイン

■ Nucleo CN5 (digital)

|Pin|MCU pin|Function|用途|
|---|---|---|---|
|D15|PB8|-|MSP2807　T_DO|
|D14|PB9|-|MSP2807　T_IRQ|
|GND|-|Ground|MSP2807　GND|
|D13|PA5|SPI1_SCK|MSP2807　SCK|
|D12|PA6|SPI1_MISO|MSP2807　SDO(MISO)|
|D11|PA7|SPI1_MOSI|MSP2807　SDI(MOSI)|
|D10|PB6|-|MSP2807　CS|
|D9|PC7|-|MSP2807　DC/RS|
|D8|PA9|-|MSP2807　RESET|


■ Nucleo CN6 (power)

|Pin|MCU pin|Function|用途|
|---|---|---|---|
|+3V3|-|3.3V output|MSP2807　LED (バックライト)|
|+5V|-|5V output|MSP2807　VCC|

※バックライトはとりあえず3.3Vに接続しているが、GPIOに接続すればソフトウェアでバックライトをON/OFFできる。  


■ Nucleo CN8 (analog)  

|Pin|MCU pin|Function|用途|
|---|---|---|---|
|A0|PA0|ADC1_0|入力チャンネル|


■ Nucleo CN9

|Pin|MCU pin|Function|用途|
|---|---|---|---|
|D7|PA8|-|MSP2807　T_CLK|
|D6|PB10|TIM2_CH3|テスト用信号(10kHz矩形波)|
|D5|PB4|TIM3_CH1|-|
|D4|PB5|-|MSP2807　T_CS|
|D3|PB3|TIM2_CH2|-|
|D2|PA10|-|MSP2807　T_DIN|



## 4. サンプリング機能
タイマー割り込みをトリガにしてAD変換

AD変換データをDMAでRAMに転送

RAMはxxx byte。サーキュラモードでDMA転送数を自動的にリロードすることで、リングバッファのように機能する。

## 5. 立ち上がりエッジトリガ機能
ADCのアナログウォッチドッグ機能を利用

高閾値の割り込みルーチンで、サンプリング用タイマ停止用のタイマを起動

タイマ割り込みルーチンでサンプリング用タイマを停止

## 6. ディスプレイ表示機能
メインループで一定周期（約10Hz）で画面を更新。

TFT液晶モジュールへのRGBデータ送信はSPIだが、1ドットずつソフトウェアでSPIデータレジスタに書き込む方法だと時間がかかる。そのため、画面全体のRGBデータを一旦RAM上で作成し、DMAでSPIデータレジスタに転送する。ただし、画面全体(320x240)のRGBデータはRAM容量(96KB)に収まらないため、画面を10分割する。
