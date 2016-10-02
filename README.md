wslib
===

いろいろ作ってるうちに毎回ライブラリを自作するのに疲れてきたのであんまり変更しない部分をまとめておきました.
なるべくクセのない仕様を心がけているので,自分で作るのが面倒なときとか,自由に使ってください.

## Description
C言語は非常にコンパクトな言語なので最低限のデータ型しか用意されていません.
高度なことをやろうと思ったらデータ構造を自作するしかありませんが,毎回それをやるのは面倒です.
C++には膨大なライブラリが用意されているので普通はそちらを使えばいいのですが,
なんらかの理由でどうしてもC++を使いたくないとか使えないときにこのライブラリを使うといいかもしれません.
あるいはデータ構造の勉強をしたけど実際のコードがなかなか手に入らないというときに参考にしてください.
現在使うことのできるデータ構造は以下の通りです.

* ByteString
* List
* Object/Type

もっと詳しく知りたい場合はこのREADMEの一番最後に
各データごとの詳細が列挙してあるのでそちらを参照してください.

## Demo
このリポジトリをクローンして `make test` コマンドでコンパイルし,生成された実行ファイル `test` を実行してみてください.
いろいろなモジュールが実際に動作している様子を確認できます. `test.c` は使用法の参考にしてください.

## Requirement
C11でコンパイルしてください. テスト済み環境は Ubuntu 16.04 LTS / gcc version 5.4.0 です. 

## Usage
基本的には `module.c` と `module.h` で1セットです. 好きなモジュールをコピーして,ご自分のプログラムと
いっしょにコンパイルしてください. 特に明記されていない限りモジュール単位で独立しているので依存の解決は
必要ありません. C11でないとコンパイルできない可能性があります. 適宜コンパイルオプションに `-std=C11` を
指定するなどしてください.

`make wslib.so` を実行するとすべてのモジュールをコンパイルして共有オブジェクトファイル `wslib.so` を生成します.
このときは `wslib.h` をインクルードして `wslib.so` をリンクするだけですべてのモジュールにアクセスできます.
モジュールを選ぶのが面倒なときや,万一モジュールどうしの依存にはまったときはこの方法をおすすめします.

## Install
以下のコマンドでコピーしたディレクトリの中身を開発中のプロジェクトに適宜追加してください.

```
$ git clone git@github.com:wsuzume/wslib.git
```

## ByteString
文字列型です. 以下のように定義されています.

```
typedef struct bytestring_t* ByteString;
struct bytestring_t {
    size_t size;
    char *data;
};
```

`size` には末尾のヌル文字を除いて格納できる最大文字数が保存されます.
`data` には文字列先頭へのポインタが格納されます.
つまりdataのインデックスにsizeを指定するとそれは必ず末尾のヌル文字を指しているはずであり,
以下のコードでputs関数は必ず実行されるべきです.

```
ByteString str = constByteString("Hello, World!");

if (str->data[str->size] == '\0') {
    puts("This statement MUST be always true.");    
}

str = eraseByteString(str);
```

もしもこのputs関数が実行されないようにデータを改変するとwslibの関数群はバッファオーバーランを引き起こす可能性があります.
ご自分でこの文字列型を操作する関数を実装するときは十分に注意してください.

文字列の生成は主に以下のいずれかの方法で行います.

```
ByteString str1 = allocByteString(n);     //n文字(+ヌル文字1文字)分の領域を生成. 先頭と末尾に'\0'を代入.
ByteString str2 = initByteString(n);      //n文字(+ヌル文字1文字)分の領域を生成. 全体を'\0'で初期化.
ByteString str3 = makeByteString(str, n); //(const char *)strの先頭からn文字をコピーして生成.
ByteString str4 = constByteString(str);   //(const char *)strの全体をコピーして生成.
ByteString str5 = cropByteString(str);    //(ByteString)strの全体をコピーして生成.
```

他にもファイルや標準入力から生成する便利な関数があります.
詳細はwikiを参照し用途に応じて適宜使い分けてください.

`eraseByteString`関数は必ず`NULL`を返却します. 一方で引数に`NULL`を与えても何もしないため,
`eraseByteString`関数の結果を,削除した文字列自身に代入することでメモリの二重解放を防ぐことができます.
つまり `str = eraseByteString(str);` という式は何度実行してもエラーにはなりません.
しかしもしも一度イレースした文字列をもう一度イレースするとメモリの二重解放によってプログラムはクラッシュします.
結果を代入しない `eraseByteString(str);` だけの式は二回目の実行でクラッシュするので注意してください.

`eraseByteString`関数はポインタを意識させないためにこのような設計になっています.
もしもポインタを使うことの煩雑さが苦でないなら`freeByteString`関数を使用して解放するのもよいでしょう.

```
void freeByteString(ByteString *str)
{
    if (str == NULL) return;
    if (*str != NULL) *str = eraseByteString(*str);
    return; 
}
```

`freeByteString(&str);`という式は何度実行してもエラーにはなりません.
つまり文字列の破棄は以下のどちらかの方法で行うことが強く推奨されます.

```
str = eraseByteString(str);
freeByteString(&str);
```

もっとも注意すべき事項は以上です. この他に定義された関数群と使用方法に関しては wiki を参照してください.

## Pair
uintptr\_t型の二つ組のタプルです.

## List
リストです. 内部的にはPair型を用いて実装されています.

## Object/Type
オブジェクトの型を実行時にも保存するObject型が使用可能になります.
オブジェクトの型はType型で一意に識別され,各種関数を統一的に扱うことができるようになります.
wslibで定義されているデータ型についても拡張されているので,Object型を使いたいときは
`make wslib.so` で全体をコンパイルし,共有オブジェクトをリンクしてください.
Object型に保証されている関数は以下の通りです.

* nameof\_type
* sizeof\_type
* alignof\_type
* typeof
* dataof
* create
* delete
* copy
* crop
* erase
* deepcrop
* deeperase
