#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <unistd.h>
#include <algorithm>


int NUM_SECTIONS;                                                       // Кол-во участков леса
int vinniIsHere;                                                        // Участок, на котором находится Винни-Пух
int packNumber;                                                         // Кол-во отрядов
int lastVisited = 0;                                                    // Последне посещенный участок

bool winnieFound = false;                                               // Найден ли Винни-Пух
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int generateRandom(int a, int b) {                                      // Функция для генерации случайного чилса из диапазона [a,b]
    std::mt19937 generator;
    std::random_device device;
    generator.seed(device());
    std::uniform_int_distribution<int> distribution(a, b);

    return distribution(generator);
}

struct ForestSection {                                                  // Участок леса с переменными: исследовани ли лес и если ли на нем медведь
    bool isExplored;
    bool winniePresent;
};


std::vector<ForestSection> forest;                                      // Массив участков

void exploreSection(int sectionIndex, int threadNum) {                  // Если нашли Винни-Пуха - проводит показательное наказание
    pthread_mutex_lock(&mutex);                                      // Захватываем мьютекс и изменяем перменную winnieFound
    winnieFound = true;
    std::cout << "Стая пчел " << threadNum << " нашла Винни-Пуха на участке " << sectionIndex << std::endl;
    pthread_mutex_unlock(&mutex);
}

void *beeThread(void *arg) {                                            // Функция для исполнения потоками
    int threadNum = *reinterpret_cast<int *>(arg);                      // Номер потока
    while (true) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        if (winnieFound) {                                              // Если Винни-Пух уже найден - выходим
            pthread_mutex_unlock(&mutex);
            break;
        }
        int plot = lastVisited;
        if (plot >= NUM_SECTIONS) {                                     // Если все участки прочесаны - выходим
            pthread_mutex_unlock(&mutex);
            break;
        }
        lastVisited++;
        std::cout << "Стая пчел №" << threadNum << " исследует участок " << plot << std::endl;
        pthread_mutex_unlock(&mutex);

        if (!forest[plot].isExplored) {                                  // Исследуем участов если он еще не посещен
            forest[plot].isExplored = true;
            forest[plot].winniePresent = plot == vinniIsHere;
        }

        if (forest[plot].winniePresent) {                                // Если на этом участке находится Винни-Пух - наказываем его и сообщает другим потокам
            exploreSection(plot, threadNum);
        }
    }

    pthread_exit(nullptr);
}

int input() {                               // Функция для ввода целого числа
    std::string in;
    while (true) {
        std::cin >> in;
        auto it = std::find_if(in.begin(), in.end(), [](char const &c) {
            return !std::isdigit(c);
        });
        if (!(!in.empty() && it == in.end()) || in[0] == '0' || in[0] == '-') {
            std::cout << "Нужно ввести число из диапазона [1,1000]" << std::endl;
        } else
            break;
    }
    std::cout << std::endl;
    return std::stoi(in);
}

int main(int argc, char* argv[]) {
    system("chcp 65001");
    srand(time(nullptr));

    std::cout << "Введите количество участков леса ([1,1000]): " << std::endl;
    NUM_SECTIONS = std::stoi(argv[1]);
    std::cout << "Введите количество отрядов пчел ([1,1000]): " << std::endl;
    packNumber = std::stoi(argv[2]);
    vinniIsHere = generateRandom(0, NUM_SECTIONS - 1);
    std::cout << "Лес поделен на " << NUM_SECTIONS << " участков" << std::endl << "Всего " << packNumber
              << " отрядов пчел" << std::endl << "Винни-Пух находится на " <<
              vinniIsHere << " участке" << std::endl << std::endl;

    for (int i = 0; i < NUM_SECTIONS; i++) {
        forest.push_back({false, false});
    }
    forest[vinniIsHere].winniePresent = true;


    std::vector<pthread_t> threads(packNumber);

    for (int i = 0; i < packNumber; ++i) {                                              // Создание потоков
        pthread_create(&threads[i], nullptr, beeThread, &i);
    }

    for (int i = 0; i < NUM_SECTIONS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    std::cout << "Винни-Пух найден! Пчелы вернулись в улей." << std::endl;

    return 0;
}
