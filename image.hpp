#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

// Структура для хранения цвета пикселя (RGB)
struct Color {
    uint8_t r; // Красный цвет (0-255)
    uint8_t g; // Зеленый цвет (0-255)
    uint8_t b; // Синий цвет (0-255)

    // Конструктор по умолчанию (черный цвет)
    Color() : r(0), g(0), b(0) {}

    // Конструктор с параметрами для инициализации цвета
    Color(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
};

// Тип данных для представления изображения
using Image = std::vector<std::vector<Color>>;

#include <cstdlib>
#include <ctime>

// Функция для генерации случайного изображения заданных размеров
Image generateRandomImage(size_t width, size_t height) {
    Image image(height, std::vector<Color>(width));
    std::srand(std::time(0)); // Инициализация генератора случайных чисел
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            image[y][x] = Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);
        }
    }
    return image;
}

#include <numeric>

// Функция для вычисления среднего цвета в окрестности 3x3
Color computeAverageColor(const Image& img, int x, int y) {
    int sumR = 0, sumG = 0, sumB = 0;
    int count = 0;

    // Проходим по пикселям в окрестности 3x3
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = x + dx;
            int ny = y + dy;

            // Проверка на выход за пределы изображения
            if (nx >= 0 && ny >= 0 && nx < img[0].size() && ny < img.size()) {
                sumR += img[ny][nx].r;
                sumG += img[ny][nx].g;
                sumB += img[ny][nx].b;
                count++;
            }
        }
    }

    // Возвращаем усредненный цвет
    return Color(static_cast<uint8_t>(sumR / count), static_cast<uint8_t>(sumG / count), static_cast<uint8_t>(sumB / count));
}

// Последовательное размытие изображения
Image SequentialBlur(const Image& input) {
    Image result = input; // Создаем копию для результата
    // Обрабатываем каждый пиксель изображения
    for (size_t y = 0; y < input.size(); ++y) {
        for (size_t x = 0; x < input[y].size(); ++x) {
            result[y][x] = computeAverageColor(input, x, y);
        }
    }
    return result;
}

#include <thread>
#include <mutex>

// Параллельное размытие с использованием std::thread
Image ParallelBlur(const Image& input, int numThreads = 4) {
    Image result = input;
    std::vector<std::thread> threads;
    int imgHeight = input.size();
    int stripHeight = imgHeight / numThreads;

    // Функция для размытия в пределах полосы
    auto blurTask = [&](int startY, int endY) {
        for (int y = startY; y < endY; ++y) {
            for (size_t x = 0; x < input[y].size(); ++x) {
                result[y][x] = computeAverageColor(input, x, y);
            }
        }
        };

    // Запускаем потоки для обработки разных частей изображения
    for (int i = 0; i < numThreads; ++i) {
        int startY = i * stripHeight;
        int endY = (i == numThreads - 1) ? imgHeight : (i + 1) * stripHeight;

        threads.emplace_back(blurTask, startY, endY);
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}

#include <chrono>

// Функция для измерения времени выполнения
template<typename Func>
void measureExecutionTime(const std::string& taskName, Func func) {
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << taskName << " executed in " << duration.count() << " ms\n";
}

#include <atomic>

// Демонстрация использования атомарных операций
void demonstrateAtomicOperations() {
    const int numIterations = 1000000;
    const int numThreads = 4;

    // Версия с мьютексом
    {
        int counter = 0;
        std::mutex counterMutex;
        std::vector<std::thread> threads;

        auto task = [&]() {
            for (int i = 0; i < numIterations; ++i) {
                std::lock_guard<std::mutex> lock(counterMutex);
                counter++;
            }
        };

        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(task);
        }
        for (auto& thread : threads) {
            thread.join();
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "Mutex-based counter: " << counter
            << ", Time: " << duration.count() << " ms\n";
    }

    // Версия с atomic
    {
        std::atomic<int> counter(0);
        std::vector<std::thread> threads;

        auto task = [&]() {
            for (int i = 0; i < numIterations; ++i) {
                counter++;
            }
            };

        auto startTime = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(task);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "Atomic counter: " << counter
            << ", Time: " << duration.count() << " ms\n";
    }
}

#include <windows.h> // Для цветного вывода в Windows

// Функция для вывода изображения в консоль с цветным отображением
void displayImageWithColor(const Image& img, int maxSize = 10) {
    int size = 10; // Ограничение для вывода изображения

    for (int y = 0; y < size; ++y) {
        int width = 10; // Ограничение ширины вывода

        for (int x = 0; x < width; ++x) {
            const Color& pixelColor = img[y][x];

            // Устанавливаем цвет для консоли
            HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            WORD color = (pixelColor.r > 128 ? 4 : 0) | (pixelColor.g > 128 ? 2 : 0) | (pixelColor.b > 128 ? 1 : 0);
            SetConsoleTextAttribute(consoleHandle, color);
            SetConsoleOutputCP(CP_UTF8);
            std::cout << ".";
        }

        // Восстанавливаем стандартный цвет
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::cout << "\n";
    }
}
