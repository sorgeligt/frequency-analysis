#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>

struct History { 
    int index[10000], j; // индексы замен в файле и размер массива
    char prev, now;
};

struct Symb_info {
    char symbol, change;
    int count;
};

int global_size = 0, count_dif_symb = 0, history_top = 0;
char spectrum[] = "оеаинтрслвкпмудяыьзбгйчюхжшцщфэъ", alphabet[] = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
struct Symb_info symb[32];
struct History list[35];

int is_decode(char c) { // проверка на заглавные буквы
    return ((c >= 'А') && (c <= 'Я'));
}

int is_letter(char c) { // проверка на буквы русского языка
    return (((c >= 'А') && (c <= 'Я')) || ((c >= 'а') && (c <= 'я')));
}

int cmp(const void* a, const void* b) { // компаратор
    const struct Symb_info* A = (const struct Symb_info*)a;
    const struct Symb_info* B = (const struct Symb_info*)b;
    if (A->count < B->count)
        return 1;
    else if (A->count > B->count)
        return -1;
    else 
        return 0;
}

void analysis(struct Symb_info* symb, char* p) {
    for (int i = 0; i < 32; i++) { // начальная инициализация структуры
        symb[i].symbol = alphabet[i];
        symb[i].count = 0;
    }
    for (int i = 0; i < global_size; i++) // подсчет встречающихся букв
        if (is_decode(p[i])) symb[p[i] - 'А'].count++;
    qsort(symb, 32, sizeof(struct Symb_info), cmp); // сортировка по полю count
    for (int i = 0; i < 32 && symb[i].count != 0; ++i) 
        count_dif_symb = i + 1; // кол-во различных чисел в файле
    for (int i = 0; i < count_dif_symb; ++i) 
        symb[i].change = spectrum[i]; // соответвие букв в соответствии с частотным анализом
}

void auto_decode(struct Symb_info* symb, char* p) { // автоматическая расшифровка
    for (int i = 0; i < global_size; i++)
        for (int j = 0; j < count_dif_symb; ++j)
            if (symb[j].symbol == p[i]) p[i] = symb[j].change;
}

void analysis_print(struct Symb_info* symb) { // вывод анализа
    for (int i = 0; i < count_dif_symb; ++i)
        printf("%lf: %c -> %c\n", symb[i].count * 1.0 / (global_size - 1), symb[i].symbol, symb[i].change);
}

void selection_size_n(char* p, char check) { // check: 'a' = all, 'd' = decode
    int cnt=0, n, move=0, j; //move: при подсчете нерасшифрованных символов отвечает за кол-во маленьких букв
    printf("Необходимое кол-во символов: ");
    scanf("%d", &n);
    for (int i = 0; i < global_size; i++) {
        if (check == 'd' && is_letter(p[i]) && !is_decode(p[i])) {
            move++; 
            continue; // пропускаем маленькие буквы при подсчете нерасшифрованных
        }
        if (check == 'a' ? is_letter(p[i]) : is_decode(p[i])) 
            cnt++;
        else if ((cnt == n && move != 0) || (cnt == n && n != 0)) { // при достижении нужной длины выводим ответ с учетом 2 вариантов работы функции
            for (j = i - cnt - move; j < i; j++) 
                printf("%c", p[j]);
            printf("\n"); 
            cnt = 0; 
            move = 0;
        }
        else {
            cnt = 0; 
            move = 0;
        }
    }
}

void crypto_print(char* p) { // вывод криптограммы
    for (int i = 0; i < global_size; i++) 
        printf("%c", p[i]);
    printf("\n");
}

void replace_letters(char a, char b, char* p, struct History *list) {
    if (!is_decode(a) || is_decode(b) || !is_letter(b) || !is_letter(a)) {
        printf("Ошибка! Введите 1 символ заглавным, а 2 строчным вводами\nПример:Ы и\n");
        return;
    }
    int j = 0;
    for (int i = 0; i < global_size; i++){
        if (a == p[i] && is_decode(p[i])) { // меняем нерасшифрованные символы и заносим изменения в историю
            list[history_top].prev = a;
            list[history_top].now = p[i] = b;
            list[history_top].index[j++] = i;
        }
    }
    list[history_top++].j = j; // обновляем размер массива отвечающего за производимые изменения на данном шаге
}

void hitory_list(struct History* list) { // вывод истории
    for (int i = 0; i < history_top; i++) 
        printf("%c -> %c\n", list[i].prev, list[i].now);
}

void undo(char* p, struct History* list) { // откат
    if (history_top == 0) {
        printf("Ошибка! История пуста\n");
        return;
    }
    for (int i = 0; i < list[history_top-1].j; i++) 
        p[list[history_top-1].index[i]] = list[history_top-1].prev;
    list[history_top--].j = 0;
}

int user_communication() { // интерфейс взаимодействия с пользвателем
    int n;
    printf("Выберите:\n1) Анализ\n2) Авторасшифровка\n3) Слова по кол-ву букв\n4) Слова по кол-ву нерасшифрованных букв\n");
    printf("5) Отображение криптограммы\n6) Замена букв\n7) История\n8) Отмена последней замены\n9) Выход\n");
    scanf("%d", &n);
    return n;
}

int main() {
    SetConsoleCP(1251); // установка win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); // установка win-cp 1251 в поток вывода
    setlocale(LC_ALL, "Russian"); // задаем локаль используемую программой
    FILE* f = fopen("lab_var_5.txt", "r");
    if (f == NULL) {
        printf("Ошибка! Такого файла нет в директории");
        return 0;
    }
    char *p, c;
    p = (char*)malloc(sizeof(char)); // выделяем блок памяти для массива символов
    while (fscanf(f, "%c", &c) != EOF) { // считываем символы с файла занося в массив
        p[global_size++] = c;
        p = (char*)realloc(p, sizeof(char) * (global_size + 5));
    }
    if (global_size == 0) {
        printf("Ошибка! Файл пуст");
        return 0;
    }
    p[global_size++] = '\0';
    analysis(symb, p); // предварительные инициализации и анализ файла
    int step = user_communication();
    while (step != 9) { // интерфейс взаимодействия
        if (step == 1) 
            analysis_print(symb);
        if (step == 2)
            auto_decode(symb, p);
        if (step == 3) 
            selection_size_n(p, 'a');
        if (step == 4)
            selection_size_n(p, 'd');
        if (step == 5) 
            crypto_print(p);
        if (step == 6) {
            char a[10], b[10];
            printf("Введите две буквы через пробел:"); 
            scanf("%s %s", a, b);
            replace_letters(a[0], b[0], p, list);
        }
        if (step == 7) hitory_list(list);
        if (step == 8) undo(p, list);
        if (step == 9) {
            free(p); 
            return 0;
        }
        step = user_communication();
    }
    free(p); // освобождение памяти
    return 0;
}
