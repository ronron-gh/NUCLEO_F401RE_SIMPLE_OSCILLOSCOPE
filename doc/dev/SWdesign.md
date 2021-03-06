# STM32 簡易オシロスコープ ソフトウェア設計

- [1. 概要](#1-概要)
- [2. ハードウェアブロック図](#2-ハードウェアブロック図)
- [3. 開発環境](#3-開発環境)
- [4. マイコンのピンアサイン](#4-マイコンのピンアサイン)
- [5. サンプリング機能](#5-サンプリング機能)
- [6. 立ち上がりエッジトリガ機能](#6-立ち上がりエッジトリガ機能)
- [7. ディスプレイ表示機能](#7-ディスプレイ表示機能)
- [8. タッチパネル機能](#8-タッチパネル機能)

## 1. 概要

* チャンネル数 : 1ch　(立ち上がりエッジでトリガ可)
* 周波数帯域 : 40kHz
* A/D分解能 : 12bit
* サンプリング周波数 : 200kS/s (※1)
* レコード長 : 300サンプル
* 最高入力電圧 : 3.3V (※2)

## 2. ハードウェアブロック図

## 3. 開発環境
* 開発用PC ・・・ OS: Windows10 64bit
* 統合開発環境 ・・・ STM32CubeIDE Version:1.8.0

## 4. マイコンのピンアサイン

■ Nucleo CN5 (digital)

| Pin | MCU pin | Function  | 用途               |
| --- | ------- | --------- | ------------------ |
| D15 | PB8     | -         | MSP2807　T_DO      |
| D14 | PB9     | -         | MSP2807　T_IRQ     |
| GND | -       | Ground    | MSP2807　GND       |
| D13 | PA5     | SPI1_SCK  | MSP2807　SCK       |
| D12 | PA6     | SPI1_MISO | MSP2807　SDO(MISO) |
| D11 | PA7     | SPI1_MOSI | MSP2807　SDI(MOSI) |
| D10 | PB6     | -         | MSP2807　CS        |
| D9  | PC7     | -         | MSP2807　DC/RS     |
| D8  | PA9     | -         | MSP2807　RESET     |


■ Nucleo CN6 (power)

| Pin  | MCU pin | Function    | 用途                        |
| ---- | ------- | ----------- | --------------------------- |
| +3V3 | -       | 3.3V output | MSP2807　LED (バックライト) |
| +5V  | -       | 5V output   | MSP2807　VCC                |

※バックライトはとりあえず3.3Vに接続しているが、GPIOに接続すればソフトウェアでバックライトをON/OFFできる。  


■ Nucleo CN8 (analog)  

| Pin | MCU pin | Function | 用途           |
| --- | ------- | -------- | -------------- |
| A0  | PA0     | ADC1_0   | 入力チャンネル |


■ Nucleo CN9

| Pin | MCU pin | Function | 用途                      |
| --- | ------- | -------- | ------------------------- |
| D7  | PA8     | -        | MSP2807　T_CLK            |
| D6  | PB10    | TIM2_CH3 | テスト用信号(10kHz矩形波) |
| D5  | PB4     | TIM3_CH1 | -                         |
| D4  | PB5     | -        | MSP2807　T_CS             |
| D3  | PB3     | TIM2_CH2 | -                         |
| D2  | PA10    | -        | MSP2807　T_DIN            |



## 5. サンプリング機能
200kHzでAD変換し、ADデータをRAMに時系列に格納する機能。次の図のように、タイマ、ADC、DMAを連携させることで、ハードウェアのみでそのようなサンプリング機能を実現する。タイマー割り込みをトリガにしてAD変換を起動し、AD変換終了をトリガにしてDMAでADデータをRAMに転送する。

![](images/%E3%82%B5%E3%83%B3%E3%83%97%E3%83%AA%E3%83%B3%E3%82%B0%E3%81%AE%E4%BB%95%E7%B5%84%E3%81%BF.png)

また、DMAをサーキュラモードにすることで、次の図のように、指定した転送数のメモリ転送を無限に繰り返す（繰り返しにより、先頭データから上書きされていく）。

![](images/%E3%82%B5%E3%83%B3%E3%83%97%E3%83%AA%E3%83%B3%E3%82%B0%E3%81%AE%E4%BB%95%E7%B5%84%E3%81%BF2.png)

## 6. 立ち上がりエッジトリガ機能
入力信号が閾値を超えたらサンプリングを停止する機能。次の図のように、ADCのアナログウォッチドッグ機能（AD変換結果が閾値を超えたら割り込みを発生）とタイマを利用して実現する。

![](images/%E3%83%88%E3%83%AA%E3%82%AC%E3%81%AE%E4%BB%95%E7%B5%84%E3%81%BF.png)

## 7. ディスプレイ表示機能
サンプリングデータをリアルタイム（10Hz程度）でディスプレイにグラフ表示する機能。MSP2807のインタフェースはSPIだが、MSP2807のサンプルプログラムをそのまま使うと、ソフトウェアで1ドットずつRGBデータをSPI送信データレジスタに書き込むため、ディスプレイの更新が非常に遅くなる。そのため、全体（320x240）のRGBデータを一旦RAM上で作成し、DMAでSPI送信データレジスタに転送するようにして高速化する。


## 8. タッチパネル機能
MSP2807のタッチパネルでオシロスコープの各種設定を行う機能。  
次の設定が可能。

* ms / div
* V / div
* トリガ機能 ON/OFF

![](images/%E3%82%BF%E3%83%83%E3%83%81%E3%83%91%E3%83%8D%E3%83%AB%E6%A6%82%E8%A6%81.jpg)