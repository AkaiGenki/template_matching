/*
 *  このプログラムは, あらかじめ与えられた画像パターン(以下この画像の
 *  ことをテンプレート画像と呼びます)を, 別の大きな画像(以下この画像のこと
 *  を探索対象画像と呼びます)から探します. 見付かった結果は, 探索対象画像の
 *  上にテンプレート画像の外枠に相当する長方形を重ね書きすることで,
 *  見えるようにします.
 *
 *  画像は, netpbm (pbmplus) で用いられるカラー画像フォーマット `ppm' を
 *  用いています.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "pnmimg.h"

#ifdef __STDC__
int
main( int argc, char *argv[] )
#else
int
main( argc, argv )
int argc ;
char *argv[] ;
#endif
{
    /*
     *  このプログラムは３つのコマンドライン引数を指定して動作します.
     *  第１の引数は, テンプレートの画像ファイル(PPM フォーマット)であり,
     *  第２の引数は, 探索対象画像のファイルです. 引数が省略された場合には,
     *  第１引数の代わりに "template.ppm" という名前が, 第２引数の代わりに
     *  "image.ppm" という名前が用いられます.
     *  更に, 第３の引数を指定すると, 探索結果を出力する画像ファイルの
     *  名前を指定することも出来ます. 指定しなければ, "result.ppm" という
     *  名前が用いられます.
     */
    char *name_tmp = "template.ppm" ;
    char *name_img = "image.ppm" ;
    char *name_out = "result.ppm" ;
    RGB_PACKED_IMAGE *template ;
    RGB_PACKED_IMAGE *image ;
    double cx, cy, rotation, scaling ;
    int icx, icy, irot, isx, isy ;
    int ret ;
    extern int findPattern __P(( RGB_PACKED_IMAGE*, RGB_PACKED_IMAGE*, double*, double*, double*, double* )) ;

    if ( argc > 1 ) name_tmp = argv[1] ;
    if ( argc > 2 ) name_img = argv[2] ;
    if ( argc > 3 ) name_out = argv[3] ;

    if (!( template = readRGBPackedImage( name_tmp ))) {
        printError( name_tmp ) ;
        return (1) ;
    }
    if (!( image = readRGBPackedImage( name_img ))) {
        printError( name_img ) ;
        return (1) ;
    }

    /*
     *  ２枚の画像が読み込まれました. ここから, これらの画像を
     *  パターンを探索する関数 findPattern() に渡して,
     *  探索を行います. 結果は見付かったテンプレートの画像の
     *  中心の座標 ( cx, cy ) とテンプレートの回転角 rotation,
     *  および拡大/縮小率 scaling とに入れて, 返されます. これらの
     *  変数は double 表現です. また, scaling は見付けたパターンの
     *  大きさとテンプレートの大きさとの比で表現し, rotation は
     *  画面に対して反時計周りを正とする degree (度)単位で表現します.
     *  この関数は, うまく見付かった時に NO_ERROR (0) を返し,
     *  そうでなければ HAS_ERROR (-1) を返すようになっています.
     */

    ret = findPattern( template, image,
                       &cx, &cy, &rotation, &scaling ) ;
    if ( ret == HAS_ERROR ) {
        printError( "findPattern" ) ;
        printf( "%s is failed to be searched in %s\n", name_tmp, name_img ) ;
        return (1) ;
    }

    /*
     *  探索した結果を元の探索画像の上に長方形の枠で表示します.
     */
    icx = (int)( cx + .5 ) ;
    icy = (int)( cy + .5 ) ;
    isx = (int)( scaling * template->cols + .5 ) ;
    isy = (int)( scaling * template->rows + .5 ) ;
    irot = (int)( rotation + .5 ) ;
    setRGBMarkColor( 255, 255, 0 ) ;
    drawRotatedRectangleRGBPackedImage( image, icx, icy, isx, isy, irot ) ;

    /*
     *  結果を端末に出力し, 画像をファイルにセーブします.
     */
    printf( "%s is successfully searched in %s\n", name_tmp, name_img ) ;
    printf( "location (%5.1lf,%5.1lf), rotation %6.1lf, scaling %4.2lf\n",
            cx, cy, rotation, scaling ) ;

    if ( writeRGBPackedImage( image, name_out ) == HAS_ERROR ) {
        printError( name_out ) ;
        return (1) ;
    }
    printf( "result image is saved in %s\n", name_out ) ;

    return (0) ;
}
