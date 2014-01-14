csv2ssm:北陽アプリでとったcsv形式のログデータをssmに書き込むプログラム。
	csv2ssmを走らせながらssm-logerでログをとることでcsv→ssmのログに変換する。

＊使い方＊
1. 変換するcsvファイルと同じディレクトリに以下のものを入れる。
・csv2ssm
・beta_data

2. ssm-coodinatorを起動する。
3. ./csv2ssm <filename>.csv を実行する。
4. ssm-logger -n LS3D -i 0 -l <filename>.log を実行する。
