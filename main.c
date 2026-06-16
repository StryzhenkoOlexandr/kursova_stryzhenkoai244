#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <windows.h>

#define FILE_NAME "cartography_data.txt"
#define MAX_STR 100

typedef struct {
    char country[MAX_STR];
    char capital[MAX_STR];
    double area;
    long long population;
} Cartography;

typedef struct Node {
    Cartography data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} Queue;


void initQueue(Queue* q);
bool isEmpty(Queue* q);
void enqueue(Queue* q, Cartography data);
void dequeue(Queue* q);
void displayQueue(Queue* q);
void editElement(Queue* q);
void sortQueueByPopulation(Queue* q);
void searchCountryOrCapital(Queue* q);
void findMaxArea(Queue* q);
void calculateDensity(Queue* q);
void saveToFile(Queue* q);
void loadFromFile(Queue* q);
void clearInputBuffer();
bool getYesNoConfirmation(const char* message);
void getValidString(const char* prompt, char* buffer, int maxLength);
double getValidDouble(const char* prompt);
long long getValidLongLong(const char* prompt);



int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Queue q;
    initQueue(&q);
    loadFromFile(&q);

    int choice;
    do {
        printf("\n======================================================\n");
        printf("    ПРЕДМЕТНА ОБЛАСТЬ: КАРТОГРАФІЯ (ЧЕРГА)\n");
        printf("======================================================\n");
        printf(" 1. Додати інформацію про країну (Enqueue)\n");
        printf(" 2. Видалити країну з початку черги (Dequeue)\n");
        printf(" 3. Коригування даних країни\n");
        printf(" 4. Виведення всіх даних\n");
        printf(" 5. Сортування за кількістю жителів\n");
        printf(" 6. Пошук країни за столицею і навпаки\n");
        printf(" 7. Виведення даних про країну з найбільшою площею\n");
        printf(" 8. Розрахунок кількості жителів на одиницю площі\n");
        printf(" 9. Зберегти дані у файл\n");
        printf(" 0. Вихід\n");
        printf("------------------------------------------------------\n");
        printf("Оберіть дію: ");

        if (scanf("%d", &choice) != 1) {
            printf("Помилка: введіть числове значення!\n");
            clearInputBuffer();
            choice = -1;
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: {
                Cartography newData;
                printf("\n--- Додавання нової країни ---\n");
                getValidString("Введіть назву країни: ", newData.country, MAX_STR);
                getValidString("Введіть назву столиці: ", newData.capital, MAX_STR);
                newData.area = getValidDouble("Введіть площу території (кв. км): ");
                newData.population = getValidLongLong("Введіть кількість жителів: ");
                enqueue(&q, newData);
                printf("Успіх: Країну '%s' додано до черги.\n", newData.country);
                break;
            }
            case 2:
                dequeue(&q);
                break;
            case 3:
                editElement(&q);
                break;
            case 4:
                displayQueue(&q);
                break;
            case 5:
                sortQueueByPopulation(&q);
                break;
            case 6:
                searchCountryOrCapital(&q);
                break;
            case 7:
                findMaxArea(&q);
                break;
            case 8:
                calculateDensity(&q);
                break;
            case 9:
                saveToFile(&q);
                break;
            case 0:
                if (getYesNoConfirmation("Ви дійсно хочете вийти з програми? (Усі незбережені дані будуть втрачені)")) {
                    printf("Завершення роботи...\n");
                } else {
                    choice = -1;
                }
                break;
            default:
                printf("Помилка: Невідома команда. Спробуйте ще раз.\n");
        }
    } while (choice != 0);

    while (!isEmpty(&q)) {
        Node* temp = q.head;
        q.head = q.head->next;
        free(temp);
    }

    return 0;
}

void initQueue(Queue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

bool isEmpty(Queue* q) {
    return q->head == NULL;
}

void enqueue(Queue* q, Cartography data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Помилка виділення пам'яті!\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(q)) {
        q->head = newNode;
        q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->size++;
}

void dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Помилка: Черга порожня! Видаляти нічого.\n");
        return;
    }

    printf("\nКраїна на початку черги: %s\n", q->head->data.country);
    if (getYesNoConfirmation("Ви дійсно хочете видалити цей запис?")) {
        Node* temp = q->head;
        q->head = q->head->next;

        if (q->head == NULL) {
            q->tail = NULL;
        }

        free(temp);
        q->size--;
        printf("Успіх: Запис видалено.\n");
    } else {
        printf("Видалення скасовано.\n");
    }
}

void displayQueue(Queue* q) {
    if (isEmpty(q)) {
        printf("\nЧерга порожня!\n");
        return;
    }

    printf("\n%-20s | %-20s | %-15s | %-15s\n", "Країна", "Столиця", "Площа (кв.км)", "К-сть жителів");
    printf("--------------------------------------------------------------------------------\n");

    Node* current = q->head;
    while (current != NULL) {
        printf("%-20s | %-20s | %-15.2f | %-15lld\n",
               current->data.country,
               current->data.capital,
               current->data.area,
               current->data.population);
        current = current->next;
    }
    printf("--------------------------------------------------------------------------------\n");
    printf("Всього записів у черзі: %d\n", q->size);
}

