#include <stdio.h>
#include <string.h>
#include <time.h>

#define CSV_FILE  "error_log.csv"  // データファイル名
#define MAX_LEN   256              // 文字列の最大長

// エラー情報を表す構造体
typedef struct {
    char date[32];          // 発生日時（自動取得）
    char machine[MAX_LEN];  // 機械名（例：1号機、スリーブ、メッキナット）
    char product[MAX_LEN];  // 製品名/品種（例：F14、06C）
    char category[MAX_LEN]; // 種別（例：画像調整、機械異常、寸法不良、搬送異常）
    char content[MAX_LEN];  // 内容（自由記述）
    char location[MAX_LEN]; // 場所（例：反転部、爪搬送、底厚検査部）
    char action[MAX_LEN];   // 対処法（不明な場合は空白可）
    char note[MAX_LEN];     // 備考（任意）
} ErrorLog;

// 現在の日時を文字列で取得する
void get_current_time(char *datetime) {
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    sprintf(datetime, "%d/%02d/%02d %02d:%02d:%02d",
        now->tm_year + 1900,
        now->tm_mon  + 1,
        now->tm_mday,
        now->tm_hour,
        now->tm_min,
        now->tm_sec
    );
}

// 改行を除去する
void remove_newline(char *dest) {
    int len = strlen(dest);
    if (len > 0 && dest[len - 1] == '\n')
        dest[len - 1] = '\0';
}

// 1項目入力（空白可）
void input_field(const char *label, char *dest) {
    printf("%s（Enterのみで空白）：", label);
    fgets(dest, MAX_LEN, stdin);
    remove_newline(dest);
}

// エラー情報をCSVに登録する
void register_error(void) {
    FILE *fp;
    ErrorLog log;

    // 日時を自動取得
    get_current_time(log.date);

    printf("\n--- エラー登録 ---\n");
    input_field("機械名　　", log.machine);
    input_field("製品/品種　", log.product);
    input_field("種別　　　", log.category);
    input_field("内容　　　", log.content);
    input_field("場所　　　", log.location);
    input_field("対処法　　", log.action);
    input_field("備考　　　", log.note);

    // 追記モードでオープン
    if ((fp = fopen(CSV_FILE, "a")) == NULL) {
        printf("\aファイルをオープンできません。\n");
        return;
    }

    // CSVに書き込む
    fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s\n",
        log.date,
        log.machine,
        log.product,
        log.category,
        log.content,
        log.location,
        log.action,
        log.note
    );

    fclose(fp);
    printf("登録しました。\n");
}

// 1行を整形して表示する
void print_line(const char *line) {
    char buf[MAX_LEN * 8];
    char *fields[8];
    int i = 0;

    strncpy(buf, line, sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    // カンマで分割
    fields[0] = strtok(buf, ",");
    for (i = 1; i < 8; i++) {
        fields[i] = strtok(NULL, ",");
        if (fields[i] == NULL) fields[i] = "";
    }

    printf("日時　：%s\n", fields[0]);
    printf("機械　：%s\n", fields[1]);
    printf("品種　：%s\n", fields[2]);
    printf("種別　：%s\n", fields[3]);
    printf("内容　：%s\n", fields[4]);
    printf("場所　：%s\n", fields[5]);
    printf("対処法：%s\n", fields[6]);
    printf("備考　：%s\n", fields[7]);
    printf("----------------------------------------\n");
}

// キーワードでCSVを検索して表示する
void search_error(void) {
    FILE *fp;
    char keyword[MAX_LEN];
    char line[MAX_LEN * 8];
    int found = 0;

    printf("\n--- エラー検索 ---\n");
    printf("キーワード（機械名・品種・種別など）：");
    fgets(keyword, MAX_LEN, stdin);
    remove_newline(keyword);

    // 読み取りモードでオープン
    if ((fp = fopen(CSV_FILE, "r")) == NULL) {
        printf("データがありません。\n");
        return;
    }

    printf("\n--- 検索結果 ---\n");

    // 1行ずつ読み込んでキーワードを検索
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, keyword) != NULL) {
            print_line(line);
            found++;
        }
    }

    if (found == 0)
        printf("該当するデータがありませんでした。\n");
    else
        printf("%d件見つかりました。\n", found);

    fclose(fp);
}

int main(void) {
    int select;

    printf("=== 製造現場エラー管理システム ===\n");
    printf("※種別の例：画像調整 / 機械異常 / 寸法不良 / 搬送異常\n");

    while (1) {
        printf("\n1.エラー登録  2.キーワード検索  0.終了\n");
        printf("選択：");
        scanf("%d", &select);
        getchar(); // 改行を捨てる

        switch (select) {
            case 1: register_error(); break;
            case 2: search_error();   break;
            case 0:
                printf("終了します。\n");
                return 0;
            default:
                printf("0〜2で入力してください。\n");
        }
    }

    return 0;
}