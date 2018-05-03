# マルチメディア信号解析 課題1

水曜3限のマルチメディア信号解析の課題1のプログラムです．  
C言語を用いて実装しました．

## コンパイルと実行の手順

```
cd src
make
cd ..
./findit ./template/usagi.ppm ./ppm/class1_b1_n0_1.ppm result_usagi_class1_b1_n0_1.ppm
```

## 実行結果の例

```
./template/usagi.ppm is successfully searched in ./ppm/class1_b1_n0_1.ppm
processing time: 3.329000 [s]
location ( 50.0,170.0), rotation    0.0, scaling 1.00
result image is saved in result_usagi_class1_b1_n0_1.ppm
```

## ppm 形式の画像の扱い方

ppm 形式の画像の閲覧や他の形式の画像への変換を行う場合，以下のソフトを使うと良い．

* IrfanView （Windowsのみ）  
* GIMP  
* ImageMagick （コマンドラインのみ）