void editElement(Queue* q) {
    if (isEmpty(q)) {
        printf("\nЧерга порожня! Немає даних для коригування.\n");
        return;
    }

    char searchCountry[MAX_STR];
    getValidString("\nВведіть назву країни для коригування даних: ", searchCountry, MAX_STR);

    Node* current = q->head;
    bool found = false;

    while (current != NULL) {
        if (strcasecmp(current->data.country, searchCountry) == 0) {
            found = true;
            printf("Знайдено країну: %s\n", current->data.country);

            getValidString("Нова столиця (або введіть -1, щоб залишити поточну): ", current->data.capital, MAX_STR);

            double newArea = getValidDouble("Нова площа (введіть -1, щоб залишити поточну): ");
            if (newArea >= 0) current->data.area = newArea;

            long long newPop = getValidLongLong("Нова кількість жителів (введіть -1, щоб залишити поточну): ");
            if (newPop >= 0) current->data.population = newPop;

            printf("Успіх: Дані оновлено!\n");
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Помилка: Країну '%s' не знайдено в черзі.\n", searchCountry);
    }
}

void sortQueueByPopulation(Queue* q) {
    if (q->size < 2) {
        printf("\nНедостатньо елементів для сортування.\n");
        return;
    }

    bool swapped;
    Node* ptr1;
    Node* lptr = NULL;

    do {
        swapped = false;
        ptr1 = q->head;

        while (ptr1->next != lptr) {
            if (ptr1->data.population > ptr1->next->data.population) {
                Cartography temp = ptr1->data;
                ptr1->data = ptr1->next->data;
                ptr1->next->data = temp;
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    printf("\nУспіх: Чергу відсортовано за зростанням кількості жителів.\n");
}

void searchCountryOrCapital(Queue* q) {
    if (isEmpty(q)) {
        printf("\nЧерга порожня!\n");
        return;
    }

    char query[MAX_STR];
    getValidString("\nВведіть назву країни АБО столиці для пошуку: ", query, MAX_STR);

    Node* current = q->head;
    bool found = false;

    printf("\nРезультати пошуку:\n");
    while (current != NULL) {
        if (strcasecmp(current->data.country, query) == 0 || strcasecmp(current->data.capital, query) == 0) {
            printf("- Країна: %s, Столиця: %s, Площа: %.2f, Жителів: %lld\n",
                   current->data.country, current->data.capital, current->data.area, current->data.population);
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        printf("Нічого не знайдено за запитом '%s'.\n", query);
    }
}

void findMaxArea(Queue* q) {
    if (isEmpty(q)) {
        printf("\nЧерга порожня!\n");
        return;
    }

    Node* current = q->head;
    Node* maxAreaNode = q->head;

    while (current != NULL) {
        if (current->data.area > maxAreaNode->data.area) {
            maxAreaNode = current;
        }
        current = current->next;
    }

    printf("\nКраїна з найбільшою площею:\n");
    printf("- Країна: %s\n- Столиця: %s\n- Площа: %.2f кв. км\n",
           maxAreaNode->data.country, maxAreaNode->data.capital, maxAreaNode->data.area);
}

void calculateDensity(Queue* q) {
    if (isEmpty(q)) {
        printf("\nЧерга порожня!\n");
        return;
    }

    char searchCountry[MAX_STR];
    getValidString("\nВведіть назву країни для розрахунку густоти населення: ", searchCountry, MAX_STR);

    Node* current = q->head;
    bool found = false;

    while (current != NULL) {
        if (strcasecmp(current->data.country, searchCountry) == 0) {
            found = true;
            if (current->data.area > 0) {
                double density = (double)current->data.population / current->data.area;
                printf("\nКраїна: %s\nГустота населення: %.2f осіб на 1 кв. км\n", current->data.country, density);
            } else {
                printf("\nПомилка: Площа дорівнює нулю, розрахунок неможливий.\n");
            }
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("Помилка: Країну '%s' не знайдено.\n", searchCountry);
    }
}


void saveToFile(Queue* q) {
    FILE* file = fopen(FILE_NAME, "w");
    if (!file) {
        printf("\nПомилка: Неможливо створити або відкрити файл для запису!\n");
        return;
    }

    Node* current = q->head;
    while (current != NULL) {
        fprintf(file, "%s;%s;%.2f;%lld\n",
                current->data.country,
                current->data.capital,
                current->data.area,
                current->data.population);
        current = current->next;
    }

    fclose(file);
    printf("\nУспіх: Дані збережено у файл '%s'.\n", FILE_NAME);
}

void loadFromFile(Queue* q) {
    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("\nФайл даних не знайдено. Буде створено новий файл при збереженні.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Cartography data;
        line[strcspn(line, "\n")] = 0;

        char* token = strtok(line, ";");
        if (token) strcpy(data.country, token);

        token = strtok(NULL, ";");
        if (token) strcpy(data.capital, token);

        token = strtok(NULL, ";");
        if (token) data.area = atof(token);

        token = strtok(NULL, ";");
        if (token) data.population = atoll(token);

        enqueue(q, data);
    }

    fclose(file);
    printf("\nУспіх: Дані завантажено з файлу '%s'.\n", FILE_NAME);
}


void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

bool getYesNoConfirmation(const char* message) {
    char response;
    while (1) {
        printf("%s (y/n): ", message);
        scanf(" %c", &response);
        clearInputBuffer();
        response = tolower(response);
        if (response == 'y') return true;
        if (response == 'n') return false;
        printf("Помилка: Введіть 'y' для ТАК або 'n' для НІ.\n");
    }
}

void getValidString(const char* prompt, char* buffer, int maxLength) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, maxLength, stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0) {
                return;
            }
        }
    }
}

double getValidDouble(const char* prompt) {
    double value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &value) == 1) {
            clearInputBuffer();
            return value;
        } else {
            printf("Помилка: Введіть коректне числове значення!\n");
            clearInputBuffer();
        }
    }
}

long long getValidLongLong(const char* prompt) {
    long long value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lld", &value) == 1) {
            clearInputBuffer();
            return value;
        } else {
            printf("Помилка: Введіть коректне ціле число!\n");
            clearInputBuffer();
        }
    }
}
